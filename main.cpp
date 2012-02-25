#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <getopt.h>
#include <unistd.h>

#include "Constants.hpp"
#include "Rectangle.hpp"
#include "RectangleDetector.hpp"
#include "CommLink.hpp"

using namespace cv;
using namespace std;

const char* wndname = "Rectangle Detector";

// the function draws all the rectangles in the image
void drawRectangles(Mat& image, const vector< vector<Point> > &allRectangles, const vector< vector<Point> >& finalRectangles)
{
  for (size_t i = 0; i < allRectangles.size(); i++) {
    const Point* p = &allRectangles.at(i).at(0);
    int n = (int)allRectangles.at(i).size();
    polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, CV_AA);
  }

  for (size_t i = 0; i < finalRectangles.size(); ++i) {
    const Point* p=&finalRectangles.at(i).at(0);
    int n=(int)finalRectangles.at(i).size();
    polylines(image, &p, &n, 1, true, Scalar(255, 0, 0), 3, CV_AA);
  }

  imshow(wndname, image);
}

int main(int argc, char* argv[])
{
  
  Constants constList;
  VideoCapture cap;
  stringstream fileName;
  stringstream deviceName;
  int isFile = 0;
  int isDevice = 0;
  int isHD = 0;
  int isHeadless = 0;
  int isNetworking = 1;
  int firstRun = 1;

  while (true) {
    static struct option long_options[] = {
      {"high-definition", no_argument, &isHD, 1},
      {"headless", no_argument, &isHeadless, 1},
      {"no-networking", no_argument, &isNetworking, 0},
      {"device", required_argument, 0, 'd'},
      {"file", required_argument, 0, 'f'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "d:f:", long_options, &option_index);

    if (c == -1) // End of Options
      break;

    switch (c) {
    case 0:
      break;
    case 'd':
      deviceName <<optarg;
      isDevice = 1;
      break;
    case 'f':
      fileName <<optarg;
      isFile = 1;
    case '?':
      exit(127);
      break;
    default:
      abort();
    }
  }

  if (! isFile && ! isDevice) {
    cout <<argv[0] <<": missing required options" <<endl;
    exit(127);
  }
  
  if (isDevice) {
    // Get Video Capture Device
    cap.open(atoi(deviceName.str().c_str()));
    if (!cap.isOpened()) {
      cerr <<"Unable to open capture device " <<deviceName <<"." <<endl;
      return -1;
    }
    if (isHD) {
      // Set Capture Resolution
      cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1920);
      cap.set(CV_CAP_PROP_FRAME_WIDTH, 1080);

      // Change Camera Data
      constList.cameraFocalLength = constList.cameraHDFocalLength;
      constList.cameraViewingAngle = constList.cameraHDViewingAngle;
    }
  }

  CommLink commLink;
  if (isNetworking)
    commLink.initServer(); // Initialize Communication Link

  if (! isHeadless)
    namedWindow(wndname, 0);
  
  while (true)
    {
      vector< vector<Point> > allRectangles;
      vector< vector<Point> > finalRectangles;
      int aquired = 0;
      int distanceMM;
      float azimuthRadians;
      float azimuthDegrees;
      Mat original;
      Mat output;

      if (isNetworking)
	commLink.waitForPing();

      if (isFile)
	original = imread(fileName.str().c_str()); // Load Image from File
      else
	cap >>original; // Load Image from Video Capture Device

      // Print Variables
      if (firstRun && ! isHeadless)
	cout <<"Camera Resolution: " <<original.cols <<"x" <<original.rows <<endl;

      // Get Distance and Azimuth
      RectangleDetector rectDetector(original);
      if (rectDetector.rectangleWasFound()) {
	aquired = 1;
	
	// Retrieve Data
	distanceMM = rectDetector.getDistance();
	azimuthRadians = rectDetector.getAzimuth();
	allRectangles = rectDetector.getAllRectangles();
	finalRectangles = rectDetector.getFinalRectangles();

	// Convert Data
	azimuthDegrees = (azimuthRadians * 180.0) / constList.mathPi;
	
	// Print Data
	if (! isHeadless) {
	  cout <<"Distance: " <<distanceMM <<"mm" <<endl;
	  cout <<"Azimuth: " <<azimuthDegrees <<" degrees, " <<azimuthRadians <<" radians" <<endl;
	}
	
	// Send Data
	if (isNetworking)
	  commLink.sendData(distanceMM, 0, azimuthRadians, 0.0);
      }
      else {
	// Print Data
	if (! isHeadless)
	  cout <<"No rectangle" <<endl;

	// Send Data
	if (isNetworking)
	  commLink.sendData();
      }
      
      // Write Data to Original Image
      int dataPointX = 0;
      int dataPointY = original.rows-5;
      Point dataCoordinates(dataPointX, dataPointY);
      int fontFace = FONT_HERSHEY_COMPLEX;
      double fontScale = (float)original.cols / 400.0;
      Scalar fontColor(0.0, 255.0, 0.0, 0.0);
      int fontThickness = 2;
      stringstream data;
      if (aquired)
	data <<distanceMM <<"mm @ " <<azimuthDegrees <<" degrees";
      else
	data <<"No rectangle";
      putText(original, data.str(), dataCoordinates, fontFace, fontScale, fontColor, fontThickness);

      if (! isHeadless)
	drawRectangles(original, allRectangles, finalRectangles); // Draw Rectangles and Display Image

      firstRun = 0;

      if (! isHeadless) {
	int keycode = waitKey(10);
	if (keycode == 120) {
	  stringstream filename;
	  filename <<time(NULL) <<".jpg";
	  imwrite(filename.str().c_str(), output);
	}
	else if (keycode == 27)
	  break;
	if (isFile) {
	  waitKey();
	  break;
	}
      }
    }
  
  return 0;
}

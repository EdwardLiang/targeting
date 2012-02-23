#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <sstream>

#include "Rectangle.hpp"

using namespace cv;
using namespace std;

Rectangle::Rectangle(vector<Point> input) {
  // Populate Points
  int minLengthSquared=6e6;
  int maxLengthSquared=0;
  int indexTopLeft=-1;
  int indexTopRight=-1;
  int indexBottomRight=-1;
  int indexBottomLeft=-1;
  for (unsigned i=0; i < input.size(); ++i) {
    int tmpLengthSquared=pow(input.at(i).x, 2) + pow(input.at(i).y, 2);
    if (tmpLengthSquared < minLengthSquared) {
      minLengthSquared=tmpLengthSquared;
      indexTopLeft=i;
    }
    if (tmpLengthSquared > maxLengthSquared) {
      maxLengthSquared=tmpLengthSquared;
      indexBottomRight=i;
    }
  }
  for (unsigned i=0; i < input.size(); ++i) {
    if (i==indexTopLeft || i==indexBottomRight) continue;

    if (indexTopRight < 0)
      indexTopRight=i;
    else if (indexBottomLeft < 0)
      indexBottomLeft=i;
  }

  if (input.at(indexTopRight).x < input.at(indexBottomLeft).x) {
    Point swap=input.at(indexTopRight);
    input.at(indexTopRight)=input.at(indexBottomLeft);
    input.at(indexBottomLeft)=swap;
    swap=input.at(indexTopRight);
    input.at(indexTopRight)=input.at(indexBottomLeft);
    input.at(indexBottomLeft)=swap;
  }

  topLeft=input.at(indexTopLeft);
  topRight=input.at(indexTopRight);
  bottomRight=input.at(indexBottomRight);
  bottomLeft=input.at(indexBottomLeft);


  // Compute Lengths
  lengthSquaredLeft=pow((topLeft.x - bottomLeft.x), 2) + pow((topLeft.y - bottomLeft.y), 2);
  lengthSquaredRight=pow((topRight.x - bottomRight.x), 2) + pow((topRight.y - bottomRight.y), 2);
  lengthSquaredTop=pow((topLeft.x - topRight.x), 2) + pow((topLeft.y - topRight.y), 2);
  lengthSquaredBottom=pow((bottomLeft.x - bottomRight.x), 2) + pow((bottomLeft.y - bottomRight.y), 2);

  // Compute Center
  center.x=(topLeft.x + topRight.x + bottomRight.x + bottomLeft.x) / 4;
  center.y=(topLeft.y + topRight.y + bottomRight.y + bottomLeft.y) / 4;
}

bool Rectangle::containsPoint(Point input) {
  return (input.x > topLeft.x && input.x < bottomRight.x &&
	  input.y > topLeft.y && input.y < bottomRight.y);
}
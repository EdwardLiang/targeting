/*
 *    This file is part of FRC Team 3341 Targeting.
 *
 *    FRC Team 3341 Targeting is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    FRC Team 3341 Targeting is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with FRC Team 3341 Targeting.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <set>
#include <thread>
#include <memory>
#include <getopt.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/lexical_cast.hpp>

#include "Constants.hpp"
#include "VideoDevice.hpp"
#include "Rectangle.hpp"
#include "RectangleDetector.hpp"
#include "RectangleProcessor.hpp"
#include "MultiRectangleProcessor.hpp"
#include "GUIManager.hpp"
#include "CmdLineInterface.hpp"
#include "Application.hpp"

Application::Application(int argc, char *argv[])
{
    constList = new Constants();

    cmdLineInterface = new CmdLineInterface(argc, argv);
    config = cmdLineInterface->getConfig();
    delete cmdLineInterface;
}

Application::~Application()
{
    if (!config.getIsHeadless())
        delete guiManager;
    if (config.getIsNetworking())
        delete networkController;
    if (config.getIsDevice())
        delete videoDevice;
    delete constList;
}

int Application::exec()
{
    initVideoDevice();
    initGUI();
    if (config.getIsCaptureMode()) {
        captureInit();
        while (true)
            captureContinuous();
    } else {
        initNetworking();
        targetingInit();
        while (true)
            targetingContinuous();
    }

    return EXIT_SUCCESS;
}

void Application::initVideoDevice()
{	
    if (config.getIsDevice()) {
        videoDevice = new VideoDevice(constList);
        videoDevice->startCapture(config.getDeviceID());
    }
}

void Application::initNetworking()
{
    if (config.getIsNetworking()) {
        networkController = new NetworkController(constList);
        networkController->startServer();
    }
}

void Application::initGUI()
{
    if (! config.getIsHeadless()) {
        guiManager = new GUIManager(constList);
        guiManager->init();
    }
}

void Application::targetingInit()
{
    /*if (! config.getIsHeadless()) {
      cv::Mat image = loadImage();
      std::cout << "Image resolution: " << image.cols << "x" << image.rows << std::endl;
      }*/
}

void Application::targetingContinuous()
{
    if (config.getIsNetworking())
        networkController->waitForPing();

    cv::Mat image = loadImage();
    if (! config.getIsHeadless())
        guiManager->setImage(image);

    RectangleDetector rectDetector(constList);

    std::vector<Rectangle> foundRectangles = rectDetector.processImage(image);
    MultiRectangleProcessor multiRectProcessor(constList);
    std::vector<std::vector<RectangleProcessor>> rectProcessors;

    
   if (rectDetector.rectangleWasFound()) {

        multiRectProcessor.processRectangles(foundRectangles);
        rectProcessors = multiRectProcessor.getRectProcessors();

        /*for (int i = 0; i < (int)rectProcessors.size(); i++) {

            std::cout << "Rectangle " << i << " processed data:" << std::endl;
            for (int j = 0; j < (int)rectProcessors.at(i).size(); j++) {
                rectProcessors.at(i).at(j).to_string();
        }*/
    }
    

        if (config.getIsNetworking())
            networkController->sendMessage(boost::lexical_cast<std::string>(multiRectProcessor.getFinalProcessor()->getTarget()) + std::string(";") + boost::lexical_cast<std::string>(multiRectProcessor.getFinalProcessor()->getProportionalDistance()) + std::string(";") + boost::lexical_cast<std::string>(multiRectProcessor.getFinalProcessor()->getHorizontalDistance()) + std::string(";") + boost::lexical_cast<std::string>(multiRectProcessor.getFinalProcessor()->getAzimuth()));
     else {
        std::cout << "No rectangle" << std::endl;

        if (config.getIsNetworking())
            networkController->sendMessage("No rectangle");
    }

     
    if (! config.getIsHeadless()) {
        std::string message;
        if (rectDetector.rectangleWasFound())
            message = boost::lexical_cast<std::string>(rectProcessors.at(0).at(0).getProportionalDistance()) + " mm @ " + boost::lexical_cast<std::string>(rectProcessors.at(0).at(0).getAzimuth()) + " degrees";
        else
            message = "No rectangle";
        guiManager->setImageText(message);
        guiManager->show(rectDetector.getAllRectangles(), rectDetector.getFinalRectangles());
        int keycode = cv::waitKey(10);
        if (keycode == 27)
            exit(EXIT_SUCCESS);
        if (config.getIsFile()) {
            cv::waitKey();
            exit(EXIT_SUCCESS);
        }
    } else {
        char waitForKey;
        std::cin >> waitForKey;
        exit(EXIT_SUCCESS);
    }
    
}

void Application::captureInit()
{

}


void Application::captureContinuous()
{
    cv::Mat image = loadImage();
    guiManager->setImage(image);
    guiManager->show();

    char keycode = cv::waitKey(10);
    if (keycode == ' ') {
        std::stringstream filename;
        filename << time(NULL) << ".jpg";
        std::cout << "Writing image to " << filename.str() << std::endl;
        cv::imwrite(filename.str().c_str(), image);
    } else if (keycode == 27) {
        exit(EXIT_SUCCESS);
    }
}

cv::Mat Application::loadImage()
{
    cv::Mat returnVal;

    if (config.getIsFile())
        cv::imread(config.getFileName().c_str()).copyTo(returnVal);
    else if (config.getIsDevice())
        returnVal = videoDevice->getImage();
    else
        exit(1);

    constList->imgCols = returnVal.cols;
    constList->imgRows = returnVal.rows;

    return returnVal;
}

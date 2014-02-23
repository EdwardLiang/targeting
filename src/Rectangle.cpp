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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "constants.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <sstream>

#include "Rectangle.hpp"

Rectangle::Rectangle(PrototypeRectangle a)
{
    //Transferring data from PrototypeRectangle to new Rectangle.
    rectPoints = a.rectPoints;
    newRect.markedForRemoval = a.markedForRemoval; // Marked for removal
    area = a.area; // Area
    topLeft = a.topLeft; // Top Left Corner
    topRight = a.topRight; // Top Right Corner
    bottomRight = a.bottomRight; // Bottom Right Corner
    bottomLeft = a.bottomRight; // Bottom Left Corner
    center = a.center; // Center Point
    lengthSquaredLeft = a.lengthSquaredLeft; // Square of Length of Left Side
    lengthSquaredRight = a.lengthSquaredRight; // Square of Length of Right Side
    lengthSquaredTop = a.lengthSquaredTop; // Square of Length of Top Side
    lengthSquaredBottom = a.lengthSquaredBottom; // Square of Length of Bottom Side
    rectName = a.lengthSquaredBottom;

    computeElevation();
    computeProportionalDistance();
    computeConstantsDistance();
    computeHeight();
    computeHorizontalDistance();
    computeAzimuth();
    //fixHeight();
    computeTilt();
    computeAspectRatio();
    computeLogAspectRatios();

}
bool Rectangle::isType(PrototypeRectangle a){}

Rectangle Rectangle::instantiate(PrototypeRectangle a){
    //Rectangle newRect = new Rectangle(a);
}

float Rectangle::getAzimuth()
{
    return azimuth;
}

int Rectangle::getProportionalDistance()
{
    return proportionalDistance;
}

int Rectangle::getConstantsDistance()
{
    return constantsDistance;
}

float Rectangle::getLogAspectRatios()
{
    return logAspectRatios;
}

int Rectangle::getHorizontalDistance()
{
    return horizontalDistance;
}

int Rectangle::getHeight()
{
    return height;
}

float Rectangle::getTilt()
{
    return tilt;
}

float Rectangle::getElevation()
{
    return elevation;
}

float Rectangle::getAspectRatio()
{
    return aspectRatio;
}

// Private Functions
void Rectangle::computeProportionalDistance()
{
    // Compute distance from each side of rectangle
    int distanceTop = (RectBase * cameraFocalLength) / sqrt(inputRect.lengthSquaredTop);
    int distanceBottom = (RectBase * cameraFocalLength) / sqrt(inputRect.lengthSquaredBottom);
    int distanceLeft = (RectHeight * cameraFocalLength * cos(elevation)) / sqrt(inputRect.lengthSquaredLeft);
    int distanceRight = (RectHeight * cameraFocalLength * cos(elevation)) / sqrt(inputRect.lengthSquaredRight);
    int distanceBase = (distanceTop + distanceBottom) / 2;
    int distanceHeight = (distanceLeft + distanceRight) / 2;

    // Account for camera angle
    //int oldDistanceHeight = -1e6;
    //std::cout << "distanceHeight = " << distanceHeight << std::endl;
    //while ((float)abs(distanceHeight - oldDistanceHeight) / (float)distanceHeight > 0.01) {
    //oldDistanceHeight = distanceHeight;
    //	distanceHeight *= sqrt(1 - pow((constList->targetHeight.at(target) - constList->cameraHeight) / distanceHeight, 2));
    //	std::cout << "oldDistanceHeight = " << oldDistanceHeight << std::endl;
    //	std::cout << "distanceHeight = " << distanceHeight << std::endl;
    //}

    // Take smaller distance
    if (distanceBase > distanceHeight) {
        proportionalDistance = distanceHeight;
        //std::cout << "distanceHeight (" << distanceHeight << ") picked over distanceBase (" << distanceBase << ")" << std::endl;
    } else {
        proportionalDistance = distanceBase;
        //std::cout << "distanceBase (" << distanceBase << ") picked over distanceHeight (" << distanceHeight << ")" << std::endl;
    }
}

void Rectangle::computeConstantsDistance()
{
    /*std::cout << "targetHeight = " << constList->targetHeight.at(target) << std::endl;
      std::cout << "cameraHeight = " << constList->cameraHeight << std::endl;
      std::cout << "sin(elevation) = " << sin(elevation) << std::endl;*/
    constantsDistance = fabs(((float)targetHeight - (float)cameraHeight) / sin(elevation));

}

void Rectangle::computeAzimuth()
{
    azimuth = ((float)inputRect.center.x - ((float)constList->imgCols / 2.0)) / (float)constList->cameraFocalLength;
}

void Rectangle::computeElevation()
{
    float tanElevation = (((float)constList->imgRows / 2.0) - (float)inputRect.center.y) / (float)constList->cameraFocalLength;
    elevation = atan(tanElevation) + constList->cameraAngleRadians;
}

void Rectangle::computeHeight()
{
    height = (float)proportionalDistance * sin(elevation);
    /*int heightError = 1000000;
      int heightIndex = 0;
      for (unsigned i = 0; i < constList->rectPossibleHeights.size(); ++i) {
      if (abs(constList->rectPossibleHeights.at(i) - height) < heightError) {
      heightIndex = i;
      heightError = abs(constList->rectPossibleHeights.at(i) - height);
      }
      }

      height = constList->rectPossibleHeights.at(heightIndex);*/
}

void Rectangle::computeHorizontalDistance()
{
    //horizontalDistance = sqrt(pow(proportionalDistance, 2) - pow(height, 2));
    horizontalDistance = sqrt(pow(proportionalDistance, 2) - pow(RectHeight - constList->cameraHeight, 2));
}

void Rectangle::fixHeight()
{
    if (height == constList->targetPossibleHeights.at(0))
        height = constList->targetPossibleHeights.at(2);
}

void Rectangle::computeTilt()
{
    float cosTilt = ((sqrt(inputRect.lengthSquaredTop) + sqrt(inputRect.lengthSquaredBottom)) / (sqrt(inputRect.lengthSquaredLeft) + sqrt(inputRect.lengthSquaredRight))) * (RectHeight / RectBase);
    if (cosTilt > 0.98)
        tilt = sqrt(1 - pow(cosTilt, 2));
    else
        tilt = acos(cosTilt);

    if (inputRect.lengthSquaredLeft < inputRect.lengthSquaredRight)
        tilt *= -1;
}

void Rectangle::computeAspectRatio()
{
    /*std::cout << "sqrt(inputRect.lengthSquaredTop) = " << sqrt(inputRect.lengthSquaredTop) << std::endl;
      std::cout << "sqrt(inputRect.lengthSquaredBottom) = " << sqrt(inputRect.lengthSquaredBottom) << std::endl;
      std::cout << "sqrt(inputRect.lengthSquaredLeft) = " << sqrt(inputRect.lengthSquaredLeft) << std::endl;
      std::cout << "sqrt(inputRect.lengthSquaredRight) = " << sqrt(inputRect.lengthSquaredRight) << std::endl;*/
    aspectRatio = (sqrt(inputRect.lengthSquaredTop) + sqrt(inputRect.lengthSquaredBottom)) / (sqrt(inputRect.lengthSquaredLeft) + sqrt(inputRect.lengthSquaredRight));
}

void Rectangle::toString(){

    std::cout 
        << "\t Rectangle Name: " << rectName << std::endl 
        << "\t Top left: " << topLeft << std::endl
        << "\t Top right: " << topRight << std::endl
        << "\t Bottom right: " << bottomRight << std::endl
        << "\t Bottom left:  " << bottomLeft << std::endl
        << "\t Area: " << area << std::endl;


    std::cout << "Proportional distance: " << proportionalDistance << " mm" << std::endl;
    std::cout << "Constants distance: " << constantsDistance << " mm" << std::endl;
    std::cout << "Log of aspect ratios: " << logAspectRatios << std::endl;
    std::cout << "Horizontal distance: " << horizontalDistance << " mm" << std::endl;
    std::cout << "Height: " << height << " mm" << std::endl;
    std::cout << "Azimuth: " << getAzimuth() * 180.0 / constList->mathPi << " degrees" << std::endl;
    std::cout << "Tilt: " << getTilt() * 180.0 / constList->mathPi << " degrees" << std::endl;
    std::cout << "Elevation: " << getElevation() * 180.0 / constList->mathPi << " degrees" << std::endl;
    std::cout << "Aspect Ratio: " << aspectRatio << std::endl << std::endl;


}


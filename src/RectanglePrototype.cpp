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

#include "RectanglePrototype.hpp"

RectanglePrototype::RectanglePrototype()
{
    // Defaults
    markedForRemoval = false;
}

RectanglePrototype::RectanglePrototype(int *nullInput)
{
    // Defaults
    markedForRemoval = false;
}

RectanglePrototype::RectanglePrototype(std::vector<cv::Point> input)
{
    populate(input);
}
void RectanglePrototype::setRectType(int type){
    rectangleType = type;
}

void RectanglePrototype::populate(std::vector<cv::Point> input)
{
    // Save inputs
    rectPoints = input;

    // Defaults
    markedForRemoval = false;

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
    for (int i=0; i < (int)input.size(); ++i) {
        if (i == indexTopLeft || i == indexBottomRight) continue;

        if (indexTopRight < 0)
            indexTopRight=i;
        else if (indexBottomLeft < 0)
            indexBottomLeft=i;
    }

    if (input.at(indexTopRight).x < input.at(indexBottomLeft).x) {
        cv::Point swap = input.at(indexTopRight);
        input.at(indexTopRight)=input.at(indexBottomLeft);
        input.at(indexBottomLeft) = swap;
    }

    topLeft = input.at(indexTopLeft);
    topRight = input.at(indexTopRight);
    bottomRight = input.at(indexBottomRight);
    bottomLeft = input.at(indexBottomLeft);


    // Compute Lengths
    lengthSquaredLeft = pow((topLeft.x - bottomLeft.x), 2) + pow((topLeft.y - bottomLeft.y), 2);
    lengthSquaredRight = pow((topRight.x - bottomRight.x), 2) + pow((topRight.y - bottomRight.y), 2);
    lengthSquaredTop = pow((topLeft.x - topRight.x), 2) + pow((topLeft.y - topRight.y), 2);
    lengthSquaredBottom = pow((bottomLeft.x - bottomRight.x), 2) + pow((bottomLeft.y - bottomRight.y), 2);

    // Compute Absolute Lengths/Widths
    absoluteHeight = (std::abs(topLeft.x - bottomLeft.x) + std::abs(topRight.x - bottomRight.x))/2;
    absoluteWidth = (std::abs(topLeft.x - topRight.x) + std::abs(bottomLeft.x - bottomRight.x))/2;


    // Compute Center
    center.x = (topLeft.x + topRight.x + bottomRight.x + bottomLeft.x) / 4;
    center.y = (topLeft.y + topRight.y + bottomRight.y + bottomLeft.y) / 4;

    // Compute Area
    area = fabs(cv::contourArea(cv::Mat(input)));
}

bool RectanglePrototype::containsPoint(cv::Point input)
{
    return (input.x > topLeft.x && input.x < bottomRight.x &&
            input.y > topLeft.y && input.y < bottomRight.y);
}

void RectanglePrototype::toString()
{
    std::cout 
        << "\t Rectangle Name: " << rectName << std::endl 
        << "\t Top left: " << topLeft << std::endl
        << "\t Top right: " << topRight << std::endl
        << "\t Bottom right: " << bottomRight << std::endl
        << "\t Bottom left:  " << bottomLeft << std::endl
        << "\t Area: " << area << std::endl;
}


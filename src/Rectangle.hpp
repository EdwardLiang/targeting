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

#ifndef Rectangle_hpp
#define Rectangle_hpp

class Rectangle
{
    public:
        Rectangle(PrototypeRectangle);
        virtual bool isType(PrototypeRectangle) = 0;
        virtual Rectangle instantiate(PrototypeRectangle) = 0;
        void toString();

        // Data
        std::vector<cv::Point> rectPoints;
        bool markedForRemoval; // Marked for removal
        float area; // Area
        cv::Point topLeft; // Top Left Corner
        cv::Point topRight; // Top Right Corner
        cv::Point bottomRight; // Bottom Right Corner
        cv::Point bottomLeft; // Bottom Left Corner
        cv::Point center; // Center Point
        int lengthSquaredLeft; // Square of Length of Left Side
        int lengthSquaredRight; // Square of Length of Right Side
        int lengthSquaredTop; // Square of Length of Top Side
        int lengthSquaredBottom; // Square of Length of Bottom Side
        std::string rectName;
};

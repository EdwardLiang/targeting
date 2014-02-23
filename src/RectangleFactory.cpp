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

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <sstream>

#include "RectangleFactory.hpp"

/*DESCRIPTION: Singleton Factory that takes RectanglePrototype objects and create full Rectangle Objects from them. */

RectangleFactory::RectangleFactory()
{
    // Add Rectangle subclasses Factory will check for.
    // example: RectangleTypes.push_back(new GenericRectangle);
}

RectangleFactory RectangleFactory::getInstance(){
    if(instance == NULL){
        instance = new RectangleFactory();
    }
    return instance;
}

std::vector<Rectangle> RectangleFactory::createRectangles(std::vector<PrototypeRectangle*> protos){
    std::vector<Rectangle> rects;
    for (int i = 0; i < protos.size(); ++i) {
        rects.push_back(createRectangle(proto.at(i)));
    }
}

Rectangle RectangleFactory::createRectangle(PrototypeRectangle proto){
    for (int a = 0; a < rectangleTypes.size(); ++a) {
        if(RectangleTypes.at(a).isType(proto))
            return RectangleTypes.at(a).instantiate(proto);    
    }
}

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

#define        mathPi 3.14159265358979
#define        cameraFocalLength 640
#define        cameraViewingAngleRadians 1.1414453308043
#define        cameraHDViewingAngleRadians 1.1414453308043
#define        cameraHeight (33.5 * 25.4)
#define        cameraAngleRadians (9.0 * mathPi / 180.0)
// GUI
#define        guiWindowName "FRC Team 3341 Targeting"
// Preprocessing
#define        preprocessingHueLowerThreshold 0
#define        preprocessingHueUpperThreshold 80
#define        preprocessingValueLowerThreshold 200
#define        preprocessingValueUpperThreshold 255
// Detection
#define        detectionMaxCosine 0.3 
#define        detectionCannyThreshold 50
#define        detectionContainedAreaLowerThreshold 0.0
#define        detectionContainedAreaUpperThreshold 0.9
// Server
#define        portNumber 3341

/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include <tulip/GlHexagon.h>

using namespace std;

namespace tlp {

GlHexagon::GlHexagon(const Coord &position,
                     const Size &size,
                     const Color &fillColor,
                     const Color &outlineColor,
                     bool filled,
                     bool outlined,
                     const string &textureName,
                     float outlineSize):
  GlRegularPolygon(position,size,6,fillColor,outlineColor,filled,outlined,textureName,outlineSize) {
}
//=====================================================
GlHexagon::~GlHexagon() {
}
}

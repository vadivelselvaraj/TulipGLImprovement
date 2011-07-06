/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
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
#ifndef GLHEXAGON_H
#define GLHEXAGON_H

#include <tulip/GlRegularPolygon.h>

namespace tlp {
/**
 * \addtogroup GlEntities
 */
/*@{*/
/**
 * class to create an hexagon
 */
class TLP_GL_SCOPE GlHexagon : public GlRegularPolygon {
public:
  /**
   * Constructor
   */
  GlHexagon(const Coord &position,
            const Size &size,
            const Color &outlineColor = Color(255, 0 , 0 , 255),
            const Color &fillColor = Color (0, 0, 255,255),
            bool filled = true,
            bool outlined =true,
            const std::string &textureName="",
            float outlineSize=1.);
  /**
   * Default empty destructor
   */
  virtual ~GlHexagon();

};
/*@}*/
}
#endif

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
#include <cmath>

#include <string>

#include <tulip/StringProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/Size.h>
#include <tulip/Coord.h>
#include <tulip/Glyph.h>
#include <tulip/GlTriangle.h>

#include <tulip/Graph.h>
#include <tulip/GlTools.h>

using namespace std;
using namespace tlp;

class Triangle : public Glyph {
public:
  Triangle(GlyphContext *gc=NULL);
  virtual ~Triangle();
  virtual void getIncludeBoundingBox(BoundingBox &boundingBox,node);
  virtual void draw(node n,float lod);

protected:

  static GlTriangle *triangle;

};

GlTriangle* Triangle::triangle=0;

//=====================================================
GLYPHPLUGIN(Triangle, "2D - Triangle", "David Auber", "09/07/2002", "Textured Triangle", "1.0", 11)
//===================================================================================
Triangle::Triangle(GlyphContext *gc): Glyph(gc) {
  if(!triangle)
    triangle = new GlTriangle(Coord(0,0,0),Size(0.5,0.5,0));
}
//=====================================================
Triangle::~Triangle() {
}
//=====================================================
void Triangle::getIncludeBoundingBox(BoundingBox &boundingBox,node) {
  boundingBox[0] = Coord(-0.25,-0.5,0);
  boundingBox[1] = Coord(0.25,0,0);
}
//=====================================================
void Triangle::draw(node n,float lod) {

  triangle->setFillColor(glGraphInputData->getElementColor()->getNodeValue(n));

  string texFile = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (texFile != "") {
    string texturePath=glGraphInputData->parameters->getTexturePath();
    triangle->setTextureName(texturePath+texFile);
  }
  else {
    triangle->setTextureName("");
  }

  triangle->setOutlineColor(glGraphInputData->getGraph()->getProperty<ColorProperty>("viewBorderColor")->getNodeValue(n));

  if (glGraphInputData->getGraph()->existProperty ("viewBorderWidth")) {
    double lineWidth=glGraphInputData->getGraph()->getProperty<DoubleProperty>("viewBorderWidth")->getNodeValue(n);

    if (lineWidth < 1e-6)
      lineWidth=1e-6;

    triangle->setOutlineSize(lineWidth);
  }
  else {
    triangle->setOutlineSize(2.);
  }

  triangle->draw(lod,NULL);
}
//=====================================================

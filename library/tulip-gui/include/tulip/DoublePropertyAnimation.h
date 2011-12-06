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

#ifndef DOUBLEPROPERTYANIMATION_H
#define DOUBLEPROPERTYANIMATION_H

#include "tulip/CachedPropertyAnimation.h"
#include <tulip/DoubleProperty.h>

namespace tlp {

class TLP_QT_SCOPE DoublePropertyAnimation:public CachedPropertyAnimation<tlp::DoubleProperty, double, double> {
public:
  DoublePropertyAnimation(tlp::Graph *graph, tlp::DoubleProperty *start, tlp::DoubleProperty *end, tlp::DoubleProperty *out,
                          tlp::BooleanProperty *selection = 0, int frameCount = 1, bool computeNodes = true, bool computeEdges = false);
  ~DoublePropertyAnimation() {}

protected:
  double getNodeFrameValue(const double &startValue, const double &endValue, int frame);
  double getEdgeFrameValue(const double &startValue, const double &endValue, int frame);
};

}

#endif // DOUBLEPROPERTYANIMATION_H

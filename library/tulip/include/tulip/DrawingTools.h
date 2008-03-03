//-*-c++-*-
#ifndef _DRAWINGTOOLS_H
#define _DRAWINGTOOLS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tulip/Coord.h>

namespace tlp {

class Graph;
class LayoutProperty;
class SizeProperty;
class DoubleProperty;
class BooleanProperty;

TLP_SCOPE   std::pair<Coord, Coord> computeBoundingBox(Graph *graph, LayoutProperty *layout, SizeProperty *size, DoubleProperty *rotation, BooleanProperty *selection = 0);
  
TLP_SCOPE   std::pair<Coord, Coord> computeBoundingRadius (Graph *graph, LayoutProperty *layout, SizeProperty *size, DoubleProperty *rotation, BooleanProperty *selection = 0);

}


#endif

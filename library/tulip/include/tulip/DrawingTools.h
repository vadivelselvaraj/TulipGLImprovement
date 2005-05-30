//-*-c++-*-
#ifndef _DRAWINGTOOLS_H
#define _DRAWINGTOOLS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tulip/Coord.h>

class SuperGraph;
class LayoutProxy;
class SizesProxy;
class MetricProxy;

namespace tlp {
    TLP_SCOPE   std::pair<Coord, Coord> computeBoundingBox(SuperGraph *graph, LayoutProxy *layout, SizesProxy *size, MetricProxy *rotation);
};


#endif

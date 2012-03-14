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
#include "tulip2ogdf/OGDFLayoutPluginBase.h"

#include <vector>
#include <tulip/DrawingTools.h>
#include <tulip/ForEach.h>

using namespace std;

OGDFLayoutPluginBase::OGDFLayoutPluginBase(const tlp::PluginContext* context, ogdf::LayoutModule *ogdfLayoutAlgo) :
  tlp::LayoutAlgorithm(context), tlpToOGDF(NULL), ogdfLayoutAlgo(ogdfLayoutAlgo) {
  // convert Tulip Graph to OGDF Graph including attributes
  if (graph)
    tlpToOGDF = new TulipToOGDF(graph);
}

OGDFLayoutPluginBase::~OGDFLayoutPluginBase() {
  if (tlpToOGDF != NULL) {
    delete tlpToOGDF;
  }

  delete ogdfLayoutAlgo;
}

bool OGDFLayoutPluginBase::run() {

  ogdf::GraphAttributes &gAttributes = tlpToOGDF->getOGDFGraphAttr();

  beforeCall();

  // run the algorithm on the OGDF Graph with attributes
  callOGDFLayoutAlgorithm(gAttributes);

  // retrieve nodes coordinates computed by the OGDF Layout Algorithm
  // and store it in the Tulip Layout Property
  tlp::node nodeTlp;
  forEach(nodeTlp, graph->getNodes()) {
    tlp::Coord nodeCoord = tlpToOGDF->getNodeCoordFromOGDFGraphAttr(nodeTlp.id);
    result->setNodeValue(nodeTlp, nodeCoord);
  }

  // same operation as above but with edges
  tlp::edge tlpEdge;
  forEach(tlpEdge, graph->getEdges()) {
    vector<tlp::Coord> edgeCoord =
      tlpToOGDF->getEdgeCoordFromOGDFGraphAttr(tlpEdge.id);
    result->setEdgeValue(tlpEdge, edgeCoord);
  }

  afterCall();

  return true;
}

void OGDFLayoutPluginBase::callOGDFLayoutAlgorithm(ogdf::GraphAttributes &gAttributes) {
  ogdfLayoutAlgo->call(gAttributes);
}

void OGDFLayoutPluginBase::transposeLayoutVertically() {
  tlp::BoundingBox graphBB = tlp::computeBoundingBox(graph, result, graph->getProperty<SizeProperty>("viewSize"), graph->getProperty<DoubleProperty>("viewRotation"));
  float midY = (graphBB[0][1] + graphBB[1][1]) / 2.f;
  tlp::node n;
  forEach(n, graph->getNodes()) {
    tlp::Coord nodeCoord = result->getNodeValue(n);
    nodeCoord[1] = midY - (nodeCoord[1] - midY);
    result->setNodeValue(n, nodeCoord);
  }
  tlp::edge e;
  forEach(e, graph->getEdges()) {
    std::vector<tlp::Coord> bends = result->getEdgeValue(e);

    for (size_t i = 0 ; i < bends.size() ; ++i) {
      bends[i][1] = midY - (bends[i][1] - midY);
    }

    result->setEdgeValue(e, bends);
  }
}

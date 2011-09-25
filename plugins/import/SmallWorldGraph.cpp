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
#include <time.h>
#include <math.h>
#include <climits>
#include <tulip/TulipPlugin.h>

using namespace std;
using namespace tlp;
static const int WIDTH = 1024;
static const int HEIGHT = 1024;

namespace {
const char * paramHelp[] = {
  // nodes
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "unsigned int" ) \
  HTML_HELP_DEF( "default", "100" ) \
  HTML_HELP_BODY() \
  "Number of nodes in the final graph." \
  HTML_HELP_CLOSE(),

  // degree
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "unsigned int" ) \
  HTML_HELP_DEF( "default", "10" ) \
  HTML_HELP_BODY() \
  "Average degree of the nodes in the final graph." \
  HTML_HELP_CLOSE(),

  // degree
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "bool" ) \
  HTML_HELP_DEF( "default", "false" ) \
  HTML_HELP_BODY() \
  "If true, long distance edges will be added in the grid approximation." \
  HTML_HELP_CLOSE(),
};
}

/*
 * TODO :
 * Use a quadtree in order to change the complexity from n*n to n*log(n)
 */

/** \addtogroup import */
/*@{*/
/// Grid Approximation - Import of a grid approximation
/** This plugin enables to create a gride approximation
 *
 *  User can specify the number of nodes and their average degree.
 */
class SmallWorldGraph:public ImportModule {
public:
  SmallWorldGraph(AlgorithmContext context):ImportModule(context) {
    addParameter<unsigned int>("nodes",paramHelp[0],"200");
    addParameter<unsigned int>("degree",paramHelp[1],"10");
    addParameter<bool>("long edge",paramHelp[2],"false");
  }
  ~SmallWorldGraph() {}

  bool import() {
    unsigned int nbNodes  = 200;
    unsigned int avgDegree = 10;
    bool enableLongEdge = false;

    if (dataSet!=0) {
      dataSet->get("nodes", nbNodes);
      dataSet->get("degree", avgDegree);
      dataSet->get("long edge", enableLongEdge);
    }

    if (nbNodes == 0) {
      if (pluginProgress)
        pluginProgress->setError(string("Error: the number of nodes cannot be null"));

      return false;
    }

    if (avgDegree == 0) {
      if (pluginProgress)
        pluginProgress->setError(string("Error: the average degree cannot be null"));

      return false;
    }

    double maxDistance = sqrt(double(avgDegree)*double(WIDTH)*double(HEIGHT)
                              / (double (nbNodes) * M_PI));
    srand(clock());
    LayoutProperty *newLayout=graph->getLocalProperty<LayoutProperty>("viewLayout");
    vector<node> sg(nbNodes);

    pluginProgress->showPreview(false);

    for (unsigned int i=0; i<nbNodes; ++i) {
      sg[i]=graph->addNode();
      newLayout->setNodeValue(sg[i],Coord(rand()%WIDTH, rand()%HEIGHT, 0));
    }

    unsigned int count = 0;
    unsigned int iterations = nbNodes*(nbNodes-1)/2;
    double minSize = DBL_MAX;

    for (unsigned int i=0; i<nbNodes-1; ++i) {
      bool longEdge =false;

      if (pluginProgress->progress(count,iterations)!=TLP_CONTINUE) break;

      for (unsigned int j=i+1; j<nbNodes; ++j) {
        ++count;

        if (i!=j) {
          double distance = newLayout->getNodeValue(sg[i]).dist(newLayout->getNodeValue(sg[j]));
          minSize = std::min(distance, minSize);

          //newSize->setAllNodeValue(Size(minSize/2.0, minSize/2.0, 1));
          if ( distance  < (double)maxDistance)
            graph->addEdge(sg[i],sg[j]);
          else if (!longEdge && enableLongEdge) {
            double distrand = (double)rand()/(double)RAND_MAX;

            if (distrand < 1.0/(2.0+double(nbNodes-i-1))) {
              longEdge = true;
              graph->addEdge(sg[i],sg[j]);
            }
          }
        }
      }
    }

    return  pluginProgress->state()!=TLP_CANCEL;
  }
};
/*@}*/
IMPORTPLUGINOFGROUP(SmallWorldGraph,"Grid Approximation","Auber","25/06/2002","","1.0","Graphs")

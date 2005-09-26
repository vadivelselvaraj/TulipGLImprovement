//-*-c++-*-
#ifndef Tulip_InducedSubGraphSelection_H
#define Tulip_InducedSubGraphSelection_H

#include <tulip/TulipPlugin.h>

/** \addtogroup selection */
/*@{*/
/// This selection plugins enables to find a subgraph induced by a set of nodes.
/**
 * This selection plugins enables to find a subgraph graph induced by a set of nodes.
 * Let V' a subset of nodes, G' is an induced subgraph of G iff all edges 
 * (and only these ones) element of G that link
 * two nodes (only one in case of loop) of V' are in G'.
 *
 *  \author David Auber, LaBRI University Bordeaux I France: 
 *   auber@tulip-software.org
 */
class InducedSubGraphSelection:public Selection { 
public:
  InducedSubGraphSelection(const PropertyContext &);
  bool run();
};
/*@}*/
#endif





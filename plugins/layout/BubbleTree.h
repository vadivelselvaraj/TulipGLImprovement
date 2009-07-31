//-*-c++-*-
#ifndef _BUBBLETREELAYOUT_H
#define _BUBBLETREELAYOUT_H

#include <tulip/tuliphash.h>
#include <tulip/TulipPlugin.h>

/** \addtogroup layout */
/*@{*/

/** This plug-in implement the bubble tree drawing algorithm
 *  first published as:
 *
 *  Bubble Tree Drawing Algorithm : \n
 *  D. Auber and S. Grivet and J-P Domenger and Guy Melancon, \n
 *  In International Conference on Computer Vision and Graphics, pages 633-641, september 2004.
 *
 *  <b>HISTORY</b> 
 *
 *  - 07/12/04 Version 0.0.3: Use new Tulip plugin parameter mechanism
 *  - 16/07/03 Version 0.0.2: Better management of node size
 *  - 16/05/03 Version 0.0.1: Initial release
 *
 *  \note  This algorithm only works on trees.
 *  Let n be the number of nodes, the algorithm complexity is in O(n) or O(nlog(n)), 
 *  By default O(nlog(n)) algorithm is used, but one can choose the complexity by using 
 *  the argument (bool)"complexity" (true means  O(nlog(n), false  O(n)).
 *  The algorithm can manage nodes of different size. The SizeProperty "viewSize"
 *  is used by default if no parameters are given to the plug-in. 
 *  The parameter is (SizeProperty*) "node size".
 *
 *  
 *
 *  \author David Auber, S. Grivet  University Bordeaux I France: 
 *   auber@tulip-software.org, grivet@labri.fr
 */
class BubbleTree:public tlp::LayoutAlgorithm { 
public:
  BubbleTree(const tlp::PropertyContext &);
  ~BubbleTree();
  bool run();
private:
  double computeRelativePosition(tlp::node n, TLP_HASH_MAP<tlp::node, tlp::Vector<double, 5 > > *relativePosition);
  void calcLayout(tlp::node n, TLP_HASH_MAP<tlp::node, tlp::Vector<double, 5 > > *relativePosition);
  void calcLayout2(tlp::node n, TLP_HASH_MAP<tlp::node, tlp::Vector<double, 5 > > *relativePosition,
		   const tlp::Vector<double, 3 > &,const tlp::Vector<double, 3 > &);
  tlp::Graph *tree;
  tlp::SizeProperty *nodeSize;
  bool nAlgo;
};
/*@}*/
#endif











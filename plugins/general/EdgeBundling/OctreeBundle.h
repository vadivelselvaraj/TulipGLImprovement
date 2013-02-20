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

#ifndef OCTREEBUNDLE_H
#define OCTREEBUNDLE_H
#include <tulip/Vector.h>
#include <tulip/DrawingTools.h>
#include <tulip/ForEach.h>

class OctreeBundle {
public:
  static void compute(tlp::Graph *graph, double splitRatio, tlp::LayoutProperty *layout=NULL, tlp::SizeProperty *size=NULL);
  //=====================================
  static bool isIn(const tlp::Coord &p, const tlp::Coord &a, const tlp::Coord &b, const tlp::Coord &c, const tlp::Coord &d);

protected:
  void createOctree(tlp::Graph *graph, tlp::LayoutProperty *layout=NULL, tlp::SizeProperty *size=NULL);
private:
  double minSize;
  double nbNodesInOriginalGraph;
  std::vector<tlp::node> resultNode;
  tlp::LayoutProperty *layout;
  tlp::SizeProperty   *size;
  tlp::DoubleProperty *rot;
  tlp::DoubleProperty *ntype;
  tlp::Graph * graph;
  double splitRatio;
  typedef tlp::Vector<double, 3> Vec3D;
  struct LessPair {
    bool operator()( const tlp::Coord &a, const tlp::Coord &b) {
      if ((a-b).norm() < 1E-6) return false;

      if (a[0] < b[0]) return true;

      if (a[0] > b[0]) return false;

      if (a[1] < b[1]) return true;

      if (a[1] > b[1]) return false;

      if (a[2] < b[2]) return true;

      return false;
    }
  };
  typedef std::map<tlp::Coord, tlp::node, LessPair> MapVecNode;
  MapVecNode mapN;
  //=====================================
  tlp::node splitEdge(tlp::node a, tlp::node b);
  //=====================================
  void elmentSplitting(const tlp::Coord &a,
                       const tlp::Coord &b,
                       const tlp::Coord &c,
                       const tlp::Coord &d,
                       const std::vector<tlp::node> &input,
                       std::vector<tlp::node> &in,
                       std::vector<tlp::node> &out);
  //=====================================
  void recQuad(const tlp::Coord fr[4], const tlp::Coord ba[4], const std::vector<tlp::node> &input);
  tlp::node addNode(const tlp::Coord pos);
  void cleanNode(const tlp::node n);
  std::vector<std::pair<tlp::node, tlp::node> > unvalidEdges;
};

#endif // OCTREEBUNDLE_H

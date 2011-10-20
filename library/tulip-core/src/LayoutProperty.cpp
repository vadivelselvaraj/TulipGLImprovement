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
#include <float.h>
#include <tulip/LayoutProperty.h>
#include <tulip/Coord.h>
#include <tulip/ForEach.h>

using namespace std;
using namespace tlp;

//=================================================================================
namespace tlp {
void maxV(tlp::Coord &res, const tlp::Coord &cmp) {
  for (unsigned int i = 0; i<3; ++i) {
    res[i] = std::max(res[i], cmp[i]);
  }
}
void minV(tlp::Coord &res, const tlp::Coord &cmp) {
  for (unsigned int i = 0; i<3; ++i) {
    res[i] = std::min(res[i], cmp[i]);
  }
}

/**
 * @brief This template specialization provides specific computation for min and max values of Layout properties (they are specific in that they use the control points of the edges)
 **/
template <>
void tlp::MinMaxProperty<tlp::PointType, tlp::LineType, tlp::LayoutAlgorithm>::computeMinMaxNode(Graph *sg) {
#ifndef NDEBUG
  std::cerr << __PRETTY_FUNCTION__ << std::endl;
#endif
  tlp::Coord maxT(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  tlp::Coord minT(FLT_MAX, FLT_MAX, FLT_MAX);

  tlp::Iterator<node> *itN=sg->getNodes();

  if  (itN->hasNext()) {
    node itn=itN->next();
    const Coord& tmpCoord = this->getNodeValue(itn);
    maxV(maxT, tmpCoord);
    minV(minT, tmpCoord);
  }

  while (itN->hasNext()) {
    node itn=itN->next();
    const Coord& tmpCoord = this->getNodeValue(itn);
    maxV(maxT, tmpCoord);
    minV(minT, tmpCoord);
  }

  delete itN;
  tlp::Iterator<edge> *itE = sg->getEdges();

  while (itE->hasNext()) {
    edge ite=itE->next();
    const LineType::RealType& value = this->getEdgeValue(ite);
    LineType::RealType::const_iterator itCoord;

    for (itCoord=value.begin(); itCoord!=value.end(); ++itCoord) {
      const Coord& tmpCoord = *itCoord;
      maxV(maxT, tmpCoord);
      minV(minT, tmpCoord);
    }
  }

  delete itE;
  unsigned int sgi = sg->getId();
  nodeValueUptodate[sgi] = true;
  minNode[sgi] = minT;
  maxNode[sgi] = maxT;
  //  cerr << "LayoutProperty::computeMinMax end" << endl;
}

/**
 * @brief This template specialization provides specific computation for min and max values of Layout properties (they are specific in that they use the control points of the edges)
 **/
template <>
void tlp::MinMaxProperty<tlp::PointType, tlp::LineType, tlp::LayoutAlgorithm>::updateEdgeValue(tlp::edge e, tlp::LineType::RealType newValue) {
  TLP_HASH_MAP<unsigned int, bool>::const_iterator it = nodeValueUptodate.begin();

  if (it != nodeValueUptodate.end()) {
    const std::vector<Coord>& oldV = this->getEdgeValue(e);

    if (newValue != oldV) {
      // loop on subgraph min/max
      for(; it != nodeValueUptodate.end(); ++it) {
        unsigned int gid = (*it).first;
        const Coord& minV = minNode[gid];
        const Coord& maxV = maxNode[gid];
        bool reset = false;

        // check if min has to be updated
        for(unsigned i = 0; i < newValue.size(); ++i) {
          if (minV > newValue[i]) {
            reset = true;
            break;
          }
        }

        if (!reset) {
          // check if max has to be updated
          for(unsigned i = 0; i < newValue.size(); ++i) {
            if (maxV < newValue[i]) {
              reset = true;
              break;
            }
          }
        }

        if (!reset) {
          // check if minV belongs to oldV
          for(unsigned i = 0; i < oldV.size(); ++i) {
            if (minV == oldV[i]) {
              reset = false;
              break;
            }
          }
        }

        if (!reset) {
          // check if maxV belongs to oldV
          for(unsigned i = 0; i < oldV.size(); ++i) {
            if (maxV == oldV[i]) {
              reset = false;
              break;
            }
          }
        }

        // reset bounding box if needed
        if (reset) {
          nodeValueUptodate.clear();
          minNode.clear();
          maxNode.clear();
          break;
        }
      }
    }
  }
}

}

inline double sqr(double x) {
  return (x*x);
}

const string LayoutProperty::propertyTypename="layout";
const string CoordVectorProperty::propertyTypename="vector<coord>";

// define a specific MetaValueCalculator
class LayoutMetaValueCalculator : public AbstractLayoutProperty::MetaValueCalculator {
public:
  void computeMetaValue(AbstractLayoutProperty* layout,
                        node mN, Graph* sg, Graph*) {
    switch(sg->numberOfNodes()) {
    case 0:
      layout->setNodeValue(mN, Coord(0, 0, 0));
      return;

    case 1:
      layout->setNodeValue(mN,((LayoutProperty *)layout)->getMax(sg));
      return;

    default:
      // between the min and max computed values
      layout->setNodeValue(mN,
                           (((LayoutProperty *)layout)->getMax(sg) +
                            ((LayoutProperty *)layout)->getMin(sg)) / 2.0f);
    }
  }
};

static LayoutMetaValueCalculator mvLayoutCalculator;

//======================================================
LayoutProperty::LayoutProperty(Graph *sg, std::string n, bool updateOnEdgeReversal) : LayoutMinMaxProperty(sg, n,
      Coord(FLT_MAX, FLT_MAX, FLT_MAX), Coord(-FLT_MAX, -FLT_MAX, -FLT_MAX), tlp::LineType::RealType(), tlp::LineType::RealType()) {
// if needed the property observes the graph (see reverseEdge)
  if (updateOnEdgeReversal)
    graph->addListener(this);

  // set default MetaValueCalculator
  setMetaValueCalculator(&mvLayoutCalculator);
}
//======================================================
Coord LayoutProperty::getMax(Graph *sg) {
  return LayoutMinMaxProperty::getNodeMax(sg);
}
//======================================================
Coord  LayoutProperty::getMin(Graph *sg) {
  return LayoutMinMaxProperty::getNodeMin(sg);
}
//=================================================================================
#define X_ROT 0
#define Y_ROT 1
#define Z_ROT 2
static void rotateVector(Coord &vec, double alpha, int rot) {
  Coord backupVec(vec);
  double aRot =  2.0*M_PI * alpha / 360.0;
  float cosA = cos(aRot);
  float sinA = sin(aRot);

  switch(rot) {
  case Z_ROT:
    vec[0] = backupVec[0]*cosA - backupVec[1]*sinA;
    vec[1] = backupVec[0]*sinA + backupVec[1]*cosA;
    break;

  case Y_ROT:
    vec[0] = backupVec[0]*cosA + backupVec[2]*sinA;
    vec[2] = backupVec[2]*cosA - backupVec[0]*sinA;
    break;

  case X_ROT:
    vec[1] = backupVec[1]*cosA - backupVec[2]*sinA;
    vec[2] = backupVec[1]*sinA + backupVec[2]*cosA;
    break;
  }
}
//=================================================================================
void LayoutProperty::rotate(const double& alpha, int rot, Iterator<node> *itN, Iterator<edge> *itE) {
  Observable::holdObservers();

  while (itN->hasNext()) {
    node itn = itN->next();
    Coord tmpCoord(getNodeValue(itn));
    rotateVector(tmpCoord, alpha, rot);
    setNodeValue(itn,tmpCoord);
  }

  while (itE->hasNext()) {
    edge ite=itE->next();

    if (!getEdgeValue(ite).empty()) {
      LineType::RealType tmp = getEdgeValue(ite);
      LineType::RealType::iterator itCoord;
      itCoord=tmp.begin();

      while(itCoord!=tmp.end()) {
        rotateVector(*itCoord, alpha, rot);
        ++itCoord;
      }

      setEdgeValue(ite,tmp);
    }
  }

  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::rotateX(const double& alpha,  Iterator<node> *itN, Iterator<edge> *itE) {
  rotate(alpha, X_ROT, itN, itE);
}
//=================================================================================
void LayoutProperty::rotateY(const double& alpha,  Iterator<node> *itN, Iterator<edge> *itE) {
  rotate(alpha, Y_ROT, itN, itE);
}
//=================================================================================
void LayoutProperty::rotateZ(const double& alpha,  Iterator<node> *itN, Iterator<edge> *itE) {
  rotate(alpha, Z_ROT, itN, itE);
}
//=================================================================================
void LayoutProperty::rotateZ(const double& alpha, Graph *sg) {
  if (sg==0) sg = graph;

  if (sg->numberOfNodes()==0) return;

  Iterator<node> *itN = sg->getNodes();
  Iterator<edge> *itE = sg->getEdges();
  rotateZ(alpha, itN, itE);
  delete itN;
  delete itE;
}
//=================================================================================
void LayoutProperty::scale(const tlp::Vector<float,3>& v, Iterator<node> *itN, Iterator<edge> *itE) {
  Observable::holdObservers();

  while (itN->hasNext()) {
    node itn = itN->next();
    Coord tmpCoord(getNodeValue(itn));
    tmpCoord *= *(Coord*)&v;
    setNodeValue(itn,tmpCoord);
  }

  while (itE->hasNext()) {
    edge ite=itE->next();

    if (!getEdgeValue(ite).empty()) {
      LineType::RealType tmp = getEdgeValue(ite);
      LineType::RealType::iterator itCoord;
      itCoord=tmp.begin();

      while(itCoord!=tmp.end()) {
        *itCoord *= *(Coord*)&v;
        ++itCoord;
      }

      setEdgeValue(ite,tmp);
    }
  }

  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::scale(const tlp::Vector<float,3>& v, Graph *sg) {
  if (sg==0) sg = graph;

  if (sg->numberOfNodes()==0) return;

  Iterator<node> *itN = sg->getNodes();
  Iterator<edge> *itE = sg->getEdges();
  scale(v, itN, itE);
  delete itN;
  delete itE;
}
//=================================================================================
void LayoutProperty::translate(const tlp::Vector<float,3>& v, Iterator<node> *itN, Iterator<edge> *itE) {
  Observable::holdObservers();

  if (itN != 0)
    while (itN->hasNext()) {
      node itn = itN->next();
      Coord tmpCoord(getNodeValue(itn));
      tmpCoord += *(Coord*)&v;
      setNodeValue(itn,tmpCoord);
    }

  if (itE != 0)
    while (itE->hasNext()) {
      edge ite=itE->next();

      if (!getEdgeValue(ite).empty()) {
        LineType::RealType tmp = getEdgeValue(ite);
        LineType::RealType::iterator itCoord;
        itCoord=tmp.begin();

        while(itCoord!=tmp.end()) {
          *itCoord += *(Coord*)&v;
          ++itCoord;
        }

        setEdgeValue(ite,tmp);
      }
    }

  if (itE != 0 || itN != 0) {
    resetBoundingBox();
  }

  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::translate(const tlp::Vector<float,3>& v, Graph *sg) {
  if (sg==0) sg = graph;

  if (sg->numberOfNodes()==0) return;

  Iterator<node> *itN = sg->getNodes();
  Iterator<edge> *itE = sg->getEdges();
  translate(v, itN, itE);
  delete itN;
  delete itE;
}
//=================================================================================
void LayoutProperty::center(Graph *sg) {
#ifndef NDEBUG
  cerr << __PRETTY_FUNCTION__  << endl;
#endif

  if (sg==0) sg = graph;

  if (sg->numberOfNodes()==0) return;

  Observable::holdObservers();
  Coord tr=getMax(sg)+getMin(sg);
  tr /= -2.0;
  translate(tr, sg);
  resetBoundingBox();
  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::normalize(Graph *sg) {
#ifndef NDEBUG
  cerr << __PRETTY_FUNCTION__ << endl;
#endif

  if (sg==0) sg = graph;

  if (sg->numberOfNodes()==0) return;

  Observable::holdObservers();
  center();
  double dtmpMax = 1.0;
  Iterator<node> *itN=sg->getNodes();

  while (itN->hasNext())  {
    node itn=itN->next();
    const Coord& tmpCoord = getNodeValue(itn);
    dtmpMax  = std::max(dtmpMax, sqr(tmpCoord[0])+sqr(tmpCoord[1])+sqr(tmpCoord[2]));
  }

  delete itN;
  dtmpMax = 1.0 / sqrt(dtmpMax);
  scale(Coord(dtmpMax,dtmpMax,dtmpMax), sg);
  resetBoundingBox();
  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::perfectAspectRatio() {
#ifndef NDEBUG
  cerr << __PRETTY_FUNCTION__ << endl;
#endif

  if (graph->numberOfNodes()==0) return;

  Observable::holdObservers();
  center();
  double scaleX,scaleY,scaleZ;
  double deltaX,deltaY,deltaZ;
  deltaX = (double )getMax()[0]-(double )getMin()[0];
  deltaY = (double )getMax()[1]-(double )getMin()[1];
  deltaZ = (double )getMax()[2]-(double )getMin()[2];
  double delta = std::max(deltaX , deltaY);
  delta = std::max(delta, deltaZ);

  if (delta<0.001) return;

  if (deltaX<0.001) deltaX=delta;

  if (deltaY<0.001) deltaY=delta;

  if (deltaZ<0.001) deltaZ=delta;

  scaleX = delta / deltaX;
  scaleY = delta / deltaY;
  scaleZ = delta / deltaZ;
  scale(Coord(scaleX,scaleY,scaleZ));
  Observable::unholdObservers();
}

//=================================================================================
void LayoutProperty::clone_handler(LayoutMinMaxProperty& proxyC) {
  if (typeid(this)==typeid(&proxyC)) {
    LayoutProperty *proxy=(LayoutProperty *)&proxyC;
    nodeValueUptodate = proxy->nodeValueUptodate;
    minNode = proxy->minNode;
    maxNode = proxy->maxNode;
  }
}
//=================================================================================
void LayoutProperty::resetBoundingBox() {
  nodeValueUptodate.clear();
  minNode.clear();
  minEdge.clear();
}
//================================================================================
void LayoutProperty::setNodeValue(const node n, const Coord& v) {
  LayoutMinMaxProperty::updateNodeValue(n, v);
  LayoutMinMaxProperty::setNodeValue(n, v);
}
//================================================================================
void LayoutProperty::setEdgeValue(const edge e, const std::vector<Coord>& v) {
  LayoutMinMaxProperty::updateEdgeValue(e, v);
  LayoutMinMaxProperty::setEdgeValue(e, v);
}
//=================================================================================
void LayoutProperty::setAllNodeValue(const Coord &v) {
  resetBoundingBox();
  LayoutMinMaxProperty::setAllNodeValue(v);
}
//=================================================================================
void LayoutProperty::setAllEdgeValue(const std::vector<Coord> &v) {
  resetBoundingBox();
  LayoutMinMaxProperty::setAllEdgeValue(v);
}
//=================================================================================
double LayoutProperty::averageAngularResolution(const Graph *sg) const {
  if (sg==0) sg=graph;

  Iterator<node> *itN=sg->getNodes();
  double result=0;

  while (itN->hasNext()) {
    result+=averageAngularResolution(itN->next(),sg);
  }

  delete itN;
  return result/(double)sg->numberOfNodes();
}
//=================================================================================
#ifndef DOXYGEN_NOTFOR_DEVEL
struct AngularOrder {
  bool operator() (const Coord &c1, const Coord &c2) {
    //if the vectors have not the same direction on y-coordiantes
    //the result is direct.
    if (c1[1]>=0 && c2[1]<0) return false;

    if (c2[1]>=0 && c1[1]<0) return true;

    //If the vectors have the same size on the y-coordinates, we compare
    //their x-coordinates
    if (c2[1]>=0 && c1[1]>=0)
      return c1[0]>c2[0];
    else
      return c1[0]<c2[0];
  }
  bool operator() (const pair<Coord, edge> &c1, const pair<Coord, edge> &c2) {
    return this->operator()(c1.first, c2.first);
  }
};

#endif // DOXYGEN_NOTFOR_DEVEL

/*
 * TODO check code duplication with angularresolution function
 */
void LayoutProperty::computeEmbedding(Graph *sg) {
  if (sg == 0) sg = graph;

  node n;
  forEach(n, sg->getNodes()) {
    computeEmbedding(n, sg);
  }
}

void LayoutProperty::computeEmbedding(const node n, Graph *sg) {
  if (sg == 0) sg = graph;

  if (sg->deg(n) < 2) return;

  //===========
  typedef pair<Coord, edge> pCE;

  list< pCE > adjCoord;
  //Extract all adjacent edges, the bends are taken
  //into account.
  Iterator<edge> *itE=sg->getInOutEdges(n);

  for (unsigned int i=0; itE->hasNext(); ++i) {
    edge ite=itE->next();

    if (getEdgeValue(ite).size()>0) {
      if (sg->source(ite)==n)
        adjCoord.push_back(pCE(getEdgeValue(ite).front(), ite));
      else
        adjCoord.push_back(pCE(getEdgeValue(ite).back(), ite) );
    }
    else {
      adjCoord.push_back(pCE(getNodeValue(sg->opposite(ite,n)), ite) );
    }
  }

  delete itE;

  //Compute normalized vectors associated to incident edges.
  const Coord& center=getNodeValue(n);
  list<pCE>::iterator it;

  for (it=adjCoord.begin(); it!=adjCoord.end(); ++it) {
    it->first  -= center;
    it->first /= it->first.norm();
  }

  //Sort the vector to compute angles between two edges
  //Correctly.
  adjCoord.sort(AngularOrder());
  //Compute the angles
  vector<edge> tmpOrder;

  for (it = adjCoord.begin(); it != adjCoord.end(); ++it) {
    tmpOrder.push_back(it->second);
  }

  sg->setEdgeOrder(n, tmpOrder);
}
//=================================================================================
vector<double> LayoutProperty::angularResolutions(const node n, const Graph *sg) const {
  vector<double> result;

  if (sg == 0) sg = graph;

  double degree = sg->deg(n);

  if (sg->deg(n)==0) return result;

  if (sg->deg(n)==1) {
    result.push_back(0.0);
    return result;
  }

  //===========
  list<Coord> adjCoord;
  //Extract all adjacent edges, the bends are taken
  //into account.
  Iterator<edge> *itE=sg->getInOutEdges(n);

  for (unsigned int i=0; itE->hasNext(); ++i) {
    edge ite=itE->next();

    if (getEdgeValue(ite).size()>0) {
      if (sg->source(ite)==n)
        adjCoord.push_back(getEdgeValue(ite).front());
      else
        adjCoord.push_back(getEdgeValue(ite).back());
    }
    else {
      adjCoord.push_back(getNodeValue(sg->opposite(ite,n)));
    }
  }

  delete itE;

  //Compute normalized vectors associated to incident edges.
  const Coord& center=getNodeValue(n);
  list<Coord>::iterator it;

  for (it=adjCoord.begin(); it!=adjCoord.end(); ++it) {
    (*it)-=center;
    (*it)/=(*it).norm();
  }

  //Sort the vector to compute angles between two edges
  //Correctly.
  adjCoord.sort(AngularOrder());
  //Compute the angles
  it=adjCoord.begin();
  Coord first=(*it);
  Coord current=first;
  ++it;

  int stop=2;

  for (; stop>0;) {
    Coord next=*it;
    double cosTheta = current.dotProduct(next);//current * next;
    double sinTheta = (current^next)[2];

    if (cosTheta+0.0001>1) cosTheta-=0.0001;

    if (cosTheta-0.0001<-1) cosTheta+=0.0001;

    if (sinTheta+0.0001>1) sinTheta-=0.0001;

    if (sinTheta-0.0001<-1) sinTheta+=0.0001;

    if (sinTheta>=0) {
      result.push_back(2.0*M_PI/degree-acos(cosTheta));
    }
    else {
      result.push_back(2.0*M_PI/degree-(2.0*M_PI-acos(cosTheta)));
    }

    current=next;
    ++it;

    if (stop<2) stop=0;

    if (it==adjCoord.end()) {
      it=adjCoord.begin();
      stop--;
    }
  }

  return result;
}
//=================================================================================
double LayoutProperty::averageAngularResolution(const node n, const Graph *sg) const {
  vector<double> tmp(angularResolutions(n, sg));

  if (tmp.empty()) return 0.;

  double sum = 0.;
  vector<double>::const_iterator it = tmp.begin();

  for (; it != tmp.end(); ++it)
    sum += *it;

  return sum / double(tmp.size());
}
//=================================================================================
double LayoutProperty::edgeLength(const edge e) const {
  pair<node, node> eEnds = graph->ends(e);
  Coord start=getNodeValue(eEnds.first);
  const Coord& end=getNodeValue(eEnds.second);
  double result=0;
  const vector<Coord> & tmp=getEdgeValue(e);

  for (unsigned int i=0; i<tmp.size(); ++i) {
    result+=(tmp[i]-start).norm();
    start=tmp[i];
  }

  result+=(end-start).norm();
  return result;
}
//=================================================================================
unsigned int LayoutProperty::crossingNumber() const {
  cerr << "!!! Warning: Not Implemented function :" << endl;
  cerr << __PRETTY_FUNCTION__ << endl;
  return 0;
}
//=================================================================================
PropertyInterface* LayoutProperty::clonePrototype(Graph * g, const std::string& n) {
  if( !g )
    return 0;

  LayoutProperty * p = g->getLocalProperty<LayoutProperty>( n );
  p->setAllNodeValue( getNodeDefaultValue() );
  p->setAllEdgeValue( getEdgeDefaultValue() );
  return p;
}
//=============================================================
void LayoutProperty::treatEvent(const Event& evt) {
  LayoutMinMaxProperty::treatEvent(evt);

  const GraphEvent* graphEvent = dynamic_cast<const tlp::GraphEvent*>(&evt);

  if (graphEvent && graphEvent->getType() == GraphEvent::TLP_REVERSE_EDGE) {
    // cannot use the assert below because this property
    // may observe subgraphs
    // see MinMaxCalculator<>::treatEvent in MinMaxCalculator.cxx
    //assert(graphEvent->getGraph() == graph);
    std::vector<Coord> bends = getEdgeValue(graphEvent->getEdge());

    // reverse bends if needed
    if (bends.size() > 1) {
      unsigned int halfSize = bends.size()/2;

      for (unsigned int i = 0, j = bends.size() - 1; i < halfSize; ++i, j--) {
        Coord tmp = bends[i];
        bends[i] = bends[j];
        bends[j] = tmp;
      }

      setEdgeValue(graphEvent->getEdge(), bends);
    }
  }
}
//=================================================================================
PropertyInterface* CoordVectorProperty::clonePrototype(Graph * g, const std::string& n) {
  if( !g )
    return 0;

  CoordVectorProperty * p = g->getLocalProperty<CoordVectorProperty>( n );
  p->setAllNodeValue( getNodeDefaultValue() );
  p->setAllEdgeValue( getEdgeDefaultValue() );
  return p;
}

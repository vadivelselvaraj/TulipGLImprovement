//-*-c++-*-

#include <float.h>
#include "tulip/SizeProperty.h"
#include "tulip/PluginContext.h"
#include "tulip/Observable.h"
#include "tulip/SizeAlgorithm.h"
#include "tulip/AbstractProperty.h"

using namespace std;
using namespace tlp;

//==============================
SizeProperty::SizeProperty (Graph *sg, std::string n):
  AbstractProperty<SizeType,SizeType, SizeAlgorithm>(sg, n) {
  // the property observes itself; see beforeSet... methods
  addPropertyObserver(this);
}
//====================================================================
void SizeProperty::scale(const tlp::Vector<float,3>& v, Iterator<node> *itN, Iterator<edge> *itE) {
  Observable::holdObservers();
  while (itN->hasNext()) {
    node itn = itN->next();
    Size tmpSize(getNodeValue(itn));
    tmpSize *= v;
    setNodeValue(itn,tmpSize);
  }
  while (itE->hasNext()) {
    edge ite=itE->next();
    Size tmpSize(getEdgeValue(ite));
    tmpSize *= v;
    setEdgeValue(ite,tmpSize);
  }
  resetMinMax();
  Observable::unholdObservers();
}
//=============================================================================
void SizeProperty::scale( const tlp::Vector<float,3>& v, Graph *sg ) {
  if (sg==0) sg = graph;
  if (sg->numberOfNodes()==0) return;
  Iterator<node> *itN = sg->getNodes();
  Iterator<edge> *itE = sg->getEdges();
  scale(v, itN, itE);
  delete itN;
  delete itE;
}
//=============================================================================
Size SizeProperty::getMax(Graph *sg) {
  if (sg==0) sg=graph;
  unsigned sgi=(unsigned long)sg;
  if (minMaxOk.find(sgi)==minMaxOk.end()) minMaxOk[sgi]=false;
  if (!minMaxOk[sgi]) computeMinMax(sg);
  return max[sgi];
}
//=============================================================================
Size  SizeProperty::getMin(Graph *sg) {
  if (sg==0) sg=graph;
  unsigned long sgi=(unsigned long)sg;
  if (minMaxOk.find(sgi)==minMaxOk.end()) minMaxOk[sgi]=false;
  if (!minMaxOk[sgi]) computeMinMax(sg);
  return min[sgi];
}
//=============================================================================
void SizeProperty::computeMinMax(Graph *sg) {
  Size maxS,minS;
  Iterator<node> *itN=sg->getNodes();
  if  (itN->hasNext()) {
    node itn=itN->next();
    const Size& tmpSize=getNodeValue(itn);
    for (int i=0;i<3;++i) {
      maxS[i]=tmpSize[i];
      minS[i]=tmpSize[i];
    }
  }
  while (itN->hasNext()) {
    node itn=itN->next();
    const Size& tmpSize=getNodeValue(itn);
    for (int i=0;i<3;++i) {
      maxS[i] = std::max(maxS[i], tmpSize[i]);
      minS[i] = std::min(minS[i], tmpSize[i]);
    }
  }delete itN;
  unsigned long sgi=(unsigned long)sg;
  minMaxOk[sgi]=true;  
  min[sgi]=minS;
  max[sgi]=maxS;
}
//=============================================================================
void SizeProperty::resetMinMax() {
  minMaxOk.clear();
  min.clear();
  max.clear();
}
//=============================================================================
void SizeProperty::beforeSetNodeValue(PropertyInterface*, const node) {
  resetMinMax();
}
//=============================================================================
void SizeProperty::beforeSetEdgeValue(PropertyInterface*, const edge) {
  resetMinMax();
}
//=============================================================================
void SizeProperty::beforeSetAllNodeValue(PropertyInterface*) {
  resetMinMax();
}
//=============================================================================
void SizeProperty::beforeSetAllEdgeValue(PropertyInterface*) {
  resetMinMax();
}
//=============================================================================
PropertyInterface* SizeProperty::clonePrototype(Graph * g, const std::string& n) {
  if( !g )
    return 0;
  SizeProperty * p = g->getLocalProperty<SizeProperty>( n );
  p->setAllNodeValue( getNodeDefaultValue() );
  p->setAllEdgeValue( getEdgeDefaultValue() );
  return p;
}
//=============================================================
void SizeProperty::copy( const node n0, const node n1, PropertyInterface * p ) {
  if( !p )
    return;
  SizeProperty * tp = dynamic_cast<SizeProperty*>(p);
  assert( tp );
  setNodeValue( n0, tp->getNodeValue(n1) );
}
//=============================================================
void SizeProperty::copy( const edge e0, const edge e1, PropertyInterface * p ) {
  if( !p )
    return;
  SizeProperty * tp = dynamic_cast<SizeProperty*>(p);
  assert( tp );
  setEdgeValue( e0, tp->getEdgeValue(e1) );
}
//=============================================================================
PropertyInterface* SizeVectorProperty::clonePrototype(Graph * g, const std::string& n) {
  if( !g )
    return 0;
  SizeVectorProperty * p = g->getLocalProperty<SizeVectorProperty>( n );
  p->setAllNodeValue( getNodeDefaultValue() );
  p->setAllEdgeValue( getEdgeDefaultValue() );
  return p;
}
//=============================================================
void SizeVectorProperty::copy( const node n0, const node n1, PropertyInterface * p ) {
  if( !p )
    return;
  SizeVectorProperty * tp = dynamic_cast<SizeVectorProperty*>(p);
  assert( tp );
  setNodeValue( n0, tp->getNodeValue(n1) );
}
//=============================================================
void SizeVectorProperty::copy( const edge e0, const edge e1, PropertyInterface * p ) {
  if( !p )
    return;
  SizeVectorProperty * tp = dynamic_cast<SizeVectorProperty*>(p);
  assert( tp );
  setEdgeValue( e0, tp->getEdgeValue(e1) );
}
//=============================================================

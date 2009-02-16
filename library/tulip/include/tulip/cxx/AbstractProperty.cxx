//-*-c++-*-
/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 26/09/2001
 This program is free software; you can redistribute it and/or modify  *
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef NDEBUG
#include <iostream>
#endif

// This is not the right place to do the initialization below
// because this file is include in many cpp files
// The initialization has to take place in only one cpp file
// TlpTools.cpp seems to be a good choice to do this
/*template <class Tnode, class Tedge, class TPROPERTY>
  TemplateFactory<PropertyFactory<TPROPERTY >, TPROPERTY, PropertyContext > AbstractProperty<Tnode,Tedge,TPROPERTY>::factory;*/

template <class Tnode, class Tedge, class TPROPERTY>
tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::AbstractProperty(tlp::Graph *sg):
  graph(sg) {
  circularCall = false;
  nodeDefaultValue = Tnode::defaultValue();
  edgeDefaultValue = Tedge::defaultValue();
  nodeProperties.setAll(Tnode::defaultValue());
  edgeProperties.setAll(Tedge::defaultValue());
}
//=============================================================
static bool isAncestor(tlp::Graph *g1, tlp::Graph *g2) {
  if(g1 == g2->getRoot())
    return true;
  tlp::Graph *currentGraph = g2;
  while(currentGraph->getSuperGraph() != currentGraph) {
    if(currentGraph == g1)
      return true;
    currentGraph = currentGraph->getSuperGraph();
  }
  return false;
}
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
bool tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::compute(const std::string &algorithm, std::string &msg, const PropertyContext& context) {

  if(!isAncestor(this->graph, context.graph))
    return false;

#ifndef NDEBUG
  std::cerr << __PRETTY_FUNCTION__ << std::endl;
#endif
  if(circularCall) {
#ifndef NDEBUG
    std::cerr << "Circular call of " << __PRETTY_FUNCTION__ << " " << msg << std::endl;
#endif
    return false;
  }
  tlp::Observable::holdObservers();
  circularCall = true;
  tlp::PropertyContext tmpContext(context);
  tmpContext.propertyProxy = this;
  tlp::PropertyAlgorithm *tmpAlgo = factory->getPluginObject(algorithm, tmpContext);
  bool result;
  if (tmpAlgo != 0) {
    result = tmpAlgo->check(msg);
    if (result) {
      tmpAlgo->run();
    }
    delete tmpAlgo;
  }
  else {
    msg = "No algorithm available with this name";
    result=false;
  }
  circularCall = false;
  notifyObservers();
  tlp::Observable::unholdObservers();
  return result;
}
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
typename Tnode::RealType tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNodeDefaultValue() { 
  return nodeDefaultValue; 
}
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
typename Tedge::RealType tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getEdgeDefaultValue() { 
  return edgeDefaultValue; 
}
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
typename tlp::ReturnType<typename Tnode::RealType>::Value tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNodeValue(const node n ) {
  return nodeProperties.get(n.id);
}
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
typename tlp::ReturnType<typename Tedge::RealType>::Value tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getEdgeValue(const edge e) {
  return edgeProperties.get(e.id);
} 
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setNodeValue(const node n,const typename Tnode::RealType &v) {
  notifyBeforeSetNodeValue(this, n);
  nodeProperties.set(n.id,v);
  notifyAfterSetNodeValue(this, n);
}
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setEdgeValue(const edge e,const typename Tedge::RealType &v) {
  notifyBeforeSetEdgeValue(this, e);
  edgeProperties.set(e.id,v);
  notifyAfterSetEdgeValue(this, e);
}
//=============================================================
template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setAllNodeValue(const typename Tnode::RealType &v) {
  notifyBeforeSetAllNodeValue(this);
  nodeDefaultValue = v;
  nodeProperties.setAll(v);
  notifyAfterSetAllNodeValue(this);
}
//============================================================
template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setAllEdgeValue(const typename Tedge::RealType &v) {
  notifyBeforeSetAllEdgeValue(this);
  edgeDefaultValue = v;
  edgeProperties.setAll(v);
  notifyAfterSetAllEdgeValue(this);
}
//==============================================================
template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::erase(const node n) {
  setNodeValue(n, nodeDefaultValue);
}
//=================================================================================
template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::erase(const edge e) {
  setEdgeValue(e, edgeDefaultValue);
}
//==============================================================
template <class Tnode, class Tedge, class TPROPERTY>
std::string tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getTypename() {
  return tlp::PropertyInterface::getTypename( this );
}
//==============================================================
// Untyped accessors
template <class Tnode, class Tedge, class TPROPERTY>
std::string tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNodeDefaultStringValue() {
  typename Tnode::RealType v = getNodeDefaultValue();
  return Tnode::toString( v );
}
template <class Tnode, class Tedge, class TPROPERTY>
std::string tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getEdgeDefaultStringValue() {
  typename Tedge::RealType v = getEdgeDefaultValue();
  return Tedge::toString( v );
}
template <class Tnode, class Tedge, class TPROPERTY>
std::string tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNodeStringValue( const node inN ) {
  typename Tnode::RealType v = getNodeValue( inN );
  return Tnode::toString( v );
}
template <class Tnode, class Tedge, class TPROPERTY> std::string
tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getEdgeStringValue( const edge inE ) {
  typename Tedge::RealType v = getEdgeValue( inE );
  return Tedge::toString( v );
}
template <class Tnode, class Tedge, class TPROPERTY>
bool tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setNodeStringValue( const node inN, const std::string & inV ) {
  typename Tnode::RealType v;
  if( !Tnode::fromString(v,inV) )
    return false;
  setNodeValue( inN, v );
  return true;
}
template <class Tnode, class Tedge, class TPROPERTY>
bool tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setEdgeStringValue( const edge inE, const std::string & inV ) {
  typename Tedge::RealType v;
  if( !Tedge::fromString(v,inV) )
    return false;
  setEdgeValue( inE, v );
  return true;
}
template <class Tnode, class Tedge, class TPROPERTY>
bool tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setAllNodeStringValue( const std::string & inV ) {
  typename Tnode::RealType v;
  if( !Tnode::fromString(v,inV) )
    return false;
  setAllNodeValue( v );
  return true;
}
template <class Tnode, class Tedge, class TPROPERTY> bool
tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setAllEdgeStringValue( const std::string & inV ) {
  typename Tedge::RealType v;
  if( !Tedge::fromString(v,inV) )
    return false;
  setAllEdgeValue( v );
  return true;
}

template<typename T>
struct PropertyValueContainer :public tlp::DataMem {
  T value;
  PropertyValueContainer(const T& val) : value(val) {
    value = val;
  }
  ~PropertyValueContainer() {
  }
};

template <class Tnode, class Tedge, class TPROPERTY>
tlp::DataMem* tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNodeDefaultDataMemValue() {
  return new PropertyValueContainer<typename Tnode::RealType>(getNodeDefaultValue());
}

template <class Tnode, class Tedge, class TPROPERTY>
tlp::DataMem* tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getEdgeDefaultDataMemValue() {
  return new PropertyValueContainer<typename Tedge::RealType>(getEdgeDefaultValue());
}

template <class Tnode, class Tedge, class TPROPERTY>
tlp::DataMem* tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNodeDataMemValue( const node inN ) {
  return new PropertyValueContainer<typename Tnode::RealType>(getNodeValue(inN));
}

template <class Tnode, class Tedge, class TPROPERTY> tlp::DataMem*
tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getEdgeDataMemValue( const edge inE ) {
  return new PropertyValueContainer<typename Tedge::RealType>(getEdgeValue(inE));
}

template <class Tnode, class Tedge, class TPROPERTY>
tlp::DataMem* tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNonDefaultDataMemValue(const node n) {
  typename Tnode::RealType value;
  if (nodeProperties.getIfNotDefaultValue(n.id, value))
    return new PropertyValueContainer<typename Tnode::RealType>(value);
  return NULL;
}

template <class Tnode, class Tedge, class TPROPERTY>
tlp::DataMem* tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNonDefaultDataMemValue( const edge e ) {
  typename Tedge::RealType value;
  if (edgeProperties.getIfNotDefaultValue(e.id, value))
    return new PropertyValueContainer<typename Tedge::RealType>(value);
  return NULL;
}


template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setNodeDataMemValue( const node inN, const tlp::DataMem* inV ) {
  setNodeValue(inN, ((PropertyValueContainer<typename Tnode::RealType> *) inV)->value);
}

template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setEdgeDataMemValue( const edge inE, const tlp::DataMem* inV ) {
  setEdgeValue(inE, ((PropertyValueContainer<typename Tedge::RealType> *) inV)->value);
}

template <class Tnode, class Tedge, class TPROPERTY>
void tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setAllNodeDataMemValue( const DataMem* inV ) {
  setAllNodeValue(((PropertyValueContainer<typename Tnode::RealType> *) inV)->value);
}

template <class Tnode, class Tedge, class TPROPERTY> void
tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::setAllEdgeDataMemValue( const DataMem* inV ) {
  setAllEdgeValue(((PropertyValueContainer<typename Tedge::RealType> *) inV)->value);
}

template <class Tnode, class Tedge, class TPROPERTY>
tlp::Iterator<tlp::node>* tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNonDefaultValuatedNodes() {
  return new tlp::UINTIterator<tlp::node>(nodeProperties.findAll(nodeDefaultValue, false));
}

template <class Tnode, class Tedge, class TPROPERTY>
tlp::Iterator<tlp::edge>* tlp::AbstractProperty<Tnode,Tedge,TPROPERTY>::getNonDefaultValuatedEdges() {
  return new tlp::UINTIterator<tlp::edge>(edgeProperties.findAll(edgeDefaultValue, false));
}

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
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <tulip/ForEach.h>
#include "ObservablePropertyTest.h"
#include <tulip/BooleanProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/StringProperty.h>

using namespace std;
using namespace tlp;

const unsigned int NB_NODES  = 4;
const unsigned int EDGE_RATIO = 3;
const unsigned int NB_EDGES = EDGE_RATIO * NB_NODES;

#include <cppunit/extensions/HelperMacros.h>
CPPUNIT_TEST_SUITE_REGISTRATION( ObservablePropertyTest );

// these classes will capture
// everything that will happen to our properties
// synchronously or asynchronously
class ObserverPTest :public Observer {
public:
  std::set<Observable*> observables;

  ObserverPTest() {}

  void reset() {
    observables.clear();
  }

  unsigned int nbObservables() {
    return observables.size();
  }

  bool found(Observable *obs) {
    return observables.find(obs) != observables.end();
  }

  // Observable methods
  void update(std::set<Observable *>::iterator begin,
	      std::set<Observable *>::iterator end) {
    reset();
    std::set<Observable *>::iterator it = begin;
    while(it != end) {
      observables.insert((*it));
      ++it;
    }
  }

  void observableDestroyed(Observable *obs) {
    observables.insert(obs);
  }    
};

static ObserverPTest* observer;

// this class will capture
// everything that will happen to our properties
class PropertyObserverTest :public PropertyObserver {
public:
  std::set<PropertyInterface*> properties;
  node lastNode;
  edge lastEdge;

  PropertyObserverTest() {}

  void reset() {
    properties.clear();
  }

  unsigned int nbProperties() {
    return properties.size();
  }

  bool found(PropertyInterface *prop) {
    return properties.find(prop) != properties.end();
  }

  node getNode() { return lastNode; }
  
  edge getEdge() { return lastEdge; }

  void beforeSetNodeValue(PropertyInterface* prop, const node n) {
    properties.insert(prop);
    lastNode = n;
  }
    
  virtual void beforeSetEdgeValue(PropertyInterface* prop, const edge e ){
    properties.insert(prop);
    lastEdge = e;
  }
    
  virtual void beforeSetAllNodeValue(PropertyInterface* prop){
    properties.insert(prop);
  }    
  virtual void beforeSetAllEdgeValue(PropertyInterface* prop){
    properties.insert(prop);
  }
  virtual void destroy(PropertyInterface* prop){
    properties.insert(prop);
  }
};

static PropertyObserverTest* pObserver;

#define DOUBLE_PROP 2
#define INTEGER_PROP 3
#define LAYOUT_PROP 4
#define SIZE_PROP 5
//==========================================================
void ObservablePropertyTest::setUp() {
  graph = tlp::newGraph();
  props[0] = new BooleanProperty(graph);
  props[1] = new ColorProperty(graph);
  props[2] = new DoubleProperty(graph);
  props[3] = new IntegerProperty(graph);
  props[4] = new LayoutProperty(graph);
  props[5] = new SizeProperty(graph);
  props[6] = new StringProperty(graph);

  vector<node> nodes;
  vector<edge> edges;
  for (unsigned int i=0; i<NB_NODES; ++i)
    nodes.push_back(graph->addNode());
  for (unsigned int i=0; i< NB_EDGES; ++i)
    graph->addEdge(nodes[rand()%NB_NODES], nodes[rand()%NB_NODES]);

  observer = new ObserverPTest();
  pObserver = new PropertyObserverTest();

  addObservers();
}

//==========================================================
void ObservablePropertyTest::tearDown() {
  delete graph;
  for (unsigned int i = 0; i < 7; ++i) {
    if (props[i])
      delete props[i];
  }

  delete observer;
  delete pObserver;
}

void ObservablePropertyTest::setNodeValue(PropertyInterface* prop, char* val,
					  bool all, bool found1,
					  bool found2) {
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  pObserver->reset();
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  
  if (all) {
    prop->setAllNodeStringValue(string(val));
    CPPUNIT_ASSERT(observer->nbObservables() == (found1 ? 1 : 0));
    CPPUNIT_ASSERT(observer->found(prop) == found1);
    CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
    CPPUNIT_ASSERT(pObserver->found(prop) == found2);
  } else {
    node n;
    forEach(n, graph->getNodes()) {
      prop->setNodeStringValue(n, string(val));
      CPPUNIT_ASSERT(observer->nbObservables() == (found1 ? 1 : 0));
      CPPUNIT_ASSERT(observer->found(prop) == found1);
      CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
      CPPUNIT_ASSERT(pObserver->found(prop) == found2);
      CPPUNIT_ASSERT(!found2 || pObserver->getNode() == n);
    }
  }
}

void ObservablePropertyTest::setEdgeValue(PropertyInterface* prop, char* val,
					  bool all, bool found1,
					  bool found2) {
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  pObserver->reset();
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  if (all) {
    prop->setAllEdgeStringValue(string(val));
    CPPUNIT_ASSERT(observer->nbObservables() == (found1 ? 1 : 0));
    CPPUNIT_ASSERT(observer->found(prop) == found1);
    CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
    CPPUNIT_ASSERT(pObserver->found(prop) == found2);
  } else {
    edge e;
    forEach(e, graph->getEdges()) {
      prop->setEdgeStringValue(e, string(val));
      CPPUNIT_ASSERT(observer->nbObservables() == (found1 ?  1 : 0));
      CPPUNIT_ASSERT(observer->found(prop) == found1);
      CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
      CPPUNIT_ASSERT(pObserver->found(prop) == found2);
      CPPUNIT_ASSERT(!found2 || pObserver->getEdge() == e);
    }
  }
}

//==========================================================
void ObservablePropertyTest::addObservers() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  for (unsigned int i = 0; i < 7; ++i) {
    props[i]->addObserver(observer);
    CPPUNIT_ASSERT(observer->nbObservables() == 0);
    props[i]->addPropertyObserver(pObserver);
    CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  }
}
//==========================================================
void ObservablePropertyTest::testAddObserver() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  for (unsigned int i = 0; i < 7; ++i) {
    CPPUNIT_ASSERT(props[i]->countObservers() == 1);
    switch(i) {
    case DOUBLE_PROP:
    case INTEGER_PROP:
    case LAYOUT_PROP:
    case SIZE_PROP:
      // some properties are self-observed
      CPPUNIT_ASSERT(props[i]->countPropertyObservers() == 2);
      break;
    default:
      CPPUNIT_ASSERT(props[i]->countPropertyObservers() == 1);
    }
  }
}
//==========================================================
void ObservablePropertyTest::testSynchronousSetNodeValue() {
  setNodeValue(props[0], "true", false, true);
  setNodeValue(props[1], "(255, 255, 255, 0)", false, true);
  setNodeValue(props[2], "1.0", false, true);
  setNodeValue(props[3], "1", false, true);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", false, true);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", false, true);
  setNodeValue(props[6], "tulip", false, true);
}

//==========================================================
void ObservablePropertyTest::testSynchronousSetEdgeValue() {
  setEdgeValue(props[0], "true", false, true);
  setEdgeValue(props[1], "(255, 255, 255, 0)", false, true);
  setEdgeValue(props[2], "1.0", false, true);
  setEdgeValue(props[3], "1", false, true);
  setEdgeValue(props[4], "()", false, true);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", false, true);
  setEdgeValue(props[6], "tulip", false, true);
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetNodeValue() {
  Observable::holdObservers();
  setNodeValue(props[0], "true", false, false);
  setNodeValue(props[1], "(255, 255, 255, 0)", false, false);
  setNodeValue(props[2], "1.0", false, false);
  setNodeValue(props[3], "1", false, false);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", false, false);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", false, false);
  setNodeValue(props[6], "tulip", false, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);
  for (unsigned int i = 0; i < 7; ++i)
    CPPUNIT_ASSERT(observer->found(props[i]));
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetEdgeValue() {
  Observable::holdObservers();
  setEdgeValue(props[0], "true", false, false);
  setEdgeValue(props[1], "(255, 255, 255, 0)", false, false);
  setEdgeValue(props[2], "1.0", false, false);
  setEdgeValue(props[3], "1", false, false);
  setEdgeValue(props[4], "()", false, false);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", false, false);
  setEdgeValue(props[6], "tulip", false, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);
  for (unsigned int i = 0; i < 7; ++i)
    CPPUNIT_ASSERT(observer->found(props[i]));
}

//==========================================================
void ObservablePropertyTest::testSynchronousSetAllNodeValue() {
  setNodeValue(props[0], "true", true, true);
  setNodeValue(props[1], "(255, 255, 255, 0)", true, true);
  setNodeValue(props[2], "1.0", true, true);
  setNodeValue(props[3], "1", true, true);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", true, true);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", true, true);
  setNodeValue(props[6], "tulip", true, true);
}

//==========================================================
void ObservablePropertyTest::testSynchronousSetAllEdgeValue() {
  setEdgeValue(props[0], "true", true, true);
  setEdgeValue(props[1], "(255, 255, 255, 0)", true, true);
  setEdgeValue(props[2], "1.0", true, true);
  setEdgeValue(props[3], "1", true, true);
  setEdgeValue(props[4], "()", true, true);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", true, true);
  setEdgeValue(props[6], "tulip", true, true);
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetAllNodeValue() {
  Observable::holdObservers();
  setNodeValue(props[0], "true", true, false);
  setNodeValue(props[1], "(255, 255, 255, 0)", true, false);
  setNodeValue(props[2], "1.0", true, false);
  setNodeValue(props[3], "1", true, false);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", true, false);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", true, false);
  setNodeValue(props[6], "tulip", true, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);
  for (unsigned int i = 0; i < 7; ++i)
    CPPUNIT_ASSERT(observer->found(props[i]));
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetAllEdgeValue() {
  Observable::holdObservers();
  setEdgeValue(props[0], "true", true, false);
  setEdgeValue(props[1], "(255, 255, 255, 0)", true, false);
  setEdgeValue(props[2], "1.0", true, false);
  setEdgeValue(props[3], "1", true, false);
  setEdgeValue(props[4], "()", true, false);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", true, false);
  setEdgeValue(props[6], "tulip", true, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);
  for (unsigned int i = 0; i < 7; ++i)
    CPPUNIT_ASSERT(observer->found(props[i]));
}

//==========================================================
void ObservablePropertyTest::testSynchronousDelete() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  for(unsigned int i = 0; i < 7; ++i) {
    delete props[i];
    CPPUNIT_ASSERT(observer->nbObservables() == i + 1);
    CPPUNIT_ASSERT(observer->found(props[i]));
    CPPUNIT_ASSERT(pObserver->nbProperties() == i + 1);
    CPPUNIT_ASSERT(pObserver->found(props[i]));
    props[i] = NULL;
  }
}

//==========================================================
void ObservablePropertyTest::testAsynchronousDelete() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  Observable::holdObservers();
  for(unsigned int i = 0; i < 7; ++i) {
    delete props[i];
    // deletion is not asynchronous
    CPPUNIT_ASSERT(observer->nbObservables() == i + 1);
    CPPUNIT_ASSERT(observer->found(props[i]));
    CPPUNIT_ASSERT(pObserver->nbProperties() == i + 1);
    CPPUNIT_ASSERT(pObserver->found(props[i]));
    props[i] = NULL;
  }
  Observable::unholdObservers();
}

//==========================================================
void ObservablePropertyTest::testRemoveObserver() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  for(unsigned int i = 0; i < 7; ++i) {
    props[i]->removeObserver(observer);
    props[i]->removePropertyObserver(pObserver);
    CPPUNIT_ASSERT(props[i]->countObservers() == 0);
    switch(i) {
    case DOUBLE_PROP:
    case INTEGER_PROP:
    case LAYOUT_PROP:
    case SIZE_PROP:
      // some properties are self-observed
      CPPUNIT_ASSERT(props[i]->countPropertyObservers() == 1);
      break;
    default:
      CPPUNIT_ASSERT(props[i]->countPropertyObservers() == 0);
    }
  }
// no more notification
  setNodeValue(props[0], "true", true, false, false);
  setNodeValue(props[1], "(255, 255, 255, 0)", true, false, false);
  setNodeValue(props[2], "1.0", true, false, false);
  setNodeValue(props[3], "1", true, false, false);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", true, false, false);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", true, false, false);
  setNodeValue(props[6], "tulip", true, false, false);
  setEdgeValue(props[0], "true", true, false, false);
  setEdgeValue(props[1], "(255, 255, 255, 0)", true, false, false);
  setEdgeValue(props[2], "1.0", true, false, false);
  setEdgeValue(props[3], "1", true, false, false);
  setEdgeValue(props[4], "()", true, false, false);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", true, false, false);
  setEdgeValue(props[6], "tulip", true, false, false);

  for(unsigned int i = 0; i < 7; ++i) {
    delete props[i];
    CPPUNIT_ASSERT(observer->nbObservables() == 0);
    CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
    props[i] = NULL;
  }
}

//==========================================================
void ObservablePropertyTest::testObserverWhenRemoveObservable() {
  CPPUNIT_ASSERT(props[0]->countPropertyObservers() == 1);
  PropertyObserverTest* pObserverTmp=new PropertyObserverTest();
  props[0]->addPropertyObserver(pObserverTmp);
  CPPUNIT_ASSERT(props[0]->countPropertyObservers() == 2);
  delete pObserverTmp;
  CPPUNIT_ASSERT(props[0]->countPropertyObservers() == 1);
}

//==========================================================
CppUnit::Test * ObservablePropertyTest::suite() {
  CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "Tulip lib : Graph" );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "Add an observer", 
								  &ObservablePropertyTest::testAddObserver) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "synchronous setNodeValue", 
								  &ObservablePropertyTest::testSynchronousSetNodeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "synchronous setEdgeValue", 
								  &ObservablePropertyTest::testSynchronousSetEdgeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "asynchronous setNodeValue", 
								  &ObservablePropertyTest::testAsynchronousSetNodeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "asynchronous setEdgeValue", 
								  &ObservablePropertyTest::testAsynchronousSetEdgeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "synchronous setAllNodeValue", 
								  &ObservablePropertyTest::testSynchronousSetAllNodeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "synchronous setAllEdgeValue", 
								  &ObservablePropertyTest::testSynchronousSetAllEdgeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "asynchronous setAllNodeValue", 
								  &ObservablePropertyTest::testAsynchronousSetAllNodeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "asynchronous setAllEdgeValue", 
								  &ObservablePropertyTest::testAsynchronousSetAllEdgeValue) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "synchronous delete", 
								  &ObservablePropertyTest::testSynchronousDelete) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "asynchronous delete", 
								  &ObservablePropertyTest::testAsynchronousDelete) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "removeObserver", 
								  &ObservablePropertyTest::testRemoveObserver) );
  suiteOfTests->addTest( new CppUnit::TestCaller<ObservablePropertyTest>( "observerWhenRemoveObservable", 
								  &ObservablePropertyTest::testObserverWhenRemoveObservable) );
  return suiteOfTests;
}
//==========================================================

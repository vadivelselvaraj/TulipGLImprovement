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
#include <sstream>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "PluginsTest.h"
#include <tulip/TlpTools.h>
#include <tulip/ForEach.h>
#include <tulip/BooleanProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/PluginLoaderTxt.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION( PluginsTest );

//==========================================================
void PluginsTest::setUp() {
  graph = tlp::newGraph();
}
//==========================================================
void PluginsTest::tearDown() {
  delete graph;
}
//==========================================================
void PluginsTest::testloadPlugin() {
  // plugin does not exist yet
  CPPUNIT_ASSERT(!tlp::PropertyPluginLister<tlp::TemplateAlgorithm<tlp::BooleanProperty> >::pluginExists("Test"));
  PluginLoaderTxt loader;
  PluginLibraryLoader::loadPluginLibrary("./testPlugin.so", &loader);
  // plugin should exist now
  CPPUNIT_ASSERT(tlp::PropertyPluginLister<tlp::TemplateAlgorithm<tlp::BooleanProperty> >::pluginExists("Test"));
  list<Dependency> deps = tlp::PropertyPluginLister<tlp::TemplateAlgorithm<tlp::BooleanProperty> >::getPluginDependencies("Test");
  // only one dependency (see testPlugin.cpp)
  CPPUNIT_ASSERT_EQUAL(size_t(1), deps.size());
  CPPUNIT_ASSERT_EQUAL(tlp::demangleTlpClassName(typeid(BooleanAlgorithm).name()), deps.front().factoryName);
  CPPUNIT_ASSERT_EQUAL(string("Test"), deps.front().pluginName);
}
//==========================================================
void PluginsTest::testCircularPlugin() {
  string name = "Test";
  string err = "Error";
  // ensure graph is not empty
  graph->addNode();
  tlp::BooleanProperty sel(graph);
  CPPUNIT_ASSERT(graph->computeProperty(name, &sel, err));
}
//==========================================================
void PluginsTest::testAncestorGraph() {
  string name = "Test";
  string err = "Error";
  PluginLibraryLoader::loadPluginLibrary("testPlugin2.so");
  Graph *child = graph->addSubGraph();
  Graph *child2 = graph->addSubGraph();
  // ensure child2 is not empty
  child2->addNode();
  Graph *child3 = child->addSubGraph();
  // ensure child3 is not empty
  child3->addNode();
  BooleanProperty sel(child);
  CPPUNIT_ASSERT(!graph->computeProperty(name, &sel, err));
  CPPUNIT_ASSERT(!child2->computeProperty(name, &sel, err));
  CPPUNIT_ASSERT(child3->computeProperty(name, &sel, err));
}

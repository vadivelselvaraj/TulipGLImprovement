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
#ifndef Tulip_GLGRAPHCOMPOSITE_H
#define Tulip_GLGRAPHCOMPOSITE_H

#include <tulip/GlComposite.h>

#include <vector>

#include <tulip/Observable.h>
#include <tulip/ObservableGraph.h>

#include <tulip/GlGraphRenderingParameters.h>
#include <tulip/GlGraphInputData.h>
#include <tulip/GlNode.h>
#include <tulip/GlEdge.h>
#include <tulip/GlMetaNode.h>

namespace tlp {

class Graph;

/** \brief Class use to represent a graph
 *
 * GlComposite use to represent a graph with nodes, metanodes and edges
 */
class TLP_GL_SCOPE GlGraphComposite : public GlComposite,
  private GraphObserver, private PropertyObserver, public Observable {

public:

  /**
   * Build a GlGraphComposite with the graph data
   */
  GlGraphComposite(Graph* graph);

  /**
   * Return the rendering parameters use for rendering
   */
  const GlGraphRenderingParameters& getRenderingParameters();
  /**
   * Set the rendering parameters use for rendering
   */
  void setRenderingParameters(const GlGraphRenderingParameters &parameter);

  /**
   * Return a pointer on rendering parameters used for rendering
   */
  GlGraphRenderingParameters* getRenderingParametersPointer();

  /**
   * Return the inputData use by the composite
   */
  GlGraphInputData* getInputData();

  /**
   * Function used to visit composite's children
   */
  virtual void acceptVisitor(GlSceneVisitor *visitor);

  /**
   * Return set of metaNodes
   */
  std::set<node> &getMetaNodes() {
    if(nodesModified) {
      metaNodes.clear();

      Graph *graph=inputData.getGraph();
      Iterator<node>* nodesIterator = graph->getNodes();

      while (nodesIterator->hasNext()) {
        node n=nodesIterator->next();

        if(graph->getNodeMetaInfo(n))
          metaNodes.insert(n);
      }

      delete nodesIterator;

      nodesModified=false;
    }

    return metaNodes;
  }

  /**
   * Get the data in XML form
   */
  void getXML(xmlNodePtr rootNode);

  /**
   * Set the data with XML
   */
  void setWithXML(xmlNodePtr rootNode);

  //Delegate
  void setDisplayNodes(bool display) {
    parameters.setDisplayNodes(display);
  }
  void setDisplayMetaNodes(bool display) {
    parameters.setDisplayMetaNodes(display);
  }
  void setDisplayEdges(bool display) {
    parameters.setDisplayEdges(display);
  }
  void setDisplayNodesLabel(bool display) {
    parameters.setViewNodeLabel(display);
  }
  void setDisplayMetaNodesLabel(bool display) {
    parameters.setViewMetaLabel(display);
  }
  void setDisplayEdgesLabel(bool display) {
    parameters.setViewEdgeLabel(display);
  }
  void setSelectedNodesStencil(int stencil) {
    parameters.setSelectedNodesStencil(stencil);
  }
  void setSelectedMetaNodesStencil(int stencil) {
    parameters.setSelectedMetaNodesStencil(stencil);
  }
  void setSelectedEdgesStencil(int stencil) {
    parameters.setSelectedEdgesStencil(stencil);
  }
  void setNodesStencil(int stencil) {
    parameters.setNodesStencil(stencil);
  }
  void setMetaNodesStencil(int stencil) {
    parameters.setMetaNodesStencil(stencil);
  }
  void setEdgesStencil(int stencil) {
    parameters.setEdgesStencil(stencil);
  }
  void setNodesLabelStencil(int stencil) {
    parameters.setNodesLabelStencil(stencil);
  }
  void setMetaNodesLabelStencil(int stencil) {
    parameters.setMetaNodesLabelStencil(stencil);
  }
  void setEdgesLabelStencil(int stencil) {
    parameters.setEdgesLabelStencil(stencil);
  }

  bool isDisplayNodes() {
    return parameters.isDisplayNodes();
  }
  bool isDisplayMetaNodes() {
    return parameters.isDisplayMetaNodes();
  }
  bool isDisplayEdges() {
    return parameters.isDisplayEdges();
  }
  bool isDisplayNodesLabel() {
    return parameters.isViewNodeLabel();
  }
  bool isDisplayMetaNodesLabel() {
    return parameters.isViewMetaLabel();
  }
  bool isDisplayEdgesLabel() {
    return parameters.isViewEdgeLabel();
  }
  int getSelectedNodesStencil() {
    return parameters.getSelectedNodesStencil();
  }
  int getSelectedMetaNodesStencil() {
    return parameters.getSelectedMetaNodesStencil();
  }
  int getSelectedEdgesStencil() {
    return parameters.getSelectedEdgesStencil();
  }
  int getNodesStencil() {
    return parameters.getNodesStencil();
  }
  int getMetaNodesStencil() {
    return parameters.getMetaNodesStencil();
  }
  int getEdgesStencil() {
    return parameters.getEdgesStencil();
  }
  int getNodesLabelStencil() {
    return parameters.getNodesLabelStencil();
  }
  int getMetaNodesLabelStencil() {
    return parameters.getMetaNodesLabelStencil();
  }
  int getEdgesLabelStencil() {
    return parameters.getEdgesLabelStencil();
  }

protected:

  /**
   * Function use by the GraphObserver when a node is create in the graph
   */
  virtual void addNode(Graph *,const node) {
    nodesModified=true;
    haveToSort=true;
  }
  /**
   * Function use by the GraphObserver when an edge is create in the graph
   */
  virtual void addEdge(Graph *,const edge ) {
    haveToSort=true;
  }
  /**
   * Function use by the GraphObserver when a node is delete in the graph
   */
  virtual void delNode(Graph *,const node) {
    nodesModified=true;
    haveToSort=true;
  }
  /**
   * Function use by the GraphObserver when an edge is delete in the graph
   */
  virtual void delEdge(Graph *,const edge ) {
    haveToSort=true;
  }
  virtual void afterSetNodeValue(PropertyInterface*, const node) {
    nodesModified=true;
  }
  /**
   * Function use by the GraphObserver when the graph is delete
   */
  virtual void destroy(Graph *);

  // override Observable::treatEvent
  void treatEvent(const Event& evt) {
    if (typeid(evt) == typeid(GraphEvent) ||
        (evt.type() == Event::TLP_DELETE &&
         dynamic_cast<Graph*>(evt.sender())))
      GraphObserver::treatEvent(evt);
    else
      PropertyObserver::treatEvent(evt);
  }

  void buildSortedList();
  void acceptVisitorForNodes(Graph *graph,GlSceneVisitor *visitor);
  void acceptVisitorForEdges(Graph *graph,GlSceneVisitor *visitor);

  GlGraphRenderingParameters parameters;
  GlGraphInputData inputData;
  Graph *rootGraph;

  bool haveToSort;
  bool nodesModified;
  std::set<node> metaNodes;
};
}

#endif

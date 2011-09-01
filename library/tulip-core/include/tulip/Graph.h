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
#ifndef Tulip_SUPERGRAPH_H
#define Tulip_SUPERGRAPH_H

#include <iostream>
#include <string>
#include <vector>

#include <climits>
#include <tulip/tulipconf.h>
#include <tulip/DataSet.h>
#include <tulip/PluginContext.h>
#include <tulip/Node.h>
#include <tulip/Edge.h>
#include <tulip/Observable.h>
#include <tulip/ObservableGraph.h>

namespace tlp {

class BooleanProperty;
class PluginProgress;
template<class C>struct Iterator;

/**
 * @enum This Enum describes the possible types of an element of the graph.
 *
 * It is used for instance for functions that can return an edge or a node, to distinguish between the two cases.
 **/
enum ElementType {
  /** This element describes a node **/
  NODE = 0,
  /** This element describes an edge **/
  EDGE = 1
};

/**
 * @brief Loads a graph in the tlp format from a file (extension can be .tlp or .tlp.gz).
 * This function uses the "tlp" import plugin, and will fail if it is not loaded (By default this plugin is linked into the library and should be loaded).
 *
 * If the import fails (no such file, parse error, ...) NULL is returned.
 *
 * @param filename The file in tlp format to parse.
 * @return :Graph* The imported Graph, NULL if the import failed.
 **/
TLP_SCOPE Graph * loadGraph(const std::string &filename);

/**
 * @brief Saves a graph to a file in the tlp format. Extension of the destination file can be either be .tlp (human-readable text file) or .tlp.gz (gzipped text file).
 *
 * This function checks the file name for the '.gz' extension and uses a compressed output if found.
 *
 * This function uses the "tlp" import plugin, and will fail if it is not loaded (By default this plugin is linked into the library and should be loaded).
 *
 * @param  The graph to save.
 * @param filename The file to save the graph to.
 * @return bool Whether the export was successfull or not.
 **/
TLP_SCOPE bool saveGraph(Graph* graph, const std::string &filename);

/**
 * @brief Exports a graph using the specified export plugin with parameters stored in the DataSet.
 *
 * You determine the destination, whether by using a fstream, or by saving the contents of the stream to the destination of your choice.
 *
 * @param graph The graph to export.
 * @param outputStream The stream to export to. Can be a standard ostream, an ofstream, or even a gzipped ostream.
 * @param format The format to use to export the Graph.
 * @param dataSet The parameters to pass to the export plugin (e.g. additional data, options for the format)
 * @param progress A PluginProgress to report the progress of the operation, as well as final state. Defaults to NULL.
 * @return bool Whether the export was successfull or not.
 **/
TLP_SCOPE bool exportGraph(Graph *graph, std::ostream &outputStream, const std::string &format, DataSet &dataSet, PluginProgress *progress=NULL);

/**
 * @brief Imports a graph using the specified import plugin with the parameters stored in the DataSet.
 *
 * If no graph is passed, then a new graph will be created. You can pass a graph in order to import data into it.
 * Returns the graph with imported data, or NULL if the import failed. In this case, the Pluginprogress should have an error that can be displayed.
 *
 * @param format The format to use to import the graph.
 * @param dataSet The parameters to pass to the import plugin (file to read, ...)
 * @param progress A PluginProgress to report the progress of the operation, as well as final state. Defaults to NULL.
 * @param newGraph The graph to import the data into. This can be usefull to import data into a subgraph. Defaults to NULL.
 * @return :Graph* The graph containing the imported data, or NULL in case of failure.
 **/
TLP_SCOPE Graph* importGraph(const std::string &format, DataSet &dataSet, PluginProgress *progress=NULL,Graph *newGraph=NULL);

/**
 * @brief Creates a new, empty graph.
 *
 * This is a simple method factory to create a Graph implementation (remember, Graph is only an interface).
 *
 * This is the recommended way to create a new Graph.
 *
 * @return :Graph* A new, empty graph.
 **/
TLP_SCOPE Graph* newGraph();

/**
 * @brief Creates and returns an empty subgraph of the given graph.
 *
 * @deprecated this functions should not be used anymore, please use Graph::addSubGraph() instead.
 *
 * @param graph The graph to add an empty subgraph to.
 * @param name The name of the new subgraph. Defaults to "unnamed".
 * @return :Graph* The newly created subgraph.
 **/
TLP_SCOPE _DEPRECATED Graph* newSubGraph(Graph *graph, std::string name = "unnamed");

/**
 * @brief Creates and returns a subgraph of the graph that is equal to root (a clone subgraph).
 *
 * @deprecated A new method should be added to Graph to perform this, because object-oriented programming and stuff.
 *
 * @param graph The Graph on which to create a clone subgraph.
 * @param name The name of the newly created subgraph. Defaults to "unnamed".
 * @return :Graph* The newly created clone subgraph.
 **/
TLP_SCOPE _DEPRECATED Graph* newCloneSubGraph(Graph *graph, std::string name = "unnamed");

/**
 * Appends the selected part of the graph inG (properties, nodes and edges) into the graph outG.
 * If no selection is done (inSel=NULL), the whole inG graph is appended.
 * The output selection is used to select the appended nodes & edges
 * \warning The input selection is extended to all selected edge ends.
 */
TLP_SCOPE void copyToGraph(Graph *outG, const Graph *inG, BooleanProperty* inSelection=0, BooleanProperty* outSelection=0 );

/**
 * Removes the selected part of the graph ioG (properties values, nodes and edges).
 * If no selection is done (inSel=NULL), the whole graph is reseted to default value.
 * \warning The selection is extended to all selected edge ends.
 */
TLP_SCOPE void removeFromGraph(Graph * ioG, BooleanProperty* inSelection = 0 );

/**
 * \defgroup graphs Graphs
 */
/*@{*/
/// Interface for a graph
/**
 * The class Graph is the interface of a Graph in the Tulip Library.
 */
class TLP_SCOPE Graph : public Observable {

  friend class GraphAbstract;
  friend class GraphUpdatesRecorder;
  friend class GraphDecorator;
  friend class PropertyManager;

public:
  Graph():id(0) {}
  virtual ~Graph() {}

  /**
   * @brief Applies an algorithm plugin, identified by its name.
   * Algorithm plugins are subclasses of the tlp::Algorithm interface.
   * Parameters are transmitted to the algorithm trough the DataSet.
   * To determine a plugin's parameters, you can either:
   *
   * * refer to its documentation
   *
   * * use buildDefaultDataSet on the plugin object if you have an instance of it
   *
   * * call getPluginParameters() with the name of the plugin on the right PluginLister
   * (there cannot be a static method that directly returns this from the plugin name as a name can be used for plugins of different types).
   *
   *
   * If an error occurs, a message describing the error should be stored in errorMessage.
   *
   * @param errorMessage A string that will be modified to contain an error message should an error occur.
   * @param dataSet The parameters to the algorithm. Defaults to NULL.
   * @param algorithm The algorithm to apply. Defaults to "any".
   * @param progress A PluginProgress to report the progress of the operation, as well as final state. Defaults to NULL.
   * @return bool Whether the algorithm was successfully applied.
  **/
  bool applyAlgorithm(std::string &errorMessage, DataSet *dataSet=NULL,const std::string &algorithm="any", PluginProgress *progress=NULL);

  //=========================================================================
  // Graph hierarchy access and building
  //=========================================================================

  /**
   * @brief Removes all nodes, edges and sub-graphs from this graph.
   *
   * Contrarily to creating a new Graph, this keeps attributes and properties.
   *
   * @return void
   **/
  virtual  void clear()=0;

  /**
   * @brief Creates and returns a new sub-graph of this graph.
   *
   * If a BooleanProperty is provided, all the nodes and edges for which it is true will be added to the subgraph.
   * If none is provided, then the subgraph will be empty.
   *
   * The id parameter should only be provided if you know exactly what you are doing; as Tulip will manage the subgraphs IDs when left to 0.
   * It is only used by the Graph loading as subgraphs ids are preserved when saving/loading a Graph.
   *
   * @param selection The elements to add to the new subgraph. Defaults to 0.
   * @param id The ID you wish to assign to the Graph. It is strongly advised to leave this as default and let Tulip manage subgraph IDs. Defaults to 0.
   * @return :Graph* The newly created subgraph.
   **/
  virtual Graph *addSubGraph(BooleanProperty *selection=0, unsigned int id = 0)=0;

  /**
   *  Creates and returns a new sub-graph of the graph
   *  induced by a set of nodes. The sub-graph contains all
   *  the nodes of the set and all the existing edges between
   *  two nodes of the set including self-loops.
   *  If the parentSubGraph argument is not null,
   *  it will be the parent of the induced sub-graph instead of this.
   */
  Graph *inducedSubGraph(const std::set<node>& nodeSet,
                         Graph* parentSubGraph = NULL);
  /**
   * Deletes a sub-graph of this graph.
   * The sub-graph's sub-graphs become sub-graphs of the graph.
   */
  virtual void delSubGraph(Graph *)=0;
  /**
   * Deletes a sub-graph of this graph and all its sub-graphs.
   */
  virtual void delAllSubGraphs(Graph *)=0;
  /**
   * Returns the parent of the graph,
   *  if it has no parent (is the root graph), it returns itself.
   */
  virtual Graph* getSuperGraph()const =0;
  /**
   * Returns the root graph of the graph hierarchy.
   */
  virtual Graph* getRoot() const =0;
  /**
   * Sets the parent of the graph (use very carefully)
   * Standard users should never use this function.
   */
  virtual void setSuperGraph(Graph *)=0;
  /**
   * Returns an iterator on all the sub-graphs of the graph.
   */
  virtual Iterator<Graph *> * getSubGraphs() const=0;
  /**
    This method returns the nth subgraph.
    Since order cannot be ensured in every implementation, this method should be equivalent to:
    int i=0;
    Iterator<Graph *> *it = g->getSubGraphs();
    while (it->hasNext()) {
      Graph *result = it->next();
      if (i++ == n) {
        delete it;
        return result;
      }
    }
    delete it;
    return NULL;
    */
  virtual Graph *getNthSubGraph(unsigned int n) const;
  /**
    Return the number of direct sub-graphs
    */
  virtual unsigned int numberOfSubGraphs() const=0;
  /**
    Return the number of descendant graphs
    */
  virtual unsigned int numberOfDescendantGraphs() const=0;
  /**
   * Indicates if the graph argument is a direct sub-graph.
   */
  virtual bool isSubGraph(const Graph* sg) const=0;
  /**
   * Indicates if the graph argument is a descendant of this graph.
   */
  virtual bool isDescendantGraph(const Graph* sg) const=0;
  /**
   * Returns a pointer on the sub-graph with the corresponding id
   * or NULL if there is no sub-graph with that id.
   */
  virtual Graph* getSubGraph(unsigned int id) const=0;
  /**
   * Returns a pointer on the descendant with the corresponding id
   * or NULL if there is no descendant  with that id.
   */
  virtual Graph* getDescendantGraph(unsigned int id) const=0;
  //==============================================================================
  // Modification of the graph structure
  //==============================================================================
  /**
   * Adds a new node in the graph and returns it. This node is also added in all
   * the graph ancestors to maintain the sub-graph relation between graphs.
   */
  virtual node addNode()=0;
  /**
   * Adds new nodes in the graph and returns them the addedNodes vector.
   * The new nodes are also added in all
   * the graph ancestors to maintain the sub-graph relation between graphs.
   * Warning the addedNodes vector is cleared before adding nodes
   */
  virtual void addNodes(unsigned int nbNodes, std::vector<node>& addedNodes)=0;
  /**
   * Adds an existing node in the graph. This node is also added in all
   * the graph ancestors to maintain the sub-graph relation between graphs.
   * Warning, the node must be an element of the graph hierarchy, thus it must be
   * an element of the root graph.
   * Warning : One can't add an existing node to the root graph.
   */
  virtual void addNode(const node)=0;
  /**
   * Adds existing nodes in the graph. The nodes are also added in all
   * the graph ancestors to maintain the sub-graph relation between graphs.
   * Warning, the added nodes must be elements of the graph hierarchy,
   * thus they must be elements of the root graph.
   * Warning : One can't add existing nodes to the root graph.
   */
  virtual void addNodes(Iterator<node>* nodes)=0;
  /**
   * Deletes a node in the graph. This node is also removed in
   * the sub-graphs hierarchy of the current graph to maintain
   * the sub-graph relation between graphs.
   * If deleteInAllGraphs is true, the node is deleted
   * in the whole hierarchy of graphs.
   */
  virtual void delNode(const node n, bool deleteInAllGraphs = false)=0;
  /**
   * Deletes nodes in the graph. These nodes are also removed in
   * the sub-graphs hierarchy of the current graph to maintain
   * the sub-graph relation between graphs.
   * If deleteInAllGraphs is true, these nodes are  deleted
   * in the whole hierarchy of graphs.
   * It is the responsibility of the caller to delete the iterator.
   */
  virtual void delNodes(Iterator<node>* itN, bool deleteInAllGraphs = false)=0;
  /**
   * Adds a new edge in the graph and returns it. This edge is also added in all
   * the super-graph of the graph to maintain the sub-graph relation between graphs.
   */
  virtual edge addEdge(const node, const node )=0;
  /**
   * Adds new edges in the graph and returns them the addedNodes vector.
   * The new nodes are also added in all
   * the graph ancestors to maintain the sub-graph relation between graphs.
   * Warning : the addedEdges vector is cleared before adding edges
   */
  virtual void addEdges(const std::vector<std::pair<node, node> >& edges,
                        std::vector<edge>& addedEdges)=0;
  /**
   * Adds an existing edge in the graph. This edge is also added in all
   * the graph's ancestors to maintain the sub-graph relation between graphs.
   * Warning, the edge must be an element of the graph hierarchy, thus it must be
   * an element of the root graph.
   * Warning : One can't add an existing edge to the root graph.
   */
  virtual void addEdge(const edge )=0;
  /**
   * Adds existing edges in the graph. The edges are also added in all
   * the graph ancestors to maintain the sub-graph relation between graphs.
   * Warning, the added edges must be elements of the graph hierarchy,
   * thus they must be elements of the root graph.
   * Warning : One can't add existing edges to the root graph.
   */
  virtual void addEdges(Iterator<edge>* edges)=0;
  /**
   * Deletes an edge in the graph. The edge is also removed in
   * the sub-graphs hierarchy of the current graph to maintain
   * the sub-graph relation between graphs.
   * If deleteInAllGraphs is true, the edge is deleted
   * in the whole hierarchy of graphs.
   * The ordering of remaining edges is preserved.
   */
  virtual void delEdge(const edge e, bool deleteInAllGraphs = false)=0;
  /**
   * Deletes edges in the graph. These edges are also removed in
   * the sub-graphs hierarchy of the current graph to maintain
   * the sub-graph relation between graphs.
   * If deleteInAllGraphs is true, the edges are deleted
   * in the whole hierarchy of graphs.
   * The ordering of remaining edges is preserved.
   * It is the responsibility of the caller to delete the iterator.
   */
  virtual void delEdges(Iterator<edge>* itE, bool deleteInAllGraphs = false)=0;
  /**
   * Sets the order of the edges around a node. This operation
   * ensures that adjacent edges of a node will
   * be ordered as they are in the vector of edges given in parameter.
   */
  virtual void setEdgeOrder(const node,const std::vector<edge> & )=0;
  /**
   * Swaps two edges in the adjacency list of a node.
   */
  virtual void swapEdgeOrder(const node,const edge , const edge )=0;
  /// Sets the source of an existing edge.
  virtual void setSource(const edge, const node) = 0;
  /// Sets the target of an existing edge.
  virtual void setTarget(const edge, const node) = 0;
  /// Sets both the source and target of an existing edge.
  virtual void setEnds(const edge, const node source, const node target) = 0;
  /**
   *  Reverses the direction of an edge, the source becomes the target and the target
   *  becomes the source.
   *  Warning : The ordering is global to the entire graph hierarchy. Thus, by changing
   *  the ordering of a graph you change the ordering of the hierarchy.
   */
  virtual void reverse(const edge)=0;
  // Attempts to reserve enough space to store nodes.
  // Only defined on root graph.
  virtual void reserveNodes(unsigned int nbNodes) = 0;
  // Attempts to reserve enough space to store edges.
  // Only defined on root graph.
  virtual void reserveEdges(unsigned int nbEdges) = 0;
  //================================================================================
  //Iterators on the graph structure.
  //================================================================================
  /**
   * @brief Finds the first node whose input degree equals 0.
   *
   * @return tlp::node The first encountered node with input degree of 0, or an invalid node if none was found.
   **/
  virtual tlp::node getSource() const;

  /// Returns an existing node of the graph.
  virtual node getOneNode() const =0;
  /// Returns an iterator on the nodes.
  virtual Iterator<node>* getNodes() const =0;
  /// Returns the ith predecessor of a node.
  virtual node getInNode(const node,unsigned int )const =0;
  /// Returns an iterator on the predecessors of a node.
  virtual Iterator<node>* getInNodes(const node) const =0;
  /// Returns the ith successor of a node.
  virtual node getOutNode(const node,unsigned int) const =0;
  /// Returns an iterator on the successors of a node.
  virtual Iterator<node>* getOutNodes(const node) const =0;
  /// Returns an iterator on the neighbours of a node.
  virtual Iterator<node>* getInOutNodes(const node) const =0;
  /// Returns the underlying graph of a meta node.
  virtual Graph* getNodeMetaInfo(const node) const = 0;
  /// Returns an iterator on the edges.
  virtual Iterator<edge>* getEdges() const =0;
  /// Returns an existing edge of the graph.
  virtual edge getOneEdge() const =0;
  /// Returns an iterator on the outgoing edges of a node.
  virtual Iterator<edge>* getOutEdges(const node) const =0;
  /// Returns an iterator on the incoming and outgoing edges of a node.
  virtual Iterator<edge>* getInOutEdges(const node) const =0;
  /// Returns an iterator on the incoming edges of a node.
  virtual Iterator<edge>* getInEdges(const node) const =0;
  /// Returns an iterator on the underlying edges of a meta edge.
  virtual Iterator<edge>* getEdgeMetaInfo(const edge) const =0;
  //================================================================================
  // Graph, nodes and edges informations about the graph stucture
  //================================================================================
  /// Returns the graph's id. This id is unique.
  unsigned int getId() const {
    return id;
  }
  /// Returns the number of nodes in the graph.
  virtual unsigned int numberOfNodes()const =0;
  /// Returns the number of edges in the graph.
  virtual unsigned int numberOfEdges()const =0;
  /// Returns the degree of a node.
  virtual unsigned int deg(const node)const =0;
  /// Returns the incoming degree of a node.
  virtual unsigned int indeg(const node)const =0;
  /// Returns the outgoing degree of a node.
  virtual unsigned int outdeg(const node)const =0;
  /// Returns the source of the edge.
  virtual node source(const edge)const =0;
  /// Returns the target of the edge.
  virtual node target(const edge)const =0;
  /// Returns the source and target of the edge
  /// as the first and second of the returned pair.
  virtual const std::pair<node, node>& ends(const edge)const=0;
  /// Returns the opposite node for n in the edge e.
  virtual node opposite(const edge, const node)const =0;
  /// Returns true if the node is an element of the graph.
  virtual bool isElement(const node ) const =0;
  /// Returns true if the node is a meta node.
  virtual bool isMetaNode(const node ) const =0;
  /// Returns true if the edge is an element of the graph.
  virtual bool isElement(const edge ) const =0;
  /// Returns true if the edge is a meta edge.
  virtual bool isMetaEdge(const edge ) const =0;
  /** Returns the edge if it exists an edge between two nodes.
   *  The 'directed' flag indicates if the direction of the edge
   *  (from source to target) must be taken in to account.
   *  If no edge is found, returns an invalid edge.
   */
  virtual edge existEdge(const node source, const node target,
                         bool directed = true) const =0;
  //================================================================================
  // Access to the graph attributes and to the node/edge property.
  //================================================================================
  /// Returns graph attributes.
  const DataSet & getAttributes() const {
    return (const_cast<Graph *>(this))->getNonConstAttributes();
  }
  /// Get an attribute of the graph; returns true if a value was found
  /// false if not.
  template<typename ATTRIBUTETYPE>
  bool getAttribute(const std::string &name, ATTRIBUTETYPE& value) const;
  /// Deprecated version of the previous method.
  template<typename ATTRIBUTETYPE>
  ATTRIBUTETYPE getAttribute(const std::string &name) const;
  /// Untyped accessor returning a copy.
  DataType* getAttribute(const std::string& name) const;
  /// Sets an attribute of the graph.
  template<typename ATTRIBUTETYPE>
  void setAttribute(const std::string &name,const ATTRIBUTETYPE &value);
  /// Sets attritute from an untyped value.
  void setAttribute(const std::string &name, const DataType* value);
  /// Removes an existing attribute.
  void removeAttribute(const std::string &name) {
    notifyRemoveAttribute(name);
    getNonConstAttributes().remove(name);
  }
  /// Returns if the attribute exist.
  bool attributeExist(const std::string &name) {
    return getAttributes().exist(name);
  }
  /**
   *  Adds a property to the graph.
   *  Be aware that the PropertyInterface will now belong to the graph object;
   *  and so it will be deleted automatically. Using of delete on that property
   *  will cause a segmentation violation (use delLocalProperty instead).
   */
  virtual  void addLocalProperty(const std::string &name, PropertyInterface *prop)=0;
  /**
   * Returns a pointer to a PropertyInterface which is in the graph properties pool.
   * The real type of the PropertyInterface is tested with the template parameter.
    * If the PropertyInterface is not in the pool, a new one is created and returned.
    * Using of delete on that property will cause an abort of the program
    * (use delLocalProperty instead).
    */
  template<typename PropertyType>
  PropertyType* getLocalProperty(const std::string &name);

  /**
   * Try to returns a pointer to a PropertyInterface PropertyInterface which is in the graph properties pool.
   * The real type of the PropertyInterface is tested with the propertyType string parameter.
   * If the PropertyInterface is not in the pool, a new one is created and returned.
   * If the type parameter is not a valid property type this function returns a NULL pointer.
   * Using of delete on that property will cause an abort of the program
   * (use delLocalProperty instead).
   */
  PropertyInterface *getLocalProperty(const std::string& propertyName, const std::string& propertyType);

  /**
   * Computes a property on the current graph
   * using an external named algorithm (plug-in).
   * The computed values will be stored in result. Warning: previous values stored in result will be deleted.
   * If the function returns false, an error message is stored in msg.
   * One can give a PluginProgress to the algorithm in order to have some feedback or to stop
   * the algorithm during its computation. One can give parameters to the algorithm
   * using the DataSet. In some cases algorithms can use this DataSet in order
   * to return some external information (not stored in result).
   */
  template<typename PropertyType>
  bool computeProperty(const std::string &algorithm, PropertyType* result, std::string &msg,
                       PluginProgress *progress=0, DataSet *data=0);
  /**
   * Returns a pointer to a PropertyInterface which is in the graph properties pool or in the pool of an ancestor in the sub-graphs hierarchy.
   * The real type of the PropertyInterface is tested with the template parameter.
   * If the PropertyInterface is not in the pool, it creates a new one and returns it.
   * Using of delete on that property will cause a segmentation violation
   * (use delLocalProperty instead).
   */
  template<typename PropertyType>
  PropertyType* getProperty(const std::string &name);
  /**
   * Returns a pointer on an existing property. If the property does not
   * exist return NULL.
   * In DEBUG the existence of a property is checked using an assertion.
   */
  virtual PropertyInterface* getProperty(const std::string& name)=0;
  /**
    * Try to returns a pointer to a PropertyInterface PropertyInterface which is in the graph properties pool or in the pool of an ancestor in the sub-graphs hierarchy.
    * The real type of the PropertyInterface is tested with the propertyType string parameter.
    * If the PropertyInterface is not in the pool, a new one is created and returned.
    * If the type parameter is not a valid property type this function returns a NULL pointer.
    * Using of delete on that property will cause an abort of the program
    * (use delLocalProperty instead).
    */
  PropertyInterface *getProperty(const std::string& propertyName, const std::string& propertyType);
  /**
   *  Returns true if a property of that name exists
   *  in the graph properties pool or in the pool of an ancestor in the sub-graphs hierarchy.
   */
  virtual  bool existProperty(const std::string& name)=0;
  /**
   * Returns true if a property of that name exists in the graph properties pool.
   */
  virtual  bool existLocalProperty(const std::string& name)=0;
  /**
   * Removes and deletes the property associated to name in the graph properties pool.
   */
  virtual  void delLocalProperty(const std::string& name)=0;
  /**
   * Returns an iterator on the names of the properties local to the graph.
   */
  virtual Iterator<std::string>* getLocalProperties() const=0;
  /**
   * Returns an iterator on the names of the properties inherited from the graph���s ancestors.
   */
  virtual Iterator<std::string>* getInheritedProperties() const=0;
  /**
   * Returns an iterator on the name of all the properties attached to the graph.
   */
  virtual Iterator<std::string>* getProperties() const=0;
  /**
   * Returns an iterator on the properties local to the graph.
   */
  virtual Iterator<PropertyInterface*>* getLocalObjectProperties() const=0;
  /**
   * Returns an iterator on the properties inherited from the graph���s ancestors.
   */
  virtual Iterator<PropertyInterface*>* getInheritedObjectProperties() const=0;
  /**
   * Returns an iterator on all the properties attached to the graph.
   */
  virtual Iterator<PropertyInterface*>* getObjectProperties() const=0;

  // observation mechanism
  /**
   * Register a new observer
   */
  void _DEPRECATED addGraphObserver(Observable *) const;
  /**
   * Returns the number of observers
   */
  unsigned int _DEPRECATED countGraphObservers() const {
    return countListeners();
  }
  /**
   * Remove an observer
   */
  void _DEPRECATED removeGraphObserver(Observable *) const;

  // updates management
  /*
   * Marks the state of the current root graph in the hierarchy.
   * The next updates will be recorded in order to be undone at the
   * next call of the pop method. Be careful that all the updates are undone
   * except those who affect the ordering of edges.
   * If the argument unpopAllowed is set to false, the next updates
   * could not be replayed after undone. If some previously undone
   * updates exist they could no longer be replayed.
   */
  virtual void push(bool unpopAllowed = true)=0;
  /*
   * Restores a previously marked state of the current root graph
   * in the hierarchy. The restored state does not remain marked.
   * If the argument unpopAllowed is set to false then
   * the undone updates could not be replayed.
   */
  virtual void pop(bool unpopAllowed = true)=0;
  /*
   * Indicates that next updates of the property given in argument
   * will be preserved during the next call of the pop method.
   * Returns false if updates of that property are already recorded
   * since the last call of the push method.
   */
  virtual bool nextPopKeepPropertyUpdates(PropertyInterface* prop)=0;
  /*
   * Marks again the current state of the root graph hierarchy
   * and replays the last updates previously undone.
   */
  virtual void unpop()=0;
  /*
   * Returns true if a previously marked state can be restored.
   */
  virtual bool canPop()=0;
  /*
   * Returns true if some previously undone updates can be replayed.
   */
  virtual bool canUnpop()=0;
  /*
   * Returns true if the current state updates can be undone then replayed.
   */
  virtual bool canPopThenUnpop()=0;

  // meta nodes management
  /**
   * Closes a set of existing nodes into a metanode and returns it.
   * Edges from nodes in the set to other nodes are replaced with
   * edges from the metanode to the other nodes.
   * Warning: this method will failed when called on the root graph.
   *
   * \param nodeSet: a set of existing nodes
   * \param multiEdges: indicates if a meta edge will be created for each underlying edge
   * \param delAllEdge: indicates if the underlying edges will be removed from the entire hierarchy
   */
  node createMetaNode(const std::set<node> &nodeSet, bool multiEdges = true, bool delAllEdge = true);
  /**
   * Populates a quotient graph with one meta node
   * for each iterated graph.
   *
   * \param itS: a Graph iterator, (typically a subgraph iterator)
   * \param quotientGraph: the graph that will contain the meta nodes
   * \param metaNodes: will contains all the added meta nodes after the call
   *
   */
  void createMetaNodes(Iterator<Graph *> *itS, Graph *quotientGraph,
                       std::vector<node>& metaNodes);
  /**
   * Closes an existing subgraph into a metanode.  Edges from nodes
   * in the subgraph to nodes outside the subgraph are replaced with
   * edges from the metanode to the nodes outside the subgraph.
   * Warning: this method will failed when called on the root graph.
   *
   * \param subGraph: an existing subgraph
   * \param multiEdges: indicates if a meta edge will be created for each underlying edge
   * \param delAllEdge: indicates if the underlying edges will be removed from the entire hierarchy
   */
  node createMetaNode(Graph* subGraph, bool multiEdges = true, bool delAllEdge = true);
  /**
   * Opens a metanode and replaces all edges between that
   * meta node and other nodes in the graph.
   * @param updateProperties If set to true, open meta node will update inner nodes layout, color, size, etc
   * Warning: this method will failed when called on the root graph.
   */
  void openMetaNode(node n, bool updateProperties=true);

protected:
  virtual DataSet &getNonConstAttributes() = 0;
  // designed to reassign an id to a previously deleted elt
  // used by GraphUpdatesRecorder
  virtual node restoreNode(node)=0;
  virtual void restoreNodes(const std::vector<node>& nodes)=0;
  virtual edge restoreEdge(edge, node source, node target)=0;
  virtual void restoreEdges(const std::vector<edge>& edges,
                            const std::vector<std::pair<node, node> >& ends)=0;
  // designed to only update own structures
  // used by GraphUpdatesRecorder
  virtual void removeNode(const node)=0;
  virtual void removeEdge(const edge)=0;

  // to check if a property can be deleted
  // used by PropertyManager
  virtual bool canDeleteProperty(Graph* g, PropertyInterface *prop) {
    return getRoot()->canDeleteProperty(g, prop);
  }

  // to deal with sub graph deletion
  virtual void removeSubGraph(Graph*, bool notify = false)=0;
  virtual void clearSubGraphs()=0;
  // only called by GraphUpdatesRecorder
  virtual void restoreSubGraph(Graph*, bool undoOrRedo = false)=0;
  virtual void setSubGraphToKeep(Graph*)=0;

  // for notification of GraphObserver
  void notifyAddNode(const node n);
  void notifyAddNode(Graph*, const node n) {
    notifyAddNode(n);
  }
  void notifyAddEdge(const edge e);
  void notifyAddEdge(Graph*, const edge e) {
    notifyAddEdge(e);
  }
  void notifyBeforeSetEnds(const edge e);
  void notifyBeforeSetEnds(Graph*, const edge e) {
    notifyBeforeSetEnds(e);
  }
  void notifyAfterSetEnds(const edge e);
  void notifyAfterSetEnds(Graph*, const edge e) {
    notifyAfterSetEnds(e);
  }
  void notifyDelNode(const node n);
  void notifyDelNode(Graph*, const node n) {
    notifyDelNode(n);
  }
  void notifyDelEdge(const edge e);
  void notifyDelEdge(Graph*, const edge e) {
    notifyDelEdge(e);
  }
  void notifyReverseEdge(const edge e);
  void notifyReverseEdge(Graph*, const edge e) {
    notifyReverseEdge(e);
  }
  void notifyAddSubGraph(const Graph*);
  void notifyAddSubGraph(Graph*, const Graph* sg) {
    notifyAddSubGraph(sg);
  }
  void notifyDelSubGraph(const Graph*);
  void notifyDelSubGraph(Graph*, const Graph* sg) {
    notifyDelSubGraph(sg);
  }

  void notifyAddDescendantGraph(const Graph*);
  void notifyDelDescendantGraph(const Graph*);

  void notifyAddLocalProperty(const std::string&);
  void notifyAddLocalProperty(Graph*, const std::string& name) {
    notifyAddLocalProperty(name);
  }
  void notifyBeforeDelLocalProperty(const std::string&);
  void notifyAfterDelLocalProperty(const std::string&);
  void notifyDelLocalProperty(Graph*, const std::string& name) {
    notifyBeforeDelLocalProperty(name);
  }
  void notifyBeforeSetAttribute(const std::string&);
  void notifyBeforeSetAttribute(Graph*, const std::string& name) {
    notifyBeforeSetAttribute(name);
  }
  void notifyAfterSetAttribute(const std::string&);
  void notifyAfterSetAttribute(Graph*, const std::string& name) {
    notifyAfterSetAttribute(name);
  }
  void notifyRemoveAttribute(const std::string&);
  void notifyRemoveAttribute(Graph*, const std::string& name) {
    notifyRemoveAttribute(name);
  }
  void notifyDestroy();
  void notifyDestroy(Graph*) {
    notifyDestroy();
  }

protected:

  unsigned int id;
  std::set<tlp::PropertyInterface*> circularCalls;
};

/// Event class for specific events on Graph
class TLP_SCOPE GraphEvent :public Event {
public:
  // be careful about the ordering of the constants
  // in the enum below because it is used in some assertions
  enum GraphEventType {
    TLP_ADD_NODE = 0,
    TLP_DEL_NODE = 1,
    TLP_ADD_EDGE = 2,
    TLP_DEL_EDGE = 3,
    TLP_REVERSE_EDGE = 4,
    TLP_BEFORE_SET_ENDS = 5,
    TLP_AFTER_SET_ENDS = 6,
    TLP_ADD_NODES = 7,
    TLP_ADD_EDGES = 8,
    TLP_ADD_DESCENDANTGRAPH = 9,
    TLP_DEL_DESCENDANTGRAPH = 10,
    TLP_ADD_SUBGRAPH = 11,
    TLP_DEL_SUBGRAPH = 12,
    TLP_ADD_LOCAL_PROPERTY = 13,
    TLP_BEFORE_DEL_LOCAL_PROPERTY = 14,
    TLP_AFTER_DEL_LOCAL_PROPERTY = 15,
    TLP_ADD_INHERITED_PROPERTY = 16,
    TLP_BEFORE_DEL_INHERITED_PROPERTY = 17,
    TLP_AFTER_DEL_INHERITED_PROPERTY = 18,
    TLP_BEFORE_SET_ATTRIBUTE = 19,
    TLP_AFTER_SET_ATTRIBUTE = 20,
    TLP_REMOVE_ATTRIBUTE = 21
  };

  // constructor for node/edge events
  GraphEvent(const Graph& g, GraphEventType graphEvtType, unsigned int id,
             Event::EventType evtType = Event::TLP_MODIFICATION)
    : Event(g, evtType), evtType(graphEvtType) {
    info.eltId = id;
  }
  // constructor for nodes events
  GraphEvent(const Graph& g, GraphEventType graphEvtType,
             const std::vector<node>& nodes,
             Event::EventType evtType = Event::TLP_MODIFICATION)
    : Event(g, evtType), evtType(graphEvtType) {
    info.nodes = &nodes;
  }
  // constructor for edges events
  GraphEvent(const Graph& g, GraphEventType graphEvtType,
             const std::vector<edge>& edges,
             Event::EventType evtType = Event::TLP_MODIFICATION)
    : Event(g, evtType), evtType(graphEvtType) {
    info.edges = &edges;
  }
  // constructor for subgraph events
  GraphEvent(const Graph& g, GraphEventType graphEvtType,
             const Graph* sg)
    : Event(g, Event::TLP_MODIFICATION), evtType(graphEvtType) {
    info.subGraph = sg;
  }

  // constructor for attribute/property events
  GraphEvent(const Graph& g, GraphEventType graphEvtType,
             const std::string& str,
             Event::EventType evtType = Event::TLP_MODIFICATION)
    : Event(g, evtType), evtType(graphEvtType) {
    info.name = new std::string(str);
  }

  // destructor needed to cleanup name if any
  ~GraphEvent() {
    if (evtType > TLP_DEL_SUBGRAPH)
      delete info.name;
  }

  Graph* getGraph() const {
    return dynamic_cast<Graph *>(sender());
  }

  node getNode() const {
    assert(evtType < TLP_ADD_EDGE);
    return node(info.eltId);
  }

  edge getEdge() const {
    assert(evtType > TLP_DEL_NODE && evtType < TLP_ADD_NODES);
    return edge(info.eltId);
  }

  const std::vector<node>& getNodes() const {
    assert(evtType == TLP_ADD_NODES);
    return *(info.nodes);
  }

  const std::vector<edge>& getEdges() const {
    assert(evtType == TLP_ADD_EDGES);
    return *(info.edges);
  }

  const Graph* getSubGraph() const {
    assert(evtType > TLP_ADD_EDGES && evtType < TLP_ADD_LOCAL_PROPERTY);
    return info.subGraph;
  }

  const std::string& getAttributeName() const {
    assert(evtType > TLP_AFTER_DEL_INHERITED_PROPERTY);
    return *(info.name);
  }

  const std::string& getPropertyName() const {
    assert(evtType > TLP_DEL_SUBGRAPH && evtType < TLP_BEFORE_SET_ATTRIBUTE);
    return *(info.name);
  }

  GraphEventType getType() const {
    return evtType;
  }

protected:
  GraphEventType evtType;
  union {
    unsigned int eltId;
    const Graph* subGraph;
    std::string* name;
    const std::vector<node>* nodes;
    const std::vector<edge>* edges;
  } info;
};

}

///Print the graph (only nodes and edges) in ostream, in the tulip format
TLP_SCOPE std::ostream& operator<< (std::ostream &,const tlp::Graph *);
/*@}*/
//================================================================================
// Specilization of some template class
//================================================================================
#ifndef DOXYGEN_NOTFOR_DEVEL

TLP_BEGIN_HASH_NAMESPACE {
  template <>
  struct TLP_SCOPE hash<const tlp::Graph *> {
    size_t operator()(const tlp::Graph *s) const {return size_t(s->getId());}
  };
  template <>
  struct TLP_SCOPE hash<tlp::Graph *> {
    size_t operator()(tlp::Graph *s) const {return size_t(s->getId());}
  };
} TLP_END_HASH_NAMESPACE

#endif // DOXYGEN_NOTFOR_DEVEL
//include the template code
#include <tulip/PropertyInterface.h>
#include "cxx/Graph.cxx"
#endif

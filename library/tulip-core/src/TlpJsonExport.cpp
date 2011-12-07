#include <tulip/ForEach.h>
#include <tulip/ExportModule.h>
#include <tulip/Graph.h>
#include <tulip/DataSet.h>
#include <tulip/MutableContainer.h>
#include <tulip/MethodFactory.h>
#include <tulip/JsonTokens.h>

#include <sstream>

#include <QDate>
#include <tulip/YajlFacade.h>

using namespace std;
using namespace tlp;

/**
 * @brief Helper class to iterate over the new IDs of nodes and edges from the old IDs.
 *
 * This class takes an iterator over the nodes or edges of a graph, and the MutableContainer used to map from old ID to new ID.
 * It iterates over the nodes/edges and returns the new IDs.
 **/
template<typename TYPE>
class NewValueIterator : public tlp::Iterator<uint> {
public:
  NewValueIterator(Iterator<TYPE>* iterator, const MutableContainer<uint>& newValues) : _iterator(iterator), _newValues(&newValues) {
  }

  ~NewValueIterator() {
    delete _iterator;
  }

  virtual uint next() {
    return _newValues->get(_iterator->next().id);
  }

  virtual bool hasNext() {
    return _iterator->hasNext();
  }

private:
  tlp::Iterator<TYPE>* _iterator;
  const MutableContainer<uint>* _newValues;
};

/**
 * @brief Exports a Tulip Graph to a JSON format.
 *
 * In order to maintain full capabilities of exporting to older format versions, the export of the data is decomposed in two parts:
 * * The metadata
 * * The Graph Hierarchy
 *
 * The metadata is exported by saveMetaData() and the graph hierarchy saved (recursively) by saveGraph().
 *
 * These functions are suffixed by the format version they export to (e.g. saveGraph_V4() as of version 4.0 of the format).
 * Under no circumstances should these functions be modified for anything besides a simple bugfix.
 *
 * Any feature addition should be done by writing a new version of saveMetaData and saveGraph, and switching on the version number in the main function.
 *
 **/
class TlpJsonExport : public ExportModule {
public:
  /**
   * @brief Mandatory constructor to initialize the AlgorithmContext.
   *
   * @param context The context this export algorithm will be initialized with.
   **/
  TlpJsonExport(AlgorithmContext context) : ExportModule(context) {
  }

  virtual bool exportGraph(ostream& fileOut, Graph* graph) {
    //initialization of the maps from old ID to new ID here, before entering saveGraph (as it is recursive).
    node n;
    int i = 0;
    forEach(n, graph->getNodes()) {
      _newNodeId.set(n.id, i++);
    }

    edge e;
    i = 0;
    forEach(e, graph->getEdges()) {
      _newEdgeId.set(e.id, i++);
    }

    _writer.writeMapOpen(); //top-level map

    _writer.writeString("version");
    _writer.writeString("4.0");

    saveMetaData_V4();

    _writer.writeString(GraphToken);
    _writer.writeMapOpen(); //graph hierarchy map
    saveGraph_V4(graph->getRoot());
    _writer.writeMapClose(); // graph hierarchy map

    _writer.writeMapClose(); // top-level map

    fileOut << _writer.generatedString();

    return true;
  }

  /**
   * @brief Saves the metadata of the graph, such as date and comment.
   * This version does not save anything else.
   *
   * @return void
   **/
  void saveMetaData_V4() {
    _writer.writeString("date");
    _writer.writeString(QDate::currentDate().toString(Qt::ISODate).toStdString());

    std::string comment;
    dataSet->get<string>("comment", comment);
    _writer.writeString("comment");
    _writer.writeString(comment);
  }

  /**
   * @brief Saves the graph recursively.
   *
   * @param graph The graph to save.
   * @return void
   **/
  void saveGraph_V4(Graph* graph) {
    node n;
    edge e;

    _writer.writeString(GraphIDToken);
    _writer.writeInteger(graph->getId());

    //we need to save all nodes and edges on the root graph
    if(graph == graph->getRoot()) {
      //saving nodes only requires knowing how many of them there are
      _writer.writeString(NodesNumberToken);
      _writer.writeInteger(graph->numberOfNodes());

      //saving edges requires writing source and target for every edge
      _writer.writeString(EdgesToken);
      _writer.writeArrayOpen();
      forEach(e, graph->getEdges()) {
        uint source = _newNodeId.get(graph->source(e).id);
        uint target = _newNodeId.get(graph->target(e).id);

        _writer.writeArrayOpen();
        _writer.writeInteger(source);
        _writer.writeInteger(target);
        _writer.writeArrayClose();
      }
      _writer.writeArrayClose();
    }
    else {
      //only saviong relevant nodes and edges
      writeInterval(NodesIDsToken, new NewValueIterator<tlp::node>(graph->getNodes(), _newNodeId));
      writeInterval(EdgesIDsToken, new NewValueIterator<tlp::edge>(graph->getEdges(), _newEdgeId));
    }

    _writer.writeString(PropertiesToken);
    _writer.writeMapOpen();
    //saving properties
    PropertyInterface* property;
    forEach(property, graph->getLocalObjectProperties()) {
      _writer.writeString(property->getName());
      _writer.writeMapOpen();

      _writer.writeString(TypeToken);
      _writer.writeString(property->getTypename());

      _writer.writeString(NodeDefaultToken);
      _writer.writeString(property->getNodeDefaultStringValue());

      _writer.writeString(EdgeDefaultToken);
      _writer.writeString(property->getEdgeDefaultStringValue());

      //TODO instead of this hack, add PropertyInterface::hasNonDefaultValuatedNodes and PropertyInterface::hasNonDefaultValuatedEdges
      Iterator<node>* nodeIt = property->getNonDefaultValuatedNodes();
      Iterator<edge>* edgeIt = property->getNonDefaultValuatedEdges();
      bool hasNonDefaultValuatedNodes = nodeIt->hasNext();
      bool hasNonDefaultValuatedEdges = edgeIt->hasNext();
      delete edgeIt;
      delete nodeIt;

      if(hasNonDefaultValuatedNodes) {
        _writer.writeString(NodesValuesToken);
        _writer.writeMapOpen();
        forEach(n, property->getNonDefaultValuatedNodes()) {
          stringstream temp;
          temp << _newNodeId.get(n.id);
          _writer.writeString(temp.str());
          _writer.writeString(property->getNodeStringValue(n));
        }
        _writer.writeMapClose();
      }

      if(hasNonDefaultValuatedEdges) {
        _writer.writeString(EdgesValuesToken);
        _writer.writeMapOpen();
        forEach(e, property->getNonDefaultValuatedEdges()) {
          stringstream temp;
          temp << _newEdgeId.get(e.id);
          _writer.writeString(temp.str());
          _writer.writeString(property->getEdgeStringValue(e));
        }
        _writer.writeMapClose();
      }

      _writer.writeMapClose();
    }
    _writer.writeMapClose();

    _writer.writeString(AttributesToken);
    _writer.writeMapOpen();
    //saving attributes
    DataSet attributes = graph->getAttributes();
    pair<string, DataType*> attribute;
    forEach(attribute, attributes.getValues()) {
      DataTypeSerializer* serializer = DataSet::typenameToSerializer(attribute.second->getTypeName());
      _writer.writeString(attribute.first);
      _writer.writeArrayOpen();
      _writer.writeString(serializer->outputTypeName);

      stringstream temp;
      serializer->writeData(temp, attribute.second);
      _writer.writeString(temp.str());
      _writer.writeArrayClose();
    }
    _writer.writeMapClose();

    //saving subgraphs
    _writer.writeString(SubgraphsToken);
    _writer.writeMapOpen();
    Graph* sub;
    forEach(sub, graph->getSubGraphs()) {
      saveGraph_V4(sub);
    }
    _writer.writeMapClose();
  }

  /**
   * @brief Writes a set of identifiers as contiguous intervals (defined by arrays containing lower and higher bounds).
   * e.g. the set {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 15, 17} will be saved as the array:
   * [ [0, 7], [9, 11], 15, 17]
   *
   * @param intervalName The name of the interval.
   * @param iterator An iterator over the values to save.
   * @return void
   **/
  void writeInterval(const std::string& intervalName, Iterator<uint>* iterator) {
    _writer.writeString(intervalName);
    _writer.writeArrayOpen();
    uint intervalBegin = UINT_MAX;
    uint intervalEnd = UINT_MAX;
    uint previousId = UINT_MAX;
    uint currentId = UINT_MAX;
    forEach(currentId, iterator) {
      //we don't need/want to do all this on the first time we loop
      if(previousId != UINT_MAX) {

        //if the ID are continuous, define an interval, otherwise witre the IDs (either intervals or single)
        if(currentId == previousId + 1) {
          //if we have no interval being defined, set the lower bound to the previous edge ID
          //if an interval is being defined, set its end to the current edge ID
          if(intervalBegin == UINT_MAX) {
            intervalBegin = previousId;
          }

          intervalEnd = currentId;
        }
        else {
          //if an interval is defined, write it
          if(intervalBegin != UINT_MAX) {
            _writer.writeArrayOpen();
            _writer.writeInteger(intervalBegin);
            _writer.writeInteger(intervalEnd);
            _writer.writeArrayClose();
            intervalBegin = UINT_MAX;
            intervalEnd = UINT_MAX;
          }
          else {
            _writer.writeInteger(previousId);
          }
        }

        if(!_it_foreach._it->hasNext()) {
          if(intervalBegin != UINT_MAX) {
            _writer.writeArrayOpen();
            _writer.writeInteger(intervalBegin);
            _writer.writeInteger(intervalEnd);
            _writer.writeArrayClose();
          }
          else {
            _writer.writeInteger(currentId);
          }
        }
      }

      previousId = currentId;
    }
    _writer.writeArrayClose();
  }

protected:
  YajlWriteFacade _writer;
  MutableContainer<uint> _newNodeId;
  MutableContainer<uint> _newEdgeId;
};

EXPORTPLUGIN(TlpJsonExport,"TlpJsonExport","Charles Huet","18/05/2011","Tulip JSon format","1.0")

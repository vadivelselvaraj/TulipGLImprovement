#include "tulip/GraphModel.h"

#include <tulip/Graph.h>
#include <tulip/ForEach.h>
#include <QtGui/QIcon>
#include <tulip/TulipMetaTypes.h>

using namespace tlp;

// Abstract Graph model
GraphModel::GraphModel(QObject *parent): TulipModel(parent), _graph(NULL) {
}

unsigned int GraphModel::elementAt(int row) const {
  return _elements[row];
}

void GraphModel::setGraph(Graph* g) {
  if (_graph != NULL) {
    _graph->removeListener(this);
    std::string s;
    forEach(s, _graph->getProperties()) _graph->getProperty(s)->removeListener(this);
  }

  _graph = g;
  _elements.clear();
  _properties.clear();

  if (_graph != NULL) {
    _graph->addListener(this);
    std::string s;
    forEach(s, _graph->getProperties()) {
      PropertyInterface* pi = _graph->getProperty(s);
      _properties.push_back(pi);
      pi->addListener(this);
    }
  }
}

Graph* GraphModel::graph() const {
  return _graph;
}

int GraphModel::rowCount(const QModelIndex &parent) const {
  if (_graph == NULL || parent.isValid())
    return 0;

  return _elements.size();
}

int GraphModel::columnCount(const QModelIndex &parent) const {
  if (_graph == NULL || parent.isValid())
    return 0;

  return _properties.size();
}

QModelIndex GraphModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}

Qt::ItemFlags GraphModel::flags(const QModelIndex &index) const {
  return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}

QVariant GraphModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (_graph == NULL)
    return QVariant();

  if (orientation == Qt::Vertical) {
    if (section > _elements.size() || section < 0)
      return QVariant();

    if (role == Qt::DisplayRole)
      return _elements[section];
  }
  else {
    if (section > _properties.size() || section < 0)
      return QVariant();
    PropertyInterface* prop = _properties[section];

    if (role == Qt::DisplayRole)
      return QString(prop->getName().c_str());
    else if (role == Qt::DecorationRole && !_graph->existLocalProperty(prop->getName()))
      return QIcon(":/tulip/gui/ui/inherited_properties.png");
  }

  return QVariant();
}

QModelIndex GraphModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row,column,parent))
    return QModelIndex();

  PropertyInterface* prop = _properties[column];
  return createIndex(row,column,prop);
}

QVariant GraphModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole)
    return value(_elements[index.row()],(PropertyInterface*)(index.internalPointer()));
  else if (role == PropertyRole)
    return QVariant::fromValue<PropertyInterface*>((PropertyInterface*)(index.internalPointer()));
  else if (role == GraphRole)
    return QVariant::fromValue<Graph*>(_graph);
  else if (role == IsNodeRole)
    return isNode();
  else if (role == StringRole)
    return stringValue(_elements[index.row()],(PropertyInterface*)(index.internalPointer()));

  return QVariant();
}

bool GraphModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::EditRole)
    return setValue(_elements[index.row()],(PropertyInterface*)(index.internalPointer()),value);

  return QAbstractItemModel::setData(index,value,role);
}

void GraphModel::treatEvent(const Event& ev) {
  if (dynamic_cast<const GraphEvent*>(&ev) != NULL) {
    const GraphEvent* graphEv = static_cast<const GraphEvent*>(&ev);

    if (graphEv->getType() == GraphEvent::TLP_ADD_INHERITED_PROPERTY || graphEv->getType() == GraphEvent::TLP_ADD_LOCAL_PROPERTY) {
      beginInsertColumns(QModelIndex(),columnCount(),columnCount());
      PropertyInterface* prop = _graph->getProperty(graphEv->getPropertyName());
      _properties.push_back(prop);
      prop->addListener(this);
      endInsertColumns();
    }
    else if (graphEv->getType() == GraphEvent::TLP_BEFORE_DEL_INHERITED_PROPERTY || graphEv->getType() == GraphEvent::TLP_BEFORE_DEL_LOCAL_PROPERTY) {
      PropertyInterface* prop = _graph->getProperty(graphEv->getPropertyName());
      int col = _properties.indexOf(prop);
      beginRemoveColumns(QModelIndex(),col,col);
      _properties.remove(col);
      endRemoveColumns();
    }
  }
}

#define STANDARD_NODE_CHECKS(MACRO) \
  MACRO(DoubleProperty,double);\
  MACRO(DoubleVectorProperty,std::vector<double>);\
  MACRO(ColorProperty,tlp::Color);\
  MACRO(ColorVectorProperty,std::vector<tlp::Color>);\
  MACRO(SizeProperty,tlp::Size);\
  MACRO(SizeVectorProperty,std::vector<tlp::Size>);\
  MACRO(StringProperty,std::string);\
  MACRO(StringVectorProperty,std::vector<std::string>);\
  MACRO(LayoutProperty,tlp::Coord);\
  MACRO(CoordVectorProperty,std::vector<tlp::Coord>);\
  MACRO(GraphProperty,tlp::Graph*);\
  MACRO(IntegerProperty,int);\
  MACRO(IntegerVectorProperty,std::vector<int>);\
  MACRO(BooleanProperty,bool);\
  MACRO(BooleanVectorProperty,std::vector<bool>);

#define STANDARD_EDGE_CHECKS(MACRO) \
  MACRO(DoubleProperty,double);\
  MACRO(DoubleVectorProperty,std::vector<double>);\
  MACRO(ColorProperty,tlp::Color);\
  MACRO(ColorVectorProperty,std::vector<tlp::Color>);\
  MACRO(SizeProperty,tlp::Size);\
  MACRO(SizeVectorProperty,std::vector<tlp::Size>);\
  MACRO(StringProperty,std::string);\
  MACRO(StringVectorProperty,std::vector<std::string>);\
  MACRO(LayoutProperty,std::vector<tlp::Coord>);\
  MACRO(CoordVectorProperty,std::vector<tlp::Coord>);\
  MACRO(IntegerProperty,int);\
  MACRO(IntegerVectorProperty,std::vector<int>);\
  MACRO(BooleanProperty,bool);\
  MACRO(BooleanVectorProperty,std::vector<bool>);

#define GET_NODE_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) return QVariant::fromValue< TYPE >(static_cast<PROP*>(prop)->getNodeValue(n))
QVariant GraphModel::nodeValue(unsigned int id, PropertyInterface * prop) {
  node n(id);
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    return QVariant::fromValue<NodeShape>(static_cast<IntegerProperty*>(prop)->getNodeValue(n));
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    return QVariant::fromValue<TulipFont>(TulipFont::fromFile(static_cast<StringProperty*>(prop)->getNodeValue(n).c_str()));
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    return QVariant::fromValue<LabelPosition>((LabelPosition)(static_cast<IntegerProperty*>(prop)->getNodeValue(n)));
  STANDARD_NODE_CHECKS(GET_NODE_VALUE)
  return QVariant();
}
#define GET_NODE_DEFAULT_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) return QVariant::fromValue< TYPE >(static_cast<PROP*>(prop)->getNodeDefaultValue())
QVariant GraphModel::nodeDefaultValue(PropertyInterface * prop) {
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    return QVariant::fromValue<EdgeShape>((EdgeShape)(static_cast<IntegerProperty*>(prop)->getNodeDefaultValue()));
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    return QVariant::fromValue<TulipFont>(TulipFont::fromFile(static_cast<StringProperty*>(prop)->getNodeDefaultValue().c_str()));
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    return QVariant::fromValue<LabelPosition>((LabelPosition)(static_cast<IntegerProperty*>(prop)->getNodeDefaultValue()));
  STANDARD_NODE_CHECKS(GET_NODE_DEFAULT_VALUE)
  return QVariant();
}
#define SET_ALL_NODE_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) static_cast<PROP*>(prop)->setAllNodeValue(v.value< TYPE >())
bool GraphModel::setAllNodeValue(PropertyInterface * prop, QVariant v) {
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    static_cast<IntegerProperty*>(prop)->setAllNodeValue(v.value<NodeShape>().nodeShapeId);
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    static_cast<StringProperty*>(prop)->setAllNodeValue(v.value<TulipFont>().fontFile().toStdString());
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    static_cast<IntegerProperty*>(prop)->setAllNodeValue(v.value<LabelPosition>());
  STANDARD_NODE_CHECKS(SET_ALL_NODE_VALUE)
  else
    return false;
  return true;
}
#define SET_NODE_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) static_cast<PROP*>(prop)->setNodeValue(n,v.value< TYPE >())
bool GraphModel::setNodeValue(unsigned int id, PropertyInterface * prop, QVariant v) {
  node n(id);
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    static_cast<IntegerProperty*>(prop)->setNodeValue(n,v.value<NodeShape>().nodeShapeId);
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    static_cast<StringProperty*>(prop)->setNodeValue(n,v.value<TulipFont>().fontFile().toStdString());
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    static_cast<IntegerProperty*>(prop)->setNodeValue(n,v.value<LabelPosition>());
  STANDARD_NODE_CHECKS(SET_NODE_VALUE)
  else
    return false;
  return true;
}

#define GET_EDGE_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) return QVariant::fromValue< TYPE >(static_cast<PROP*>(prop)->getEdgeValue(e))
QVariant GraphModel::edgeValue(unsigned int id, PropertyInterface * prop) {
  edge e(id);
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    return QVariant::fromValue<EdgeShape>((EdgeShape)(static_cast<IntegerProperty*>(prop)->getEdgeValue(e)));
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    return QVariant::fromValue<TulipFont>(TulipFont::fromFile(static_cast<StringProperty*>(prop)->getEdgeValue(e).c_str()));
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    return QVariant::fromValue<LabelPosition>((LabelPosition)(static_cast<IntegerProperty*>(prop)->getEdgeValue(e)));
  STANDARD_EDGE_CHECKS(GET_EDGE_VALUE)
  return QVariant();
}
#define GET_EDGE_DEFAULT_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) return QVariant::fromValue< TYPE >(static_cast<PROP*>(prop)->getEdgeDefaultValue())
QVariant GraphModel::edgeDefaultValue(PropertyInterface * prop) {
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    return QVariant::fromValue<EdgeShape>((EdgeShape)(static_cast<IntegerProperty*>(prop)->getEdgeDefaultValue()));
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    return QVariant::fromValue<TulipFont>(TulipFont::fromFile(static_cast<StringProperty*>(prop)->getEdgeDefaultValue().c_str()));
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    return QVariant::fromValue<LabelPosition>((LabelPosition)(static_cast<IntegerProperty*>(prop)->getEdgeDefaultValue()));
  STANDARD_EDGE_CHECKS(GET_EDGE_DEFAULT_VALUE)
  return QVariant();
}
#define SET_EDGE_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) static_cast<PROP*>(prop)->setEdgeValue(e,v.value< TYPE >())
bool GraphModel::setEdgeValue(unsigned int id, PropertyInterface* prop, QVariant v) {
  edge e(id);
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    static_cast<IntegerProperty*>(prop)->setEdgeValue(e,v.value<EdgeShape>());
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    static_cast<StringProperty*>(prop)->setEdgeValue(e,v.value<TulipFont>().fontFile().toStdString());
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    static_cast<IntegerProperty*>(prop)->setEdgeValue(e,v.value<LabelPosition>());
  STANDARD_EDGE_CHECKS(SET_EDGE_VALUE)
  else
    return false;
  return true;
}
#define SET_ALL_EDGE_VALUE(PROP,TYPE) else if (dynamic_cast<PROP*>(prop) != NULL) static_cast<PROP*>(prop)->setAllEdgeValue(v.value< TYPE >())
bool GraphModel::setAllEdgeValue(PropertyInterface* prop, QVariant v) {
  if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewShape")
    static_cast<IntegerProperty*>(prop)->setAllEdgeValue(v.value<EdgeShape>());
  else if (dynamic_cast<StringProperty*>(prop) != NULL && prop->getName() == "viewFont")
    static_cast<StringProperty*>(prop)->setAllEdgeValue(v.value<TulipFont>().fontFile().toStdString());
  else if (dynamic_cast<IntegerProperty*>(prop) != NULL && prop->getName() == "viewLabelPosition")
    static_cast<IntegerProperty*>(prop)->setAllEdgeValue(v.value<LabelPosition>());
  STANDARD_EDGE_CHECKS(SET_ALL_EDGE_VALUE)
  else
    return false;
  return true;
}

// Nodes model
NodesGraphModel::NodesGraphModel(QObject *parent): GraphModel(parent) {
}

bool NodesGraphModel::lessThan(unsigned int a, unsigned int b, PropertyInterface * prop) const {
  return prop->compare(node(a),node(b)) <= -1;
}

void NodesGraphModel::treatEvent(const Event& ev) {
  GraphModel::treatEvent(ev);

  if (dynamic_cast<const GraphEvent*>(&ev) != NULL) {
    const GraphEvent* graphEv = static_cast<const GraphEvent*>(&ev);

    if (graphEv->getType() == GraphEvent::TLP_ADD_NODE) {
      beginInsertRows(QModelIndex(),_elements.size(),_elements.size());
      _elements.push_back(graphEv->getNode().id);
      endInsertRows();
    }
    else if (graphEv->getType() == GraphEvent::TLP_ADD_NODES) {
      beginInsertRows(QModelIndex(),_elements.size(),_elements.size()+graphEv->getNodes().size()-1);

      for(std::vector<tlp::node>::const_iterator it = graphEv->getNodes().begin(); it != graphEv->getNodes().end(); ++it)
        _elements.push_back(it->id);

      endInsertRows();
    }
    else if (graphEv->getType() == GraphEvent::TLP_DEL_NODE) {
      int index = _elements.indexOf(graphEv->getNode().id);
      beginRemoveRows(QModelIndex(),index,index);
      _elements.remove(index);
      endRemoveRows();
    }
  }

  else if (dynamic_cast<const PropertyEvent*>(&ev) != NULL) {
    const PropertyEvent* propEv = static_cast<const PropertyEvent*>(&ev);
    int col=_properties.indexOf(propEv->getProperty());

    if (propEv->getType() == PropertyEvent::TLP_AFTER_SET_NODE_VALUE) {
      int row = _elements.indexOf(propEv->getNode().id);
      emit dataChanged(index(row,col),index(row,col));
    }
    else if (propEv->getType() == PropertyEvent::TLP_AFTER_SET_ALL_NODE_VALUE) {
      emit dataChanged(index(0,col),index(_elements.size()-1,col));
    }
  }
}

void NodesGraphModel::setGraph(Graph* g) {
  GraphModel::setGraph(g);
  if (graph() == NULL)
    return;

  _elements.resize(graph()->numberOfNodes());
  int i=0;
  node n;
  forEach(n,graph()->getNodes())
  _elements[i++] = n.id;
  reset();
}

QString NodesGraphModel::stringValue(unsigned int id, PropertyInterface* pi) const {
  return pi->getNodeStringValue(node(id)).c_str();
}

QVariant NodesGraphModel::value(unsigned int id, PropertyInterface* prop) const {
  return nodeValue(id,prop);
}

bool NodesGraphModel::setValue(unsigned int id, PropertyInterface* prop, QVariant v) const {
  return setNodeValue(id,prop,v);
}

// Edges model
EdgesGraphModel::EdgesGraphModel(QObject *parent): GraphModel(parent) {
}
QString EdgesGraphModel::stringValue(unsigned int id, PropertyInterface* pi) const {
  return pi->getEdgeStringValue(edge(id)).c_str();
}

void EdgesGraphModel::treatEvent(const Event& ev) {
  GraphModel::treatEvent(ev);

  if (dynamic_cast<const GraphEvent*>(&ev) != NULL) {
    const GraphEvent* graphEv = static_cast<const GraphEvent*>(&ev);

    if (graphEv->getType() == GraphEvent::TLP_ADD_EDGE) {
      beginInsertRows(QModelIndex(),_elements.size(),_elements.size());
      _elements.push_back(graphEv->getEdge().id);
      endInsertRows();
    }
    else if (graphEv->getType() == GraphEvent::TLP_ADD_EDGES) {
      beginInsertRows(QModelIndex(),_elements.size(),_elements.size()+graphEv->getEdges().size()-1);

      for(std::vector<tlp::edge>::const_iterator it = graphEv->getEdges().begin(); it != graphEv->getEdges().end(); ++it)
        _elements.push_back(it->id);

      endInsertRows();
    }
    else if (graphEv->getType() == GraphEvent::TLP_DEL_EDGE) {
      int index = _elements.indexOf(graphEv->getEdge().id);
      beginRemoveRows(QModelIndex(),index,index);
      _elements.remove(index);
      endRemoveRows();
    }
  }

  else if (dynamic_cast<const PropertyEvent*>(&ev) != NULL) {
    const PropertyEvent* propEv = static_cast<const PropertyEvent*>(&ev);
    int col=_properties.indexOf(propEv->getProperty());

    if (propEv->getType() == PropertyEvent::TLP_AFTER_SET_EDGE_VALUE) {
      int row = _elements.indexOf(propEv->getEdge().id);
      emit dataChanged(index(row,col),index(row,col));
    }
    else if (propEv->getType() == PropertyEvent::TLP_AFTER_SET_ALL_EDGE_VALUE) {
      emit dataChanged(index(0,col),index(_elements.size()-1,col));
    }
  }
}

void EdgesGraphModel::setGraph(Graph* g) {
  GraphModel::setGraph(g);

  if (graph() == NULL)
    return;

  _elements.resize(graph()->numberOfEdges());
  int i=0;
  edge e;
  forEach(e,graph()->getEdges())
  _elements[i++] = e.id;
  reset();
}

QVariant EdgesGraphModel::value(unsigned int id, PropertyInterface* prop) const {
  return edgeValue(id,prop);
}
bool EdgesGraphModel::setValue(unsigned int id, PropertyInterface* prop, QVariant v) const {
  return setEdgeValue(id,prop,v);
}

bool EdgesGraphModel::lessThan(unsigned int a, unsigned int b, PropertyInterface * prop) const {
  return prop->compare(edge(a),edge(b)) <= -1;
}

// Filter proxy
GraphSortFilterProxyModel::GraphSortFilterProxyModel(QObject *parent): QSortFilterProxyModel(parent), _properties(QVector<PropertyInterface*>()) {
}

bool GraphSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
  GraphModel* graphModel = static_cast<GraphModel*>(sourceModel());
  return graphModel->lessThan(graphModel->elementAt(left.row()),graphModel->elementAt(right.row()),static_cast<PropertyInterface*>(left.internalPointer()));
}
void GraphSortFilterProxyModel::setProperties(QVector<PropertyInterface *> properties) {
  _properties = properties;
}
bool GraphSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex&) const {
  if (filterRegExp().isEmpty())
    return true;

  GraphModel* graphModel = static_cast<GraphModel*>(sourceModel());

  if (graphModel->graph() == NULL)
    return true;

  unsigned int id = graphModel->elementAt(sourceRow);

  if (_properties.isEmpty()) {
    std::string s;
    forEach(s,graphModel->graph()->getProperties()) {
      if (graphModel->stringValue(id,graphModel->graph()->getProperty(s)).contains(filterRegExp()))
        return true;
    }
  }
  else {
    foreach(PropertyInterface* pi, _properties) {
      if (graphModel->stringValue(id,pi).contains(filterRegExp()))
        return true;
    }
  }

  return false;
}

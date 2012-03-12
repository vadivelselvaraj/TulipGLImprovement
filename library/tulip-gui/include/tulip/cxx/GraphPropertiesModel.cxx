#include <tulip/Graph.h>

namespace tlp {

template<typename PROPTYPE>
GraphPropertiesModel<PROPTYPE>::GraphPropertiesModel(tlp::Graph* graph, QObject *parent): tlp::TulipModel(parent), _graph(graph), _placeholder(QString::null) {
  _graph->addListener(this);
}

template<typename PROPTYPE>
GraphPropertiesModel<PROPTYPE>::GraphPropertiesModel(QString placeholder, tlp::Graph* graph, QObject *parent): tlp::TulipModel(parent), _graph(graph), _placeholder(placeholder) {
  _graph->addListener(this);
}

template<typename PROPTYPE>
QModelIndex GraphPropertiesModel<PROPTYPE>::index(int row, int column,const QModelIndex &parent) const {
  if (!hasIndex(row,column,parent))
    return QModelIndex();

  int i=0;
  if (!_placeholder.isNull()) {
    i=1;
    if (row == 0)
      return createIndex(row,column);
  }
  std::string propName;

  forEach(propName,_graph->getInheritedProperties()) {
    if (dynamic_cast<PROPTYPE*>(_graph->getProperty(propName)) == NULL)
      continue;

    if (i++ == row)
      return createIndex(row,column,_graph->getProperty(propName));
  }
  forEach(propName,_graph->getLocalProperties()) {
    if (dynamic_cast<PROPTYPE*>(_graph->getProperty(propName)) == NULL)
      continue;

    if (i++ == row)
      return createIndex(row,column,_graph->getProperty(propName));
  }
  return QModelIndex();
}

template<typename PROPTYPE>
QModelIndex GraphPropertiesModel<PROPTYPE>::parent(const QModelIndex &) const {
  return QModelIndex();
}

template<typename PROPTYPE>
int GraphPropertiesModel<PROPTYPE>::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;

  int result = 0;
  if (!_placeholder.isNull())
    result = 1;
  std::string propName;
  forEach(propName, _graph->getProperties()) {
    if (dynamic_cast<PROPTYPE*>(_graph->getProperty(propName)) == NULL)
      continue;

    result++;
  }
  return result;
}

template<typename PROPTYPE>
int GraphPropertiesModel<PROPTYPE>::columnCount(const QModelIndex &) const {
  return 1;
}

template<typename PROPTYPE>
QVariant GraphPropertiesModel<PROPTYPE>::data(const QModelIndex &index, int role) const {
  PropertyInterface* pi = NULL;
  if (index.internalPointer() != NULL)
    pi = (PropertyInterface*)(index.internalPointer());

  if (role == Qt::DisplayRole) {
    if (!_placeholder.isNull() && index.row() == 0)
      return _placeholder;
    return QString(pi->getName().c_str());
  }
  else if (role == Qt::ToolTipRole) {
    if (!_placeholder.isNull() && index.row() == 0)
      return _placeholder;
    return (_graph->existLocalProperty(pi->getName()) ? trUtf8("Local") : trUtf8("Inherited"));
  }
  else if (role == Qt::FontRole) {
    QFont f;
    if (!_placeholder.isNull() && index.row() == 0) {
      f.setItalic(true);
    }
    else
      f.setBold(!_graph->existLocalProperty(pi->getName()));
    return f;
  }
  else if (role == PropertyRole) {
    return QVariant::fromValue<PropertyInterface*>(pi);
  }
  return QVariant();
}

template<typename PROPTYPE>
int GraphPropertiesModel<PROPTYPE>::rowOf(PropertyInterface* pi) const {
  for (int i=0; i<rowCount(); ++i) {
    QVariant v = data(index(i,0),PropertyRole);

    if (v.value<PropertyInterface*>() == pi)
      return i;
  }

  return 0;
}

}

#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

#include <QtCore/QVector>
#include <QtGui/QSortFilterProxyModel>
#include <tulip/Observable.h>
#include <tulip/TulipModel.h>

namespace tlp {
class Graph;
class PropertyInterface;

class TLP_QT_SCOPE GraphModel : public tlp::TulipModel, public Observable {
  tlp::Graph* _graph;
public:
  explicit GraphModel(QObject *parent = 0);

  virtual void setGraph(tlp::Graph*);
  tlp::Graph* graph() const;

  // Methods re-implemented from QAbstractItemModel
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &child) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);

  void treatEvent(const tlp::Event&);

  unsigned int elementAt(int) const;
  virtual bool lessThan(unsigned int,unsigned int,tlp::PropertyInterface*) const = 0;

protected:
  QVector<unsigned int> _elements;
  QVector<tlp::PropertyInterface*> _properties;

  virtual bool setValue(unsigned int,tlp::PropertyInterface*,QVariant) const = 0;
  virtual QVariant value(unsigned int,tlp::PropertyInterface*) const = 0;
  virtual QString stringValue(unsigned int,tlp::PropertyInterface*) const = 0;
  virtual bool isNode() const = 0;
};

class GraphSortFilterProxyModel: public QSortFilterProxyModel {
public:
  GraphSortFilterProxyModel(QObject* parent = 0);
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class TLP_QT_SCOPE NodesGraphModel: public GraphModel {
public:
  NodesGraphModel(QObject* parent = 0);
  void setGraph(Graph *);

  void treatEvent(const tlp::Event &);
  virtual bool lessThan(unsigned int,unsigned int,tlp::PropertyInterface*) const;

protected:
  virtual bool setValue(unsigned int,tlp::PropertyInterface*,QVariant) const;
  virtual QVariant value(unsigned int,tlp::PropertyInterface*) const;
  virtual QString stringValue(unsigned int,tlp::PropertyInterface*) const;
  bool isNode() const {
    return true;
  }
};

class TLP_QT_SCOPE EdgesGraphModel: public GraphModel {
public:
  EdgesGraphModel(QObject* parent = 0);
  void setGraph(Graph *);
  void treatEvent(const tlp::Event &);
  virtual bool lessThan(unsigned int,unsigned int,tlp::PropertyInterface*) const;

protected:
  virtual bool setValue(unsigned int,tlp::PropertyInterface*,QVariant) const;
  virtual QVariant value(unsigned int,tlp::PropertyInterface*) const;
  virtual QString stringValue(unsigned int,tlp::PropertyInterface*) const;
  bool isNode() const {
    return false;
  }
};

}

#endif // GRAPHMODEL_H

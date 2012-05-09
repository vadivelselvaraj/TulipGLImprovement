#ifndef PROPERTIESEDITOR_H
#define PROPERTIESEDITOR_H

#include <QtGui/QWidget>
#include <QtCore/QModelIndex>

#include <tulip/GraphPropertiesModel.h>
#include <tulip/PropertyInterface.h>

namespace Ui {
class PropertiesEditor;
}

namespace tlp {
class Graph;
class TulipItemDelegate;
}

class PropertiesEditor : public QWidget {
  Q_OBJECT

  Ui::PropertiesEditor* _ui;
  tlp::PropertyInterface* _contextProperty;
  tlp::Graph* _graph;
  tlp::TulipItemDelegate* _delegate;
  tlp::GraphPropertiesModel<tlp::PropertyInterface>* _sourceModel;

public:
  explicit PropertiesEditor(QWidget *parent = 0);
  virtual ~PropertiesEditor();

  void setGraph(tlp::Graph* g);

signals:
  void propertyVisibilityChanged(tlp::PropertyInterface*,bool);

protected slots:
  void checkStateChanged(QModelIndex,Qt::CheckState);
  void showCustomContextMenu(const QPoint&);
  void setAllNodes();
  void setAllEdges();
  void copyProperty();
  void delProperty();
  void toLabels();
  void checkAll();
  void unCheckAll();
  void unCheckAllExcept();
};

#endif // PROPERTIESEDITOR_H
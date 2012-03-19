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
#ifndef GRAPHPERSPECTIVE_H
#define GRAPHPERSPECTIVE_H

#include <QtCore/QModelIndex>
#include <QtCore/QPoint>
#include <tulip/Perspective.h>
#include <tulip/MethodFactory.h>

class QAction;
class QHeaderView;

class GraphHierarchiesEditor;

namespace tlp {
class GraphHierarchiesModel;
class View;
}

namespace Ui {
class GraphPerspectiveMainWindowData;
}

class GraphPerspective : public tlp::Perspective {
  Q_OBJECT
  Ui::GraphPerspectiveMainWindowData *_ui;
  tlp::GraphHierarchiesModel *_graphs;
  bool _maximised;

  static QSet<QString> RESERVED_PROPERTIES;
  static void reserveDefaultProperties();

public:
  static bool isReservedPropertyName(QString);
  static void registerReservedProperty(QString);

  PLUGININFORMATIONS("Graph hierarchy analysis", "Ludwig Fiolka", "2011/07/11", "Analyze several graphs/subgraphs hierarchies", "1.0", "Hierarchy")
  std::string icon() const {
    return ":/tulip/graphperspective/icons/32/desktop.png";
  }

  GraphPerspective(const tlp::PluginContext* c);
  virtual void construct(tlp::PluginProgress *);
  tlp::GraphHierarchiesModel* model() const;

public slots:
  void showFullScreen(bool);
  void importGraph();
  void createPanel(tlp::Graph* g = NULL);
  void save();
  void saveAs(const QString& path=QString::null);
  void open(const QString& path=QString::null);
  void centerPanels(tlp::PropertyInterface*);

protected slots:
  void currentGraphChanged(tlp::Graph* graph);
  void refreshDockExpandControls();
  void panelFocused(tlp::View*);
  void focusedPanelGraphSet(tlp::Graph*);
  void modeSwitch();
  void deleteSelectedElements();
  void invertSelection();
  void cancelSelection();
  void selectAll();
  void undo();
  void redo();
  void cut();
  void paste();
  void copy();
  void group();
  void createSubGraph();
  void CSVImport();

  void setDatasetGraph(tlp::Graph*);
  void setDatasetFilter();
  void setDatasetFilterProperty();
  void datasetHeaderContextMenuRequested(QPoint);
  void datasetHeaderSectionToggled(bool);
  void datasetHeaderShowAll();
  void datasetHeaderHideAll();
  void datasetHeaderSetAllVisible(QHeaderView*,bool);
};

#endif // GRAPHPERSPECTIVE_H

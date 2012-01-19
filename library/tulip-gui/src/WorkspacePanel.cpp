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
#include "tulip/WorkspacePanel.h"

#include <QtCore/QPropertyAnimation>
#include <QtGui/QGraphicsView>
#include <QtGui/QCloseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QApplication>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QTabWidget>

#include <tulip/TulipMetaTypes.h>
#include <tulip/ProcessingAnimationItem.h>
#include <tulip/Interactor.h>
#include <tulip/ForEach.h>
#include <tulip/View.h>
#include <tulip/Graph.h>
#include <tulip/GraphHierarchiesModel.h>
#include "ui_WorkspacePanel.h"

using namespace tlp;

// helper class
class ProgressItem: public QGraphicsObject {
  ProcessingAnimationItem* _animation;
public:
  ProgressItem(QGraphicsScene* parentScene): QGraphicsObject() {
    _animation = new ProcessingAnimationItem(QPixmap(":/tulip/gui/ui/process-working.png"),QSize(64,64));
    _animation->setParentItem(this);
    parentScene->addItem(_animation);
  }

  virtual ~ProgressItem() {
    delete _animation;
  }

  QRectF boundingRect() const {
    return QRectF();
  }

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setPen(QColor(255,255,255));
    painter->setBrush(QColor(0,0,0,70));
    painter->drawRect(scene()->sceneRect());
    _animation->setOpacity(opacity());
    _animation->setPos(scene()->width()/2-16,scene()->height()/2-16);
  }
};

// ========================

WorkspacePanel::WorkspacePanel(tlp::View* view, const QString& viewName, QWidget *parent)
  : QWidget(parent), _ui(new Ui::WorkspacePanel), _view(NULL), _viewName(viewName), _progressItem(NULL), _currentInteractorConfigurationItem(NULL), _viewConfigurationWidgets(NULL), _viewConfigurationExpanded(false) {
  _ui->setupUi(this);
  connect(_ui->closeButton,SIGNAL(clicked()),this,SLOT(close()));
  setView(view,viewName);
  setAttribute(Qt::WA_DeleteOnClose);
}

WorkspacePanel::~WorkspacePanel() {
  delete _ui;

  if (_view != NULL) {
    disconnect(_view,SIGNAL(destroyed()),this,SLOT(viewDestroyed()));
    delete _view->graphicsView();
  }

  delete _view;
}
void WorkspacePanel::viewDestroyed() {
  _view = NULL;
  deleteLater();
}

View* WorkspacePanel::view() const {
  return _view;
}

QString WorkspacePanel::viewName() const {
  return _viewName;
}

void WorkspacePanel::setView(tlp::View* view, const QString& viewName) {
  assert(view != NULL);
  _ui->currentInteractorButton->setChecked(false);

  if (_view != NULL) {
    disconnect(_view,SIGNAL(destroyed()),this,SLOT(viewDestroyed()));
    disconnect(_view,SIGNAL(graphSet(tlp::Graph*)),this,SLOT(viewGraphSet(tlp::Graph*)));
    disconnect(_view,SIGNAL(drawNeeded()),this,SIGNAL(drawNeeded()));
  }

  if (_view)
    delete _view->graphicsView();

  delete _view;

  _view = view;
  _viewName = viewName;

  QList<Interactor*> compatibleInteractors;
  QList<std::string> interactorNames = InteractorLister::compatibleInteractors(viewName.toStdString());
  foreach(std::string name,interactorNames) {
    compatibleInteractors << InteractorLister::getPluginObject(name,NULL);
  }
  _view->setInteractors(compatibleInteractors);
  _view->graphicsView()->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  layout()->addWidget(_view->graphicsView());
  refreshInteractorsToolbar();

  if (compatibleInteractors.size()>0)
    setCurrentInteractor(compatibleInteractors[0]);

  connect(_view,SIGNAL(destroyed()),this,SLOT(viewDestroyed()));
  connect(_view,SIGNAL(graphSet(tlp::Graph*)),this,SLOT(viewGraphSet(tlp::Graph*)));
  connect(_view,SIGNAL(drawNeeded()),this,SIGNAL(drawNeeded()));

  if (_view->configurationWidgets().size()==0)
    return;

  QTabWidget* viewConfigurationTabs = new QTabWidget();
  viewConfigurationTabs->setTabPosition(QTabWidget::West);
  viewConfigurationTabs->setStyleSheet(QString() +

                                       "QTabWidget {\n"
                                       + "  background-color: transparent;\n"
                                       + "}\n"

                                       + "QTabWidget::pane {\n"
                                       + "  background-color: white;\n"
                                       + "  border: 1px solid #C9C9C9;\n"
                                       + "}\n"

                                       + "QTabBar::tab {\n"
                                       + "  border-image:none;\n"
                                       + "  border: 1px solid #C9C9C9;\n"
                                       + "  border-right: 0px;\n"
                                       + "  border-top-left-radius: 4px;\n"
                                       + "  border-bottom-left-radius: 4px;\n"
                                       + "  min-height: 8ex;\n"
                                       + "  padding: 2px;\n"
                                       + "  font: bold;\n"
                                       + "}\n"

                                       + "QTabBar::tab:!selected {\n"
                                       + "  margin-left: 1px;\n"
                                       + "}\n"

                                       + "QTabBar::tab:!selected {\n"
                                       + "  background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,\n"
                                       + "  stop: 0 #ffffff,\n"
                                       + "  stop: 0.4 #eeeeee,\n"
                                       + "  stop: 0.401 #e2e2e2,\n"
                                       + "  stop: 1 #dddddd);\n"
                                       + "  color: black;\n"
                                       + "}\n"

                                       + "QTabBar::tab:!selected:hover {\n"
                                       + "  background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,\n"
                                       + "  stop: 0 #ffffff,\n"
                                       + "  stop: 0.4 #fefefe,\n"
                                       + "  stop: 0.401 #f2f2f2,\n"
                                       + "  stop: 1 #ededed);\n"
                                       + "}\n"

                                       + "QTabBar::tab:selected {\n"
                                       + "  background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,\n"
                                       + "  stop: 0 #838383,\n"
                                       + "  stop: 0.4 #707070,\n"
                                       + "  stop: 0.401 #636363,\n"
                                       + "  stop: 1 #4a4a4a);\n"
                                       + "  color: white;\n"
                                       + "}\n"

                                       + "QTabBar::tab:selected:hover {\n"
                                       + "  background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,\n"
                                       + "  stop: 0 #939393,\n"
                                       + "  stop: 0.4 #808080,\n"
                                       + "  stop: 0.401 #737373,\n"
                                       + "  stop: 1 #5a5a5a);\n"
                                       + "}");

  foreach(QWidget* w, _view->configurationWidgets()) {
    w->resize(w->width(),w->sizeHint().height());
    viewConfigurationTabs->addTab(w,w->windowTitle());
  }

  _viewConfigurationWidgets = new QGraphicsProxyWidget(_view->centralItem());
  _viewConfigurationWidgets->setWidget(viewConfigurationTabs);

  _view->graphicsView()->scene()->installEventFilter(this);
}

void WorkspacePanel::setCurrentInteractor(tlp::Interactor *i) {
  assert(i);
  view()->setCurrentInteractor(i);
  _ui->currentInteractorButton->setText(i->action()->text());
  _ui->currentInteractorButton->setIcon(i->action()->icon());
  _ui->currentInteractorButton->setChecked(false);
}

void WorkspacePanel::setCurrentInteractorConfigurationVisible(bool toggle) {
  if (_currentInteractorConfigurationItem != NULL) {
    static_cast<QScrollArea*>(_currentInteractorConfigurationItem->widget())->takeWidget();
    delete _currentInteractorConfigurationItem;
    _currentInteractorConfigurationItem = NULL;
  }

  if (!toggle || _view->currentInteractor() == NULL || _view->currentInteractor()->configurationWidget() == NULL)
    return;

  _currentInteractorConfigurationItem = new QGraphicsProxyWidget();
  _currentInteractorConfigurationItem->setParent(_view->graphicsView());
  _currentInteractorConfigurationItem->setObjectName("currentInteractorConfigurationItem");
  _currentInteractorConfigurationItem->setOpacity(0);
  _currentInteractorConfigurationItem->setPos(0,0);
  QScrollArea* area = new QScrollArea();
  area->setFrameShape(QScrollArea::NoFrame);
  QWidget* interactorWidget = _view->currentInteractor()->configurationWidget();
  interactorWidget->setObjectName("contents");
  area->setStyleSheet("#contents { background-color: white; border: 1px solid #C9C9C9; }");
  area->setWidget(interactorWidget);
  _currentInteractorConfigurationItem->setWidget(area);
  _view->graphicsView()->scene()->addItem(_currentInteractorConfigurationItem);
  QPropertyAnimation* anim = new QPropertyAnimation(_currentInteractorConfigurationItem,"opacity",_currentInteractorConfigurationItem);
  anim->setStartValue(0);
  anim->setEndValue(0.85);
  anim->setDuration(300);
  anim->setEasingCurve(QEasingCurve::OutQuad);
  anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void WorkspacePanel::interactorActionTriggered() {
  QAction* action = static_cast<QAction*>(sender());
  Interactor* interactor = static_cast<Interactor*>(action->parent());

  if (interactor == view()->currentInteractor())
    return;

  setCurrentInteractor(interactor);
}

bool WorkspacePanel::isProgressMode() const {
  return _progressItem != NULL;
}

void WorkspacePanel::toggleProgressMode(bool p) {
  assert(view() && view()->graphicsView() && view()->graphicsView()->scene());

  if (p && _progressItem == NULL)  {
    _progressItem = new ProgressItem(_view->graphicsView()->scene());
    _view->graphicsView()->scene()->addItem(_progressItem);
    QPropertyAnimation* progressFadeIn = new QPropertyAnimation(_progressItem,"opacity",_progressItem);
    progressFadeIn->setStartValue(0);
    progressFadeIn->setEndValue(1);
    progressFadeIn->setDuration(800);
    progressFadeIn->start(QAbstractAnimation::DeleteWhenStopped);
  }
  else if (!p && _progressItem != NULL) {
    _view->graphicsView()->scene()->removeItem(_progressItem);
    delete _progressItem;
    _progressItem = NULL;
  }
}

void WorkspacePanel::progress_handler(int,int) {
  if (!isProgressMode())
    toggleProgressMode(true);

  QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void WorkspacePanel::refreshInteractorsToolbar() {
  _actionTriggers.clear();
  QList<Interactor*> compatibleInteractors = _view->interactors();
  delete _ui->interactorsFrame->layout();
  bool interactorsUiShown = compatibleInteractors.size() > 0;
  _ui->currentInteractorButton->setVisible(interactorsUiShown);
  _ui->interactorsFrame->setVisible(interactorsUiShown);
  _ui->sep1->setVisible(interactorsUiShown);
  _ui->sep2->setVisible(interactorsUiShown);

  if (interactorsUiShown) {
    QHBoxLayout* interactorsLayout = new QHBoxLayout;
    interactorsLayout->setContentsMargins(0,0,0,0);
    interactorsLayout->setSpacing(4);
    foreach(Interactor* i, compatibleInteractors) {
      QPushButton* button = new QPushButton();
      button->setMinimumSize(22,22);
      button->setMaximumSize(22,22);
      button->setIcon(i->action()->icon());
      button->setToolTip(i->action()->text());
      interactorsLayout->addWidget(button);
      connect(button,SIGNAL(clicked()),i->action(),SLOT(trigger()));
      connect(i->action(),SIGNAL(triggered()),this,SLOT(interactorActionTriggered()));
      connect(i->action(),SIGNAL(changed()),this,SLOT(actionChanged()));
      _actionTriggers[i->action()] = button;
    }
    _ui->interactorsFrame->setLayout(interactorsLayout);
    setCurrentInteractor(compatibleInteractors[0]);
  }
}

void WorkspacePanel::actionChanged() {
  QAction* action = static_cast<QAction*>(sender());
  if (!_actionTriggers.contains(action))
    return;
  _actionTriggers[action]->setEnabled(action->isEnabled());
}

void WorkspacePanel::setGraphsModel(tlp::GraphHierarchiesModel* model) {
  _ui->graphCombo->setModel(model);
  connect(_ui->graphCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(graphComboIndexChanged()));
}

void WorkspacePanel::viewGraphSet(tlp::Graph* g) {
  if (_ui->graphCombo->model() == NULL)
    return;

  tlp::GraphHierarchiesModel* model = static_cast<tlp::GraphHierarchiesModel*>(_ui->graphCombo->model());
  QModelIndex graphIndex = model->indexOf(g);

  if (graphIndex == _ui->graphCombo->selectedIndex())
    return;

  _ui->graphCombo->selectIndex(graphIndex);
}

void WorkspacePanel::graphComboIndexChanged() {
  tlp::Graph* g = _ui->graphCombo->model()->data(_ui->graphCombo->selectedIndex(),TulipModel::GraphRole).value<tlp::Graph*>();
#ifndef NDEBUG

  if (g != NULL) {
    std::string name;
    g->getAttribute<std::string>("name",name);
    std::cerr << "selecting graph " << name << " in view" << std::endl;
  }

#endif /* NDEBUG */

  if (g != NULL && _view != NULL && g != _view->graph()) {
    _view->setGraph(g);
  }
}

bool WorkspacePanel::eventFilter(QObject* obj, QEvent* ev) {
  if (_viewConfigurationWidgets != NULL && _view != NULL) {
    if (obj == _view->graphicsView()->scene() && ev->type() == QEvent::GraphicsSceneMousePress) {
      QGraphicsSceneMouseEvent* mouseEv = static_cast<QGraphicsSceneMouseEvent*>(ev);
      bool expand = _viewConfigurationWidgets->sceneBoundingRect().contains(mouseEv->scenePos());
      setConfigurationTabExpanded(expand);
    }
  }

  return QWidget::eventFilter(obj,ev);
}

void WorkspacePanel::resizeEvent(QResizeEvent* ev) {
  if (_viewConfigurationWidgets) {
    setConfigurationTabExpanded(_viewConfigurationExpanded,false);
  }

  QWidget::resizeEvent(ev);
}

void WorkspacePanel::setConfigurationTabExpanded(bool expanded, bool animate) {
  QPointF newPos = configurationTabPosition(expanded);

  if (newPos == _viewConfigurationWidgets->pos())
    return;

  if (animate) {
    QPropertyAnimation* anim = new QPropertyAnimation(_viewConfigurationWidgets,"pos",_viewConfigurationWidgets);
    anim->setDuration(250);
    anim->setStartValue(_viewConfigurationWidgets->pos());
    anim->setEndValue(newPos);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
  }
  else
    _viewConfigurationWidgets->setPos(newPos);

  _viewConfigurationWidgets->setOpacity((expanded ? 0.9 : 0.6));
  _viewConfigurationExpanded = expanded;
}

QPointF WorkspacePanel::configurationTabPosition(bool expanded) const {
  if (expanded)
    return QPointF(width()-_viewConfigurationWidgets->size().width(),10);
  else {
    QTabWidget* tabWidget = static_cast<QTabWidget*>(_viewConfigurationWidgets->widget());
    int tabWidth = _viewConfigurationWidgets->size().width() - tabWidget->widget(0)->width();
    return QPointF(width() - tabWidth,10);
  }
}

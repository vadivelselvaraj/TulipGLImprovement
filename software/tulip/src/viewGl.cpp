//**********************************************************************

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <map>
#include <vector>

#if (QT_REL == 3)
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qworkspace.h>
#include <qmenubar.h>
#include <qtable.h>
#include <qvbox.h>
#include <qstatusbar.h>
#include <qpixmap.h>
#include <qstrlist.h>
#include <qimage.h>
#include <qpainter.h>
#include <qprogressdialog.h>
#include <qdockwindow.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qaction.h>
#include <qradiobutton.h>
#include <qprinter.h>
#include <qtabwidget.h>
#include <qdesktopwidget.h>
#else
#ifdef  _WIN32
// compilation pb workaround
#include <windows.h>
#endif
#include <QtGui/qmessagebox.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qapplication.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qfiledialog.h>
#include <QtGui/qinputdialog.h>
#include <QtGui/qworkspace.h>
#include <QtGui/qmenubar.h>
#include <QtGui/qdesktopwidget.h>
#include <Qt3Support/q3table.h>
#include <Qt3Support/q3vbox.h>
#include <QtGui/qstatusbar.h>
#include <QtGui/qpixmap.h>
#include <Qt3Support/q3strlist.h>
#include <QtGui/qimage.h>
#include <QtGui/qimagewriter.h>
#include <QtGui/qpainter.h>
#include <QtGui/qprogressdialog.h>
#include <Qt3Support/q3dockwindow.h>
#include <QtGui/qlayout.h>
#include <QtGui/qcombobox.h>
#include <QtGui/qcursor.h>
#include <QtGui/qaction.h>
#include <QtGui/qradiobutton.h>
#include <QtGui/qprinter.h>
#include "tulip/Qt3ForTulip.h"
#endif

#include <tulip/TlpTools.h>
#include <tulip/Reflect.h>
#include <tulip/GlGraphWidget.h>
#include <tulip/TulipElementProperties.h>
#include <tulip/PropertyWidgets.h>
#include <tulip/ClusterTree.h>
#include <tulip/PropertyProxy.h>
#include <tulip/SelectionProxy.h>
#include <tulip/SizesProxy.h>
#include <tulip/ColorsProxy.h>
#include <tulip/MetaGraphProxy.h>
#include <tulip/TlpQtTools.h>
#include <tulip/StableIterator.h>
#include <tulip/FindSelection.h>
#include <tulip/Morphing.h>
#include <tulip/ExtendedClusterOperation.h>

#include "TulipStatsWidget.h"
#include "PropertyDialog.h"
#include "viewGl.h"
#include "Application.h"
#include "QtProgress.h"
#include "ElementInfoToolTip.h"
#include "TabWidgetData.h"
#include "GridOptionsWidget.h"
#include "Overview.h"
#include "ToolBar.h"
#include "InfoDialog.h"
#include "AppStartUp.h"
#include <tulip/MouseMoveSelection.h>
          
#define UNNAMED "unnamed"

using namespace std;
using namespace tlp;


//**********************************************************************
///Constructor of ViewGl
viewGl::viewGl(QWidget* parent,	const char* name):TulipData( parent, name )  {
  //  cerr << __PRETTY_FUNCTION__ << endl;

#if (QT_REL == 4)
  // remove strange scories from designer/Tulip.ui
  Graph->removeAction(Action);
  Graph->removeAction(menunew_itemAction);
  // set workspace background
  workspace->setBackground(QBrush(Ui_TulipData::icon(image1_ID)));
#endif

  Observable::holdObservers();
  glWidget=0;
  gridOptionsWidget=0;
  aboutWidget=0;
  copyCutPasteGraph = 0;
  elementsDisabled = false;

  //=======================================

  //MDI
  workspace->setScrollBarsEnabled( TRUE );
  connect (workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(windowActivated(QWidget *)));
  //Create overview widget
  overviewDock = new QDockWindow(this,"Overview");
  overviewDock->setCaption("3D Overview");
  overviewDock->setCloseMode(QDockWindow::Always);
  overviewDock->setResizeEnabled(true);
  overviewWidget = new Overview(overviewDock);
  overviewDock->boxLayout()->add(overviewWidget);
  this->addDockWindow(overviewDock,"Overview", Qt::DockLeft);
  overviewWidget->view->GlGraph::setBackgroundColor(Color(255,255,255));
  overviewWidget->show();
  overviewDock->show();
  //Create Data information editor (Hierarchy, Element info, Property Info)
  tabWidgetDock = new QDockWindow(this,"Data manipulation");
  tabWidgetDock->setCaption("Info Editor");
  tabWidgetDock->setCloseMode(QDockWindow::Always);
  tabWidgetDock->setResizeEnabled(true);
  TabWidgetData *tabWidget = new TabWidgetData(tabWidgetDock);
#ifndef STATS_UI
  // remove Statistics tab if not needed
  tabWidget->tabWidget2->removePage(tabWidget->StatsTab);
#endif
  tabWidgetDock->boxLayout()->add(tabWidget);
  this->addDockWindow(tabWidgetDock,"Data manipulation", Qt::DockLeft);
  tabWidget->show();
  tabWidgetDock->show();
  //Create toolbar widget
  mouseToolBarDock = new QDockWindow(this,"Tool Bar");
  mouseToolBarDock->setCloseMode(QDockWindow::Always);
  mouseToolBar = new ToolBar(mouseToolBarDock);
  mouseToolBarDock->boxLayout()->add(mouseToolBar);
  this->addDockWindow(mouseToolBarDock,"ToolBar", Qt::DockRight);
  mouseToolBar->show();
  mouseToolBarDock->show();
  //Init hierarchy visualization widget
  clusterTreeWidget=tabWidget->clusterTree;
  //Init Property Editor Widget
  propertiesWidget=tabWidget->propertyDialog;
  //Init Element info widget
  nodeProperties = tabWidget->elementInfo;
#ifdef STATS_UI
  //Init Statistics panel
  statsWidget = tabWidget->tulipStats;
  statsWidget->setClusterTreeWidget(clusterTreeWidget);
#endif

  ((Application*)qApp)->nodeProperties = nodeProperties;
  //connect signals related to supergraph replacement
  connect(clusterTreeWidget, SIGNAL(supergraphChanged(SuperGraph *)), 
	  this, SLOT(hierarchyChangeSuperGraph(SuperGraph *)));
  connect(clusterTreeWidget, SIGNAL(aboutToRemoveView(SuperGraph *)), this, SLOT(superGraphAboutToBeRemoved(SuperGraph *)));
  connect(clusterTreeWidget, SIGNAL(aboutToRemoveAllView(SuperGraph *)), this, SLOT(superGraphAboutToBeRemoved(SuperGraph *)));
  //+++++++++++++++++++++++++++
  //Connection of the menu
  connect(&stringMenu     , SIGNAL(activated(int)), SLOT(changeString(int)));
  connect(&metricMenu     , SIGNAL(activated(int)), SLOT(changeMetric(int)));
  connect(&layoutMenu     , SIGNAL(activated(int)), SLOT(changeLayout(int)));
  connect(&selectMenu     , SIGNAL(activated(int)), SLOT(changeSelection(int)));
  connect(clusteringMenu  , SIGNAL(activated(int)), SLOT(makeClustering(int)));
  connect(&sizesMenu      , SIGNAL(activated(int)), SLOT(changeSizes(int)));
  connect(&intMenu        , SIGNAL(activated(int)), SLOT(changeInt(int)));
  connect(&colorsMenu     , SIGNAL(activated(int)), SLOT(changeColors(int)));
  connect(&exportGraphMenu, SIGNAL(activated(int)), SLOT(exportGraph(int)));
  connect(&importGraphMenu, SIGNAL(activated(int)), SLOT(importGraph(int)));
  connect(&exportImageMenu, SIGNAL(activated(int)), SLOT(exportImage(int)));
  connect(&dialogMenu     , SIGNAL(activated(int)), SLOT(showDialog(int)));
  windowsMenu->setCheckable( TRUE );
  connect(windowsMenu, SIGNAL( aboutToShow() ), this, SLOT( windowsMenuAboutToShow() ) );
  Observable::unholdObservers();
  morph = new Morphing();

  // initialization of Qt Assistant, the path should be in $PATH
#if defined(__APPLE__)
  std::string assistantPath(tlp::TulipLibDir);
  assistantPath += "../assistant";
  assistant = new QAssistantClient(assistantPath.c_str(), this);
#else
  assistant = new QAssistantClient("", this);
#endif
  connect(assistant, SIGNAL(error(const QString&)), SLOT(helpAssistantError(const QString&)));
}
//**********************************************************************
void viewGl::enableQPopupMenu(QPopupMenu *popupMenu, bool enabled) {
  int nbElements = popupMenu->count();
  for(int i = 0 ; i < nbElements ; i++) {
    int currentId = popupMenu->idAt(i);
    popupMenu->setItemEnabled(currentId, enabled);
  }
}
//**********************************************************************
void viewGl::enableElements(bool enabled) {
  enableQPopupMenu((QPopupMenu *) editMenu, enabled);
  enableQPopupMenu(&stringMenu, enabled);
  enableQPopupMenu(&layoutMenu, enabled);
  enableQPopupMenu(&metricMenu, enabled);
  enableQPopupMenu(&selectMenu, enabled);
  enableQPopupMenu(&intMenu, enabled);
  enableQPopupMenu(&sizesMenu, enabled);
  enableQPopupMenu(&colorsMenu, enabled);
  enableQPopupMenu((QPopupMenu *) clusteringMenu, enabled);
  enableQPopupMenu(&exportGraphMenu, enabled);
  enableQPopupMenu(&exportImageMenu, enabled);
  fileSaveAction->setEnabled(enabled);
  fileSaveAsAction->setEnabled(enabled);
  filePrintAction->setEnabled(enabled);
  grid_option->setEnabled(enabled);

  elementsDisabled = !enabled;
}
//**********************************************************************
void viewGl::observableDestroyed(Observable *) {
  //cerr << "[WARNING]" << __PRETTY_FUNCTION__ << endl;
}
//**********************************************************************
void viewGl::update ( ObserverIterator begin, ObserverIterator end) {
  Observable::holdObservers();
  clearObservers();
  nodeProperties->updateTable();
  propertiesWidget->update();
  if (gridOptionsWidget !=0 )
    gridOptionsWidget->validateGrid();
  redrawView();
  Observable::unholdObservers();
  initObservers();
}
//**********************************************************************
static const unsigned int NB_VIEWED_PROPERTIES=9;
static const string viewed_properties[NB_VIEWED_PROPERTIES]= {"viewLabel",
					   "viewColor",
					   "viewSelection",
					   "viewMetaGraph",
					   "viewShape",
					   "viewSize",
					   "viewTexture",
					   "viewLayout",
					   "viewRotation" };
//**********************************************************************
void viewGl::initObservers() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  if (!glWidget) return;
  SuperGraph *graph = glWidget->getSuperGraph();
  if (graph==0) return;
  for (unsigned int i=0; i<NB_VIEWED_PROPERTIES; ++i) {
    graph->getProperty(viewed_properties[i])->addObserver(this);
  }
}
//**********************************************************************
void viewGl::clearObservers() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  if (glWidget == 0) return;
  SuperGraph *graph = glWidget->getSuperGraph();
  if (graph == 0) return;
  for (unsigned int i=0; i<NB_VIEWED_PROPERTIES; ++i) {
    graph->getProperty(viewed_properties[i])->deleteObserver(this);
  }
}
//**********************************************************************
///Destructor of viewGl
viewGl::~viewGl() {
  delete morph;
  //  cerr << __PRETTY_FUNCTION__ << endl;
}
//**********************************************************************
void viewGl::startTulip() {
  // adjust size if needed
  QRect sRect = QApplication::desktop()->availableGeometry();
  QRect vRect(this->geometry());
  int needResize = 0;
  if (vRect.width() > sRect.width()) {
    vRect.setWidth(sRect.width());
    //vRect.setHeight(sRect.height());
    needResize = 1;
  }
  if (vRect.right() > sRect.right()) {
    vRect.moveLeft(sRect.left() + (sRect.width() - vRect.width())/2);
    needResize = 1;
  }
  if (vRect.height() > sRect.height()) {
    vRect.setHeight(sRect.height());
    //vRect.setWidth(sRect.width());
    needResize = 1;
  }
  if (vRect.bottom() > sRect.bottom()) {
    vRect.moveTop(sRect.top() + (sRect.height() - vRect.height())/2);
    needResize = 1;
  }
  if (needResize)
    this->setGeometry(vRect.x(), vRect.y(),
		      vRect.width(), vRect.height());

  AppStartUp *appStart=new AppStartUp(this);
  appStart->show();
  appStart->initTulip();
  delete appStart;
  buildMenus();
  overviewWidget->view->initializeGL();
  this->show();
  enableElements(false);
  int argc = qApp->argc();
  if (argc>1) {
    char ** argv = qApp->argv();
    for (int i=1;i<argc;++i) {
      QFileInfo info(argv[i]);
      QString s = info.absFilePath();
      fileOpen(0, s);
    }
  }
}
//**********************************************************************
void viewGl::changeSuperGraph(SuperGraph *graph) {
  //cerr << __PRETTY_FUNCTION__ << " (SuperGraph = " << (int)graph << ")" << endl;
  clearObservers();
  clusterTreeWidget->setSuperGraph(graph);
  propertiesWidget->setSuperGraph(graph);
  nodeProperties->setSuperGraph(graph);
  if(glWidget != 0) {
    propertiesWidget->setGlGraphWidget(glWidget);
    overviewWidget->setObservedView(glWidget);
#ifdef STATS_UI
    statsWidget->setGlGraphWidget(glWidget);
#endif
  }
  updateStatusBar();
  redrawView();
  initObservers();
}
//**********************************************************************
void viewGl::hierarchyChangeSuperGraph(SuperGraph *graph) {
  //cerr << __PRETTY_FUNCTION__ << " (SuperGraph = " << (int)graph << ")" << endl;
  if( !glWidget ) return;
  if (glWidget->getSuperGraph() == graph)  return;
  clearObservers();
  glWidget->setSuperGraph(graph);  
  changeSuperGraph(graph);
  initObservers();
}
//**********************************************************************
void viewGl::windowActivated(QWidget *w) {
  //cerr << __PRETTY_FUNCTION__ << " (QWidget = " << (int)w << ")" << endl;
  if (w==0)  {
    glWidget = 0;
    return;
  }
  if (typeid(*w)==typeid(GlGraphWidget)) {
    glWidget=((GlGraphWidget *)w);
    changeSuperGraph(glWidget->getSuperGraph());
  }
}
//**********************************************************************
GlGraphWidget * viewGl::newOpenGlView(SuperGraph *graph, const QString &name) {
  //Create 3D graph view
  GlGraphWidget *glWidget = new GlGraphWidget(workspace, name);
#if (QT_REL == 4)
  workspace->addWindow(glWidget);
#endif
  glWidget->setSuperGraph(graph);
  glWidget->move(0,0);
  glWidget->setCaption(name);
  glWidget->show();
  glWidget->setMinimumSize(0, 0);
  glWidget->resize(500,500);
  glWidget->setMaximumSize(32767, 32767);
#if (QT_REL == 3)
  glWidget->setFocusPolicy(QWidget::NoFocus);
#else
  glWidget->setFocusPolicy(Qt::NoFocus);
#endif
  glWidget->setBackgroundMode(Qt::PaletteBackground);  
  glWidget->installEventFilter(this);
  glWidget->setMouse(mouseToolBar->getCurrentMouse());
  connect(mouseToolBar,   SIGNAL(mouseChanged(MouseInterface *)), glWidget, SLOT(setMouse(MouseInterface *)));
  connect(mouseToolBar,   SIGNAL(mouseChanged(MouseInterface *)), SLOT(mouseChanged(MouseInterface *)));
  connect(glWidget,       SIGNAL(nodeClicked(SuperGraph *, const node &)), 
	  nodeProperties, SLOT(setCurrentNode(SuperGraph*, const node &)));
  connect(glWidget,       SIGNAL(nodeClicked(SuperGraph *, const node &)), 
	  this, SLOT(showElementProperties()));
  connect(glWidget,       SIGNAL(edgeClicked(SuperGraph *, const edge &)), 
	  nodeProperties, SLOT(setCurrentEdge(SuperGraph*, const edge &)));
  connect(glWidget,       SIGNAL(edgeClicked(SuperGraph *, const edge &)), 
	  this, SLOT(showElementProperties()));
  connect(glWidget, SIGNAL(closed(GlGraphWidget *)), this, SLOT(glGraphWidgetClosed(GlGraphWidget *)));

#if (QT_REL == 3)
  new ElementInfoToolTip(glWidget,"toolTip",glWidget);

  QToolTip::setWakeUpDelay(2500);
#endif

  changeSuperGraph(graph);

  if(elementsDisabled)
    enableElements(true);

  //cerr << __PRETTY_FUNCTION__ << "...END" << endl;
  qApp->processEvents();
  return glWidget;
}
//**********************************************************************
std::string viewGl::newName() {
  static int idx = 0;

  if (idx++ == 0)
    return std::string(UNNAMED);
  
  stringstream ss;
  ss << UNNAMED << '_' << idx - 1;
  return ss.str();
}    
//**********************************************************************
void viewGl::new3DView() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  DataSet param=glWidget->getParameters();
  //QString name(glWidget->name());
  newOpenGlView(glWidget->getSuperGraph(),glWidget->parentWidget()->caption());
  glWidget->setParameters(param);
  glWidget->setFontsPath(((Application *)qApp)->bitmapPath);
  //  cerr << __PRETTY_FUNCTION__ << "...END" << endl;
}
//**********************************************************************
void viewGl::fileNew() {
  Observable::holdObservers();
  SuperGraph *newSuperGraph=tlp::newSuperGraph();
  initializeGraph(newSuperGraph);
  GlGraph *glW = newOpenGlView(newSuperGraph,
			       newSuperGraph->getAttribute<string>(std::string("name")).c_str());
  initializeGlGraph(glW);
  overviewWidget->syncFromView();
  redrawView();
  Observable::unholdObservers();
}
//**********************************************************************
void viewGl::setNavigateCaption(string newCaption) {
   QWidgetList windows = workspace->windowList();
   for( int i = 0; i < int(windows.count()); ++i ) {
     QWidget *win = windows.at(i);
     if (typeid(*win)==typeid(GlGraphWidget)) {
       GlGraphWidget *tmpNavigate = dynamic_cast<GlGraphWidget *>(win);
       if(tmpNavigate == glWidget) {
	 tmpNavigate->setCaption(newCaption.c_str());
	 return;
       }
     }
   }
}
//**********************************************************************
void viewGl::fileSave() {
  if (!glWidget) return;
  if (openFiles.find((unsigned long)glWidget)==openFiles.end() || 
      (openFiles[(unsigned long)glWidget] == "")) {
    fileSaveAs();
    return;
  }
  fileSave("tlp", openFiles[(unsigned long)glWidget]);
}
//**********************************************************************
bool viewGl::fileSave(string plugin, string filename) {
  if (!glWidget) return false;
  ostream *os;
  if (filename.rfind(".gz") == (filename.length() - 3)) 
    os = tlp::getOgzstream(filename.c_str());
  else
    os = new ofstream(filename.c_str());
  DataSet dataSet;
  StructDef parameter = ExportModuleFactory::factory->getParam(plugin);
  parameter.buildDefaultDataSet(dataSet);
  if (!tlp::openDataSetDialog(dataSet, parameter, &dataSet, "Enter Export parameters"))
    return false;
  dataSet.set("displaying", glWidget->getParameters());
  bool result;
  if (!(result=tlp::exportGraph(glWidget->getSuperGraph(), *os, plugin, dataSet, NULL))) {
    QMessageBox::critical( 0, "Tulip export Failed",
			   "The file has not been saved"
			   );
  }
  setNavigateCaption(filename);
  delete os;
  return result;
}
//**********************************************************************
void viewGl::fileSaveAs() {
  if (!glWidget) return;
  if( !glWidget->getSuperGraph() )
    return;
  QString name = QFileDialog::getSaveFileName( QString::null,
					       tr("Tulip graph (*.tlp *.tlp.gz)"),
					       this,
					       tr("open file dialog"),
					       tr("Choose a file to save" ));
  if (name == QString::null) return;
  string filename = name.latin1();
  if (fileSave("tlp",filename)) 
    openFiles[(unsigned long)glWidget]=filename;
}
//**********************************************************************
void viewGl::fileOpen() {
  QString s;
  fileOpen(0,s);
}
//**********************************************************************
void viewGl::initializeGraph(SuperGraph *superGraph) {
  superGraph->setAttribute("name", newName());
  superGraph->getProperty<SizesProxy>("viewSize")->setAllNodeValue(Size(1,1,1));
  superGraph->getProperty<SizesProxy>("viewSize")->setAllEdgeValue(Size(0.125,0.125,0.5));
  superGraph->getProperty<ColorsProxy>("viewColor")->setAllNodeValue(Color(255,0,0));
  superGraph->getProperty<ColorsProxy>("viewColor")->setAllEdgeValue(Color(0,0,0));
  superGraph->getProperty<IntProxy>("viewShape")->setAllNodeValue(1);
  superGraph->getProperty<IntProxy>("viewShape")->setAllEdgeValue(0);
}
//**********************************************************************
void viewGl::initializeGlGraph(GlGraph *glGraph) {
    glGraph->setViewArrow(true);
    glGraph->setDisplayEdges(true);
    glGraph->setFontsType(1);
    glGraph->setFontsPath(((Application *)qApp)->bitmapPath);
    glGraph->setViewLabel(true);
    glGraph->setBackgroundColor(Color(255,255,255));
    glGraph->setViewOrtho(true);
    glGraph->setViewStrahler(false);
    glGraph->setEdgeColorInterpolate(false);
}
//**********************************************************************
void viewGl::fileOpen(string *plugin, QString &s) {
  Observable::holdObservers();
  DataSet dataSet;
  string tmpStr="tlp";
  bool cancel=false, noPlugin = true;
  if (s == QString::null) {
    if (plugin==0) {
      plugin = &tmpStr;
      s = QFileDialog::getOpenFileName( QString::null,
					tr("Tulip graph (*.tlp *.tlp.gz)"),
					this,
					tr("open file dialog"),
					tr("Choose a file to open" ));

      if (s == QString::null)
	cancel=true;
      else
	dataSet.set("file::filename", string(s.latin1()));
    }
    else {
      noPlugin = false;
      s = QString::null;
      StructDef parameter = ImportModuleFactory::factory->getParam(*plugin);
      parameter.buildDefaultDataSet( dataSet );
      cancel = !tlp::openDataSetDialog(dataSet, parameter, &dataSet, "Enter plugin parameter");
    }
  } else {
    plugin = &tmpStr;
    dataSet.set("file::filename", string(s.latin1()));
    noPlugin = true;
  }
  if (!cancel) {
    if(noPlugin) {
      QWidgetList windows = workspace->windowList();
      for( int i = 0; i < int(windows.count()); ++i ) {
	QWidget *win = windows.at(i);
	if (typeid(*win)==typeid(GlGraphWidget)) {
	  GlGraphWidget *tmpNavigate = dynamic_cast<GlGraphWidget *>(win);
	  if(openFiles[((unsigned long)tmpNavigate)] == s.latin1()) {
	    int answer = QMessageBox::question(this, "Open", "This file is already opened. Do you want to load it anyway?",  
					       QMessageBox::Yes,  QMessageBox::No);
	    if(answer == QMessageBox::No)
	      return;
	    break;
	  }
	}
      }
    }
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    SuperGraph *newGraph = tlp::newSuperGraph();
    initializeGraph(newGraph);
    if (s == QString::null)
      s = newGraph->getAttribute<string>("name").c_str();
    bool result=true;
    GlGraphWidget *glW = newOpenGlView(newGraph, s);
    initializeGlGraph(glW);
    glW->setSuperGraph(newGraph);
    changeSuperGraph(0);
    QtProgress *progressBar = new QtProgress(this,string("Loading : ")+ s.section('/',-1).ascii(), glW );
    result = tlp::importGraph(*plugin, dataSet, progressBar ,newGraph);
    if (progressBar->state()==TLP_CANCEL || !result ) {
      changeSuperGraph(0);
      delete glW;
      delete newGraph;
      QApplication::restoreOverrideCursor();
      // qWarning("Canceled import/Open");
      std::string errorTitle("Canceled import/Open: ");
      errorTitle += s.section('/',-1).ascii();
      std::string errorMsg = progressBar->getError();
      delete progressBar;
      Observable::unholdObservers();
      QMessageBox::critical(this, errorTitle.c_str(), errorMsg.c_str());
      return;
    }
    delete progressBar;
    if(noPlugin) {
      QString cleanName=s.section('/',-1);
      QStringList fields = QStringList::split('.', cleanName);
      cleanName=cleanName.section('.',-fields.count(), -2);
      newGraph->setAttribute("name", string(cleanName.latin1()));
    }


    if(noPlugin)
      openFiles[((unsigned long)glW)]=s.latin1();
    QApplication::restoreOverrideCursor();
    changeSuperGraph(0);
    changeSuperGraph(newGraph);
    restoreView();
    DataSet glGraphData;
    if (dataSet.get<DataSet>("displaying", glGraphData))
      glW->setParameters(glGraphData);
  }
  /* else {
    qWarning("Canceled  Open/import");
  } */
  Observable::unholdObservers();
}
//**********************************************************************
void viewGl::importGraph(int id) {
  string name(importGraphMenu.text(id).ascii());
  QString s;
  fileOpen(&name,s);
}

//==============================================================

namespace {
  typedef std::vector<node> NodeA;
  typedef std::vector<edge> EdgeA;

  void GetSelection(NodeA & outNodeA, EdgeA & outEdgeA,
		    SuperGraph *inG, SelectionProxy * inSel ) {
    assert( inSel );
    assert( inG );
    outNodeA.clear();
    outEdgeA.clear();
    // Get edges
    Iterator<edge> * edgeIt = inG->getEdges();
    while( edgeIt->hasNext() ) {
      edge e = edgeIt->next();
      if( inSel->getEdgeValue(e) )
	outEdgeA.push_back( e );
    } delete edgeIt;
    // Get nodes
    Iterator<node> * nodeIt = inG->getNodes();
    while( nodeIt->hasNext() ) {
      node n = nodeIt->next();
      if( inSel->getNodeValue(n) )
	outNodeA.push_back( n );
    } delete nodeIt;
  }

  void SetSelection(SelectionProxy * outSel, NodeA & inNodeA,
		    EdgeA & inEdgeA, SuperGraph * inG) {
    assert( outSel );
    assert( inG );
    outSel->setAllNodeValue( false );
    outSel->setAllEdgeValue( false );
    // Set edges
    for( unsigned int e = 0 ; e < inEdgeA.size() ; e++ )
      outSel->setEdgeValue( inEdgeA[e], true );
    // Set nodes
    for( unsigned int n = 0 ; n < inNodeA.size() ; n++ )
      outSel->setNodeValue( inNodeA[n], true );
  }
}
//**********************************************************************
void viewGl::editCut() {
  if( !glWidget ) return;
  SuperGraph * g = glWidget->getSuperGraph();
  if( !g ) return;
  // free the previous ccpGraph
  if( copyCutPasteGraph ) {
    delete copyCutPasteGraph;
    copyCutPasteGraph = 0;
  }
  SelectionProxy * selP = g->getProperty<SelectionProxy>("viewSelection");
  if( !selP ) return;
  // Save selection
  NodeA nodeA;
  EdgeA edgeA;
  GetSelection( nodeA, edgeA, g, selP );
  Observable::holdObservers();
  copyCutPasteGraph = tlp::newSuperGraph();
  tlp::copyToGraph( copyCutPasteGraph, g, selP );
  // Restore selection
  SetSelection( selP, nodeA, edgeA, g );
  tlp::removeFromGraph( g, selP );
  Observable::unholdObservers();
}
//**********************************************************************
void viewGl::editCopy() {
  if( !glWidget ) return;
  SuperGraph * g = glWidget->getSuperGraph();
  if( !g ) return;
  // free the previous ccpGraph
  if( copyCutPasteGraph ) {
    delete copyCutPasteGraph;
    copyCutPasteGraph = 0;
  }
  SelectionProxy * selP = g->getProperty<SelectionProxy>("viewSelection");
  if( !selP ) return;
  Observable::holdObservers();
  copyCutPasteGraph = tlp::newSuperGraph();
  tlp::copyToGraph( copyCutPasteGraph, g, selP );
  Observable::unholdObservers();
}
//**********************************************************************
void viewGl::editPaste() {
  if( !glWidget ) return;
  SuperGraph * g = glWidget->getSuperGraph();
  if( !g ) return;
  if( !copyCutPasteGraph ) return;
  Observable::holdObservers();
  SelectionProxy * selP = g->getProperty<SelectionProxy>("viewSelection");
  tlp::copyToGraph( g, copyCutPasteGraph, 0, selP );
  Observable::unholdObservers();
}
//**********************************************************************
void viewGl::editFind() {
  if(!glWidget) return;
  SuperGraph * g = glWidget->getSuperGraph();
  if( !g ) return;
  
  SelectionWidget *sel = new SelectionWidget(g);
  Observable::holdObservers();
  sel->exec();
  Observable::unholdObservers();
  delete sel;
}
//**********************************************************************
void viewGl::setParameters(const DataSet data) {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  glWidget->setParameters(data);
  clusterTreeWidget->setSuperGraph(glWidget->getSuperGraph());
  nodeProperties->setSuperGraph(glWidget->getSuperGraph());
  propertiesWidget->setSuperGraph(glWidget->getSuperGraph());
}
//**********************************************************************
void viewGl::updateStatusBar() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  SuperGraph *graph=clusterTreeWidget->getSuperGraph();
  if (graph==0) return;
  char tmp[255];
  sprintf(tmp,"Ready, Nodes:%d, Edges:%d",graph->numberOfNodes(),graph->numberOfEdges());
  statusBar()->message(tmp);
}
//*********************************************************************
static std::vector<std::string> getItemGroupNames(std::string itemGroup) {
  std::string::size_type start = 0;
  std::string::size_type end = 0;
  std::vector<std::string> groupNames;
  const char * separator = "::";

  while(true) {
    start = itemGroup.find_first_not_of(separator, end);
    if (start == std::string::npos) {
      return groupNames;
    }
    end = itemGroup.find_first_of(separator, start);
    if (end == std::string::npos)
      end = itemGroup.length();
    groupNames.push_back(itemGroup.substr(start, end - start));
  }
}
//**********************************************************************
static void insertInMenu(QPopupMenu &menu, string itemName, string itemGroup,
			 std::vector<QPopupMenu*> &groupMenus, std::string::size_type &nGroups) {
  std::vector<std::string> itemGroupNames = getItemGroupNames(itemGroup);
  QPopupMenu *subMenu = &menu;
  std::string::size_type nGroupNames = itemGroupNames.size();
  for (std::string::size_type i = 0; i < nGroupNames; i++) {
    QPopupMenu *groupMenu = (QPopupMenu *) 0;
    for (std::string::size_type j = 0; j < nGroups; j++) {
      if (itemGroupNames[i] == groupMenus[j]->name()) {
	subMenu = groupMenu = groupMenus[j];
	break;
      }
    }
    if (!groupMenu) {
      groupMenu = new QPopupMenu(subMenu, itemGroupNames[i].c_str());
      subMenu->insertItem(itemGroupNames[i].c_str(), groupMenu);
      groupMenus.push_back(groupMenu);
      nGroups++;
      subMenu = groupMenu;
    }
  }
  //cout << subMenu->name() << "->" << itemName << endl;
  subMenu->insertItem(itemName.c_str());
}
  
//**********************************************************************
template <typename TYPEN, typename TYPEE, typename TPROPERTY>
std::vector<QPopupMenu*> buildPropertyMenu(QPopupMenu &menu) {
  typename TemplateFactory<PropertyFactory<TPROPERTY>, TPROPERTY, PropertyContext>::ObjectCreator::const_iterator it;
  std::vector<QPopupMenu*> groupMenus;
  std::string::size_type nGroups = 0;
  it=PropertyProxy<TYPEN, TYPEE, TPROPERTY>::factory->objMap.begin();
  for (;it!=PropertyProxy<TYPEN,TYPEE, TPROPERTY>::factory->objMap.end();++it)
    insertInMenu(menu, it->first.c_str(), it->second->getGroup(), groupMenus, nGroups);
  return groupMenus;
}
//**********************************************************************
void viewGl::buildMenus(){
  //Properties PopMenus
  std::vector<QPopupMenu*> groupMenus = buildPropertyMenu<IntType, IntType, Int>(intMenu);
  std::string::size_type nGroups = groupMenus.size();
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(changeInt(int)));
  groupMenus = buildPropertyMenu<StringType, StringType, String>(stringMenu);
  nGroups = groupMenus.size();
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(changeString(int)));
  groupMenus = buildPropertyMenu<SizeType, SizeType, Sizes>(sizesMenu);
  nGroups = groupMenus.size();
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(changeSize(int)));
  groupMenus = buildPropertyMenu<ColorType, ColorType, Colors>(colorsMenu);
  nGroups = groupMenus.size();
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(changeColor(int)));
  groupMenus = buildPropertyMenu<PointType, LineType, Layout>(layoutMenu);
  nGroups = groupMenus.size();
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(changeLayout(int)));
  groupMenus = buildPropertyMenu<DoubleType,DoubleType,Metric>(metricMenu);
  nGroups = groupMenus.size();
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(changeMetric(int)));
  groupMenus = buildPropertyMenu<BooleanType,BooleanType, Selection>(selectMenu);
  nGroups = groupMenus.size();
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(changeSelection(int)));
  //Clustering PopMenu
  TemplateFactory<ClusteringFactory,Clustering,ClusterContext>::ObjectCreator::const_iterator it3;
  groupMenus.resize(nGroups = 0);
  for (it3=ClusteringFactory::factory->objMap.begin();it3!=ClusteringFactory::factory->objMap.end();++it3)
    clusteringMenu->insertItem(it3->first.c_str());
  //Export PopMenu
  TemplateFactory<ExportModuleFactory,ExportModule,ClusterContext>::ObjectCreator::const_iterator it9;
  groupMenus.resize(nGroups = 0);
  for (it9=ExportModuleFactory::factory->objMap.begin();it9!=ExportModuleFactory::factory->objMap.end();++it9)
    insertInMenu(exportGraphMenu, it9->first.c_str(), it9->second->getGroup(), groupMenus, nGroups);
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(exportGraph(int)));
  //Import PopMenu
  TemplateFactory<ImportModuleFactory,ImportModule,ClusterContext>::ObjectCreator::const_iterator it4;
  groupMenus.resize(nGroups = 0);
  for (it4=ImportModuleFactory::factory->objMap.begin();it4!=ImportModuleFactory::factory->objMap.end();++it4)
    insertInMenu(importGraphMenu, it4->first.c_str(), it4->second->getGroup(), groupMenus, nGroups);
  for (std::string::size_type i = 0; i < nGroups; i++)
    connect(groupMenus[i], SIGNAL(activated(int)), SLOT(importGraph(int)));
  //Image PopuMenu
#if (QT_REL == 3)
  QStrList listFormat=QImageIO::outputFormats();
  char *tmp=listFormat.first();
  while (tmp!=0) {
    exportImageMenu.insertItem(tmp);
    tmp=listFormat.next();
  }
#else
  foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
    exportImageMenu.insertItem(QString(format));
  }
#endif
  exportImageMenu.insertItem("EPS");
  //Windows
  dialogMenu.insertItem("Mouse Tool Bar");
  dialogMenu.insertItem("3D Overview");
  dialogMenu.insertItem("Info Editor");
  //==============================================================
  //File Menu 
  fileMenu->insertSeparator();
  if (importGraphMenu.count()>0)
    fileMenu->insertItem("&Import", &importGraphMenu );
  if (exportGraphMenu.count()>0)
    fileMenu->insertItem("&Export", &exportGraphMenu );
  if (exportImageMenu.count()>0)
    fileMenu->insertItem("&Save Picture as " , &exportImageMenu); //this , SLOT( outputImage() ));
  //View Menu
  viewMenu->insertItem( "Redraw View", this, SLOT( redrawView() ));
  viewMenu->insertItem( "Center View", this, SLOT( centerView() ));
  viewMenu->insertItem( "Restore View", this, SLOT( restoreView() ));
  viewMenu->insertItem( "Dialogs",  &dialogMenu);
  //EditMenu
  editMenu->insertSeparator();
  editMenu->insertItem( "Deselect All", this, SLOT( deselectALL() ));
  editMenu->insertItem( "Reverse Selection", this, SLOT( reverseSelection() ));
  editMenu->insertItem( "Delete Selection", this , SLOT(delSelection() ));
  editMenu->insertItem( "New subgraph", this, SLOT( newSubgraph() ));
  editMenu->insertItem( "Reverse selected edges direction", this, SLOT( reverseSelectedEdgeDirection() ));
  editMenu->insertItem( "group", this, SLOT( group() ));
  //Property Menu
  if (selectMenu.count()>0)
    propertyMenu->insertItem("&Selection", &selectMenu );
  if (layoutMenu.count()>0)
    propertyMenu->insertItem("&Layout", &layoutMenu );
  if (metricMenu.count()>0)
    propertyMenu->insertItem("&Metric", &metricMenu );
  if (colorsMenu.count()>0)
    propertyMenu->insertItem("&Colors", &colorsMenu );
  if (intMenu.count()>0)
    propertyMenu->insertItem("&Integer", &intMenu );
  if (stringMenu.count()>0)
    propertyMenu->insertItem("&String", &stringMenu );
  if (sizesMenu.count()>0)
    propertyMenu->insertItem("&Sizes", &sizesMenu );
}
//**********************************************************************
void viewGl::outputEPS() {
  if (!glWidget) return;
  QString s( QFileDialog::getSaveFileName());
  if (!s.isNull()) {
    glWidget->outputEPS(64000000,true,s.ascii());
  }
}
//**********************************************************************
void viewGl::exportImage(int id) {
  if (!glWidget) return;
  string name(exportImageMenu.text(id).ascii());
  if (name=="EPS") {
    outputEPS();
    return;
  }
  QString s(QFileDialog::getSaveFileName());
  if (s.isNull()) return;    
  int width,height;
  unsigned char* image = glWidget->getImage(width,height);
  QPixmap pm(width,height);
  QPainter painter;
  painter.begin(&pm);
  for (int y=0; y<height; y++)
    for (int x=0; x<width; x++) {
      painter.setPen(QColor(image[(height-y-1)*width*3+(x)*3],
			    image[(height-y-1)*width*3+(x)*3+1],
			    image[(height-y-1)*width*3+(x)*3+2]));
      painter.drawPoint(x,y);
    }
  painter.end();
  delete image;
  pm.save( s, name.c_str());
}
//**********************************************************************
void viewGl::exportGraph(int id) {
  if (!glWidget) return;
  QString filename(QFileDialog::getSaveFileName(this->caption().ascii()));
  if (filename.isNull()) return;    
  DataSet dataSet;
  string name(exportGraphMenu.text(id).ascii());
  fileSave(name, filename.ascii());
}
//**********************************************************************
void viewGl::windowsMenuActivated( int id ) {
  QWidget* w = workspace->windowList().at( id );
  if ( w ) {
    w->setFocus();
    w->show();
  }
}
//**********************************************************************
void viewGl::windowsMenuAboutToShow() {
  windowsMenu->clear();
  int cascadeId = windowsMenu->insertItem("&Cascade", workspace, SLOT(cascade() ) );
  int tileId = windowsMenu->insertItem("&Tile", workspace, SLOT(tile() ) );
  if ( workspace->windowList().isEmpty() ) {
    windowsMenu->setItemEnabled( cascadeId, FALSE );
    windowsMenu->setItemEnabled( tileId, FALSE );
  }
  windowsMenu->insertSeparator();
  QWidgetList windows = workspace->windowList();
  for ( int i = 0; i < int(windows.count()); ++i ) {
    int id = windowsMenu->insertItem(windows.at(i)->caption(),
				     this, SLOT( windowsMenuActivated( int ) ) );
    windowsMenu->setItemParameter( id, i );
    windowsMenu->setItemChecked( id, workspace->activeWindow() == windows.at(i) );
  }
}
//**********************************************************************
int viewGl::closeWin() {
  set<unsigned int> treatedGraph;
  QWidgetList windows = workspace->windowList();
  for(int i = 0; i < int(windows.count()); ++i ) {
    QWidget *win = windows.at(i);
    if (typeid(*win)==typeid(GlGraphWidget)) {
      GlGraphWidget *tmpNavigate = dynamic_cast<GlGraphWidget *>(win);
      SuperGraph *superGraph = tmpNavigate->getSuperGraph()->getRoot();
      if(!alreadyTreated(treatedGraph, superGraph)) {
	string message = "Do you want to save this graph : " + superGraph->getAttribute<string>("name") + " ?";
	int answer = QMessageBox::question(this, "Save", message.c_str(),  QMessageBox::Yes,  QMessageBox::No,
					   QMessageBox::Cancel);
	if(answer == QMessageBox::Cancel)
	  return false;
	if(answer == QMessageBox::Yes) {
	  glWidget = tmpNavigate;
	  fileSave();
	}
	treatedGraph.insert((unsigned long)superGraph);
      }
    }
  } 
  delete this;
  return true;
}
//**********************************************************************
int viewGl::alreadyTreated(set<unsigned int> treatedGraph, SuperGraph *graph) {
  set<unsigned int>::iterator iterator = treatedGraph.begin();
  while(iterator != treatedGraph.end()) {
    unsigned int currentGraph = *iterator;
    if(currentGraph == (unsigned long)graph)
      return true;
    iterator++;
  }
  return false;
}
//**********************************************************************
void viewGl::closeEvent(QCloseEvent *e) {
  if (closeWin())
    e->accept();
  else
    e->ignore();
}
//**********************************************************************
void viewGl::goInside() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  node tmpNode;
  edge tmpEdge;
  tlp::ElementType type;
  if (glWidget->doSelect(mouseClicX, mouseClicY, type, tmpNode,tmpEdge)) {
    if (type==NODE) {
      SuperGraph *supergraph=glWidget->getSuperGraph();
      MetaGraphProxy *meta=supergraph->getProperty<MetaGraphProxy>("viewMetaGraph");
      if (meta->getNodeValue(tmpNode)!=0) {
	changeSuperGraph(meta->getNodeValue(tmpNode));
      }
    }
  }
}
//**********************************************************************
void viewGl::ungroup() {
  node tmpNode;
  edge tmpEdge;
  tlp::ElementType type;
  if (glWidget->doSelect(mouseClicX, mouseClicY, type, tmpNode,tmpEdge)) {
    if (type==NODE) {
      SuperGraph *supergraph=glWidget->getSuperGraph();
      tlp::openMetaNode(supergraph, tmpNode);
    }
  }
  clusterTreeWidget->update();
}
//**********************************************************************
void viewGl::group() {
  set<node> tmp;
  SuperGraph *supergraph=glWidget->getSuperGraph();
  Iterator<node> *it=supergraph->getNodes();
  SelectionProxy *select = supergraph->getProperty<SelectionProxy>("viewSelection");
  while (it->hasNext()) {
    node itn = it->next();
    if (select->getNodeValue(itn))
	tmp.insert(itn);
  }delete it;
  if (tmp.empty()) return;
  if (supergraph == supergraph->getRoot()) {
    QMessageBox::critical( 0, "Warning" ,"Grouping can't be done on the root graph, a subgraph will be created");    
    supergraph = tlp::newCloneSubGraph(supergraph, "groups");
  }
  tlp::createMetaNode(supergraph, tmp);
  clusterTreeWidget->update();
  changeSuperGraph(supergraph);
}
//**********************************************************************
void viewGl::deleteElement(unsigned int x, unsigned int y, GlGraphWidget *glW){
  bool result;
  ElementType type;
  node tmpNode;
  edge tmpEdge;
  Observable::holdObservers();
  result = glW->doSelect(x, y, type, tmpNode, tmpEdge);
  if(result==true) {
    switch(type) {
    case NODE: glW->getSuperGraph()->delNode(tmpNode); break;
    case EDGE: glW->getSuperGraph()->delEdge(tmpEdge); break;
    }
  }
  Observable::unholdObservers();
}
//**********************************************************************
void viewGl::selectElement() {
  selectElement(mouseClicX, mouseClicY, glWidget, true);
}
//**********************************************************************
void viewGl::addRemoveElement() {
  selectElement(mouseClicX, mouseClicY, glWidget, false);
}
//**********************************************************************
void viewGl::deleteElement() {
  deleteElement(mouseClicX, mouseClicY, glWidget);
}
//**********************************************************************
void viewGl::selectElement(unsigned int x, unsigned int y, GlGraphWidget *glW, bool reset) {
  Observable::holdObservers();
  bool result;
  ElementType type;
  node tmpNode;
  edge tmpEdge;
  SelectionProxy *elementSelected = glW->getSuperGraph()->getProperty<SelectionProxy>("viewSelection");
  if (reset) {
    elementSelected->setAllNodeValue(false);
    elementSelected->setAllEdgeValue(false);
  }
  result = glW->doSelect(x, y, type, tmpNode, tmpEdge);
  if (result==true) {
    switch(type) {
    case NODE: elementSelected->setNodeValue(tmpNode, !elementSelected->getNodeValue(tmpNode)); break;
    case EDGE: elementSelected->setEdgeValue(tmpEdge, !elementSelected->getEdgeValue(tmpEdge)); break;
    }
  }
  Observable::unholdObservers();
}
//**********************************************************************
bool viewGl::eventFilter(QObject *obj, QEvent *e) {
  if ( obj->inherits("GlGraphWidget") &&
       (e->type() == QEvent::MouseButtonRelease)) {
    QMouseEvent *me = (QMouseEvent *) e;
    if (me->button()==RightButton) {
      mouseClicX = me->x();
      mouseClicY = me->y();
      QPopupMenu *contextMenu=new QPopupMenu(this,"dd");
      contextMenu->insertItem(tr("Go inside"), this, SLOT(goInside()));
      contextMenu->insertItem(tr("New 3D View"), this, SLOT(new3DView()));
      contextMenu->insertItem(tr("Delete"), this, SLOT(deleteElement()));
      contextMenu->insertItem(tr("Select"), this, SLOT(selectElement()));
      contextMenu->insertItem(tr("Add/Remove selection"), this, SLOT(addRemoveElement()));
      SuperGraph *supergraph=glWidget->getSuperGraph();
      if (supergraph != supergraph->getRoot())
	contextMenu->insertItem(tr("ungroup"), this, SLOT(ungroup()));
      contextMenu->exec(me->globalPos());
      delete contextMenu;
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}
//**********************************************************************
void viewGl::focusInEvent ( QFocusEvent * ) {
}
//**********************************************************************
void viewGl::showDialog(int id){
  string name(dialogMenu.text(id).ascii());
  if (name=="Mouse Tool Bar") {
    mouseToolBarDock->show();
    clusterTreeWidget->raise();
  }
  if (name=="Info Editor") {
    tabWidgetDock->show();
    tabWidgetDock->raise();
  }
  if (name=="3D Overview") {
    overviewDock->show();
    overviewDock->raise();
  }
}
//======================================================================
//Fonction du Menu de vue
//======================================================================
///Redraw the view of the graph
void  viewGl::redrawView() {
  if (!glWidget) return;
  glWidget->updateGL();
}
//**********************************************************************
///Reccenter the layout of the graph
void viewGl::centerView() {
  if (!glWidget) return;
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  Observable::holdObservers();
  glWidget->centerScene();
  redrawView();
  Observable::unholdObservers();
}
//**********************************************************************
///Restore the view of the graph
void viewGl::restoreView() {
  if (!glWidget) return;
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  Observable::holdObservers();
  glWidget->centerScene();
  redrawView();
  overviewWidget->setObservedView(glWidget);
  updateStatusBar();
  Observable::unholdObservers();
}
//===========================================================
//Menu Edit : functions
//===========================================================
///Deselect all entries in the glGraph current selection Proxy
void viewGl::deselectALL() {
  if (!glWidget) return;
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  Observable::holdObservers();
  graph->getProperty<SelectionProxy>("viewSelection")->setAllNodeValue(false);
  graph->getProperty<SelectionProxy>("viewSelection")->setAllEdgeValue(false);
  Observable::unholdObservers();
}
//**********************************************************************
void viewGl::delSelection() {
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  Observable::holdObservers();
  SelectionProxy *elementSelected=graph->getProperty<SelectionProxy>("viewSelection");
  StableIterator<node> itN(graph->getNodes());
  while(itN.hasNext()) {
    node itv = itN.next();
    if (elementSelected->getNodeValue(itv)==true)
      graph->delNode(itv);
  }
  StableIterator<edge> itE(graph->getEdges());
  while(itE.hasNext()) {
    edge ite=itE.next();
    if (elementSelected->getEdgeValue(ite)==true)   
      graph->delEdge(ite);
  }
  Observable::unholdObservers();
}
//==============================================================
///Reverse all entries in the glGraph current selection Proxy
void viewGl::reverseSelection() {
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  Observable::holdObservers();
  graph->getProperty<SelectionProxy>("viewSelection")->reverse();
  Observable::unholdObservers();
}
//==============================================================
void viewGl::newSubgraph() {
  if (!glWidget) return;
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  bool ok = FALSE;
  string tmp;
  bool verifGraph = true;
  SelectionProxy *sel1 = graph->getProperty<SelectionProxy>("viewSelection");  
  Observable::holdObservers();
  Iterator<edge>*itE = graph->getEdges();
  while (itE->hasNext()) {
    edge ite= itE->next();
    if (sel1->getEdgeValue(ite)) {
      if (!sel1->getNodeValue(graph->source(ite))) {sel1->setNodeValue(graph->source(ite),true); verifGraph=false;}
      if (!sel1->getNodeValue(graph->target(ite))) {sel1->setNodeValue(graph->target(ite),true); verifGraph=false;}
    }
  } delete itE;
  Observable::unholdObservers();
  
  if(!verifGraph) 
    QMessageBox::critical( 0, "Tulip Warning" ,"The selection wasn't a graph, missing nodes have been added");
  QString text = QInputDialog::getText( "View building" ,  "Please enter view name" , QLineEdit::Normal,QString::null, &ok, this );
  if (ok && !text.isEmpty()) {
    sel1 = graph->getProperty<SelectionProxy>("viewSelection");
    SuperGraph *tmp = graph->addSubGraph(sel1);
    tmp->setAttribute("name",string(text.latin1()));
    clusterTreeWidget->update();
  }
  else if (ok) {
    sel1 = graph->getProperty<SelectionProxy>("viewSelection");
    SuperGraph *tmp=graph->addSubGraph(sel1);
    tmp->setAttribute("name", newName());
    clusterTreeWidget->update();
  }
}
//==============================================================
void viewGl::reverseSelectedEdgeDirection() {
  if (!glWidget) return;
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  Observable::holdObservers();
  graph->getProperty<SelectionProxy>("viewSelection")->reverseEdgeDirection();  
  Observable::unholdObservers();
}
//==============================================================
void viewGl::superGraphAboutToBeRemoved(SuperGraph *sg) {
  //  cerr << __PRETTY_FUNCTION__ <<  "Possible bug" << endl;
  glWidget->setSuperGraph(0);
}
//==============================================================
void viewGl::helpAbout() {
  if (aboutWidget==0)
    aboutWidget = new InfoDialog(this);
  aboutWidget->show();
}
//==============================================================
void viewGl::helpIndex() {
  QStringList cmdList;
  cmdList << "-profile" 
	  << QString( (tlp::TulipDocProfile).c_str());
  
  assistant->setArguments(cmdList);
  if ( !assistant->isOpen() ){
    assistant->showPage(QString( (tlp::TulipUserHandBookIndex).c_str()));
    assistant->openAssistant();
  }
  else	
    assistant->showPage(QString( (tlp::TulipUserHandBookIndex).c_str()));
}
//==============================================================
void viewGl::helpContents() {
  QStringList cmdList;
  cmdList << "-profile" 
	  << QString( (tlp::TulipDocProfile).c_str());
  
  assistant->setArguments(cmdList);
  if ( !assistant->isOpen() ){ 
    assistant->showPage(QString( (tlp::TulipUserHandBookIndex).c_str()));
    assistant->openAssistant();
  }
  else	
    assistant->showPage(QString( (tlp::TulipUserHandBookIndex).c_str()));
}
//==============================================================
void viewGl::helpAssistantError(const QString &msg) {
  cerr << msg.ascii() << endl;
}
//==============================================================
void viewGl::fileExit() {
  closeWin();
}
//==============================================================
void viewGl::filePrint() {
  if (!glWidget) return;
  SuperGraph *graph=glWidget->getSuperGraph();
  if (graph==0) return;
  
  QPrinter printer;
  if (!printer.setup(this)) 
    return;
  int width,height;
  unsigned char* image = glWidget->getImage(width,height);
  QPainter painter(&printer);
  for (int y=0; y<height; y++)
    for (int x=0; x<width; x++) {
      painter.setPen(QColor(image[(height-y-1)*width*3+(x)*3],
			    image[(height-y-1)*width*3+(x)*3+1],
			    image[(height-y-1)*width*3+(x)*3+2]));
      painter.drawPoint(x,y);
    }
  painter.end();
  delete image;
}
//==============================================================
void viewGl::glGraphWidgetClosed(GlGraphWidget *navigate) {
  GlGraphWidget *w = navigate;
  SuperGraph *root = w->getSuperGraph()->getRoot();
  QWidgetList windows = workspace->windowList();
  int i;
  for( i = 0; i < int(windows.count()); ++i ) {
    QWidget *win = windows.at(i);
    if (typeid(*win)==typeid(GlGraphWidget)) {
      GlGraphWidget *tmpNavigate = dynamic_cast<GlGraphWidget *>(win);
      int superGraph1 = w->getSuperGraph()->getRoot()->getId();
      int superGraph2 = tmpNavigate->getSuperGraph()->getRoot()->getId();
      if((tmpNavigate != navigate) && (superGraph1 == superGraph2))
	break;
    }
  }
  if(i == int(windows.count())) {
    string message = "Do you want to save this graph : " +
      navigate->getSuperGraph()->getAttribute<string>("name") + " ?";

    int answer = QMessageBox::question(this, "Save", message.c_str(), QMessageBox::Yes,
				       QMessageBox::No, 
				       QMessageBox::Cancel);
    if(answer == QMessageBox::Cancel)
      return;
    if(answer == QMessageBox::Yes)
      fileSave();
    clusterTreeWidget->setSuperGraph(0);
    propertiesWidget->setSuperGraph(0);
    propertiesWidget->setGlGraphWidget(0);
    nodeProperties->setSuperGraph(0);
#ifdef STATS_UI
    statsWidget->setGlGraphWidget(0);
#endif
    w->setSuperGraph(0);
    delete root;
  }
  
  if (openFiles.find((unsigned long)w) != openFiles.end())   
    openFiles.erase((unsigned long)w);
  
  if(w == glWidget) {
    glWidget->setSuperGraph(0);
    glWidget = 0;
  }
  delete navigate;
  
  if(windows.count() == 1)
    enableElements(false);
} 
//**********************************************************************
///Make a new clustering of the view graph
void viewGl::makeClustering(int id) {
  clearObservers();
  if (glWidget==0) return;
  Observable::holdObservers();
  string name(clusteringMenu->text(id).ascii());
  string erreurMsg;
  DataSet dataSet;
  SuperGraph *graph=glWidget->getSuperGraph();
  StructDef parameter = ClusteringFactory::factory->getParam(name);
  parameter.buildDefaultDataSet( dataSet, graph );
  tlp::openDataSetDialog(dataSet, parameter, &dataSet, "Tulip Parameter Editor", graph );
  QtProgress myProgress(this,name);
  myProgress.hide();
  if (!tlp::clusterizeGraph(graph, erreurMsg, &dataSet, name, &myProgress  )) {
    QMessageBox::critical( 0, "Tulip Algorithm Check Failed",QString((name + "::" + erreurMsg).c_str()));
  }
  clusterTreeWidget->update();
  clusterTreeWidget->setSuperGraph(graph);
  Observable::unholdObservers();
  initObservers();
}
//**********************************************************************
//Management of properties
//**********************************************************************
template<typename PROPERTY>
bool viewGl::changeProperty(string name, string destination, bool query, bool redraw) {
  if( !glWidget ) return false;
  SuperGraph *graph = glWidget->getSuperGraph();
  if(graph == 0) return false;
  Observable::holdObservers();
  overviewWidget->setObservedView(0);    
  QtProgress myProgress(this, name, redraw ? glWidget : 0);
  string erreurMsg;
  bool   resultBool=true;  
  DataSet *dataSet =0;
  if (query) {
    dataSet = new DataSet();
    StructDef parameter = PROPERTY::factory->getParam(name);
    parameter.buildDefaultDataSet( *dataSet, graph );
    resultBool = tlp::openDataSetDialog(*dataSet, parameter, dataSet, "Tulip Parameter Editor", graph );
  }

  if (resultBool) {
    PROPERTY *dest = graph->template getLocalProperty<PROPERTY>(name);
    resultBool = graph->computeProperty(name, dest, erreurMsg, &myProgress, dataSet);
    if (!resultBool) {
      QMessageBox::critical( 0, "Tulip Algorithm Check Failed", QString((name + "::" + erreurMsg).c_str()) );
    }
    else 
      switch(myProgress.state()){
      case TLP_CONTINUE:
      case TLP_STOP:
	*graph->template getLocalProperty<PROPERTY>(destination) = *dest;
	break;
      case TLP_CANCEL:
	resultBool=false;
      };
    graph->delLocalProperty(name);
  }
  if (dataSet!=0) delete dataSet;

  propertiesWidget->setSuperGraph(graph);
  overviewWidget->setObservedView(glWidget);
  Observable::unholdObservers();
  return resultBool;
}
//**********************************************************************
void viewGl::changeString(int id) {
  clearObservers();
  string name(stringMenu.text(id).ascii());
  if (changeProperty<StringProxy>(name,"viewLabel"))
    redrawView();
  initObservers();
}
//**********************************************************************
void viewGl::changeSelection(int id) {
  clearObservers();
  string name(selectMenu.text(id).ascii());
  if (changeProperty<SelectionProxy>(name,"viewSelection"))
    redrawView();
  initObservers();
}
//**********************************************************************
void viewGl::changeMetric(int id) {
  clearObservers();
  string name(metricMenu.text(id).ascii());
  bool result = changeProperty<MetricProxy>(name,"viewMetric", true);
  if (result && map_metric->isOn()) {
    if (changeProperty<ColorsProxy>("Metric Mapping","viewColor", false))
      redrawView();
  }
  initObservers();
}
//**********************************************************************
void viewGl::changeLayout(int id) {
  clearObservers();
  string name(layoutMenu.text(id).ascii());
  GraphState * g0 = 0;
  if( enable_morphing->isOn() ) 
    g0 = new GraphState(glWidget);
  Camera cam = glWidget->getCamera();
  Coord scTrans = glWidget->getSceneTranslation();
  Coord scRot = glWidget->getSceneRotation();
  glWidget->setInputLayout(name);
  bool result = changeProperty<LayoutProxy>(name, "viewLayout", true, true);
  glWidget->setInputLayout("viewLayout");
  glWidget->setCamera(cam);
  glWidget->setSceneTranslation(scTrans);
  glWidget->setSceneRotation(scRot);
  if (result) {
    if( force_ratio->isOn() )
      glWidget->getSuperGraph()->getLocalProperty<LayoutProxy>("viewLayout")->perfectAspectRatio();
    //SuperGraph *graph=glWidget->getSuperGraph();
    Observable::holdObservers();
    glWidget->centerScene();
    overviewWidget->setObservedView(glWidget);
    updateStatusBar();
    Observable::unholdObservers();
    if( enable_morphing->isOn() ) {
      GraphState * g1 = new GraphState( glWidget );
      bool morphable = morph->start( glWidget, g0, g1);
      if( !morphable ) {
	delete g1;
	g1 = 0;
      } else {
	g0 = 0;	// state remains in morph data ...
      }
    }
  }
  redrawView();
  if( g0 )
    delete g0;
  initObservers();
}
  //**********************************************************************
void viewGl::changeInt(int id) {
  clearObservers();
  string name(intMenu.text(id).ascii());
  changeProperty<IntProxy>(name,"viewInt");
  initObservers();
}
  //**********************************************************************
void viewGl::changeColors(int id) {
  clearObservers();
  GraphState * g0 = 0;
  if( enable_morphing->isOn() )
    g0 = new GraphState( glWidget );
  string name(colorsMenu.text(id).ascii());
  bool result = changeProperty<ColorsProxy>(name,"viewColor");
  if( result ) {
    if( enable_morphing->isOn() ) {
      GraphState * g1 = new GraphState( glWidget );
      bool morphable = morph->start( glWidget, g0, g1);
      if( !morphable ) {
	delete g1;
	g1 = 0;
      } else {
	g0 = 0;	// state remains in morph data ...
      }
    }
    redrawView();
  }
  if( g0 )
    delete g0;
  initObservers();
}
//**********************************************************************
void viewGl::changeSizes(int id) {
  clearObservers();
  GraphState * g0 = 0;
  if( enable_morphing->isOn() )
    g0 = new GraphState( glWidget );
  string name(sizesMenu.text(id).ascii());
  bool result = changeProperty<SizesProxy>(name,"viewSize");
  if( result ) {
    if( enable_morphing->isOn() ) {
      GraphState * g1 = new GraphState( glWidget );
      bool morphable = morph->start( glWidget, g0, g1);
      if( !morphable ) {
	delete g1;
	g1 = 0;
      } else {
	g0 = 0;	// state remains in morph data ...
      }
    }
    redrawView();
  }
  if( g0 )
    delete g0;
  initObservers();
}
//**********************************************************************
void viewGl::gridOptions() {
  if (gridOptionsWidget == 0)
    gridOptionsWidget = new GridOptionsWidget(this);
  gridOptionsWidget->setCurrentGraphWidget(glWidget);
  gridOptionsWidget->show();
}
//**********************************************************************
void viewGl::mouseChanged(MouseInterface *m) {
}
//**********************************************************************
void viewGl::showElementProperties() {
  // show 'Element' tab in 'Info Editor'
  QWidget *tab = nodeProperties->parentWidget();
  QTabWidget *tabWidget = (QTabWidget *) tab->parentWidget()->parentWidget();
  tabWidget->showPage(tab);
}
//**********************************************************************
#include <tulip/AcyclicTest.h>
void viewGl::isAcyclic() {
  if (glWidget == 0) return;
  if (AcyclicTest::isAcyclic(glWidget->getSuperGraph()))
    QMessageBox::information( this, "Tulip test",
			   "The graph is acyclic"
			   );
  else
    QMessageBox::information( this, "Tulip test",
			   "The graph is not acyclic"
			   );
}
void viewGl::makeAcyclic() {
  if (glWidget == 0) return;
  Observable::holdObservers();
  vector<tlp::SelfLoops> tmpSelf;
  vector<edge> tmpReversed;
  AcyclicTest::makeAcyclic(glWidget->getSuperGraph(), tmpReversed, tmpSelf);
  Observable::unholdObservers();
}
//**********************************************************************
#include <tulip/SimpleTest.h>
void viewGl::isSimple() {
  if (glWidget == 0) return;
  if (SimpleTest::isSimple(glWidget->getSuperGraph()))
    QMessageBox::information( this, "Tulip test",
			   "The graph is simple"
			   );
  else
    QMessageBox::information( this, "Tulip test",
			   "The graph is not simple"
			   );
}
void viewGl::makeSimple() {
  if (glWidget == 0) return;
  Observable::holdObservers();
  vector<edge> removed;
  SimpleTest::makeSimple(glWidget->getSuperGraph(), removed);
  Observable::unholdObservers();
}
//**********************************************************************
#include <tulip/ConnectedTest.h>
void viewGl::isConnected() {
  if (glWidget == 0) return;
  if (ConnectedTest::isConnected(glWidget->getSuperGraph()))
    QMessageBox::information( this, "Tulip test",
			   "The graph is connected"
			   );
  else
    QMessageBox::information( this, "Tulip test",
			   "The graph is not connected"
			   );
}
void viewGl::makeConnected() {
  if (glWidget == 0) return;
  Observable::holdObservers();
  vector<edge> tmp;
  ConnectedTest::makeConnected(glWidget->getSuperGraph(), tmp);
  Observable::unholdObservers();
}
//**********************************************************************
#include <tulip/BiconnectedTest.h>
void viewGl::isBiconnected() {
  if (glWidget == 0) return;
  if (BiconnectedTest::isBiconnected(glWidget->getSuperGraph()))
    QMessageBox::information( this, "Tulip test",
			   "The graph is biconnected"
			   );
  else
    QMessageBox::information( this, "Tulip test",
			   "The graph is not biconnected"
			   );
}
void viewGl::makeBiconnected() {
  if (glWidget == 0) return;
  Observable::holdObservers();
  vector<edge> tmp;
  BiconnectedTest::makeBiconnected(glWidget->getSuperGraph(), tmp);
  Observable::unholdObservers();
}
//**********************************************************************
#include <tulip/TriconnectedTest.h>
void viewGl::isTriconnected() {
  if (glWidget == 0) return;
  if (TriconnectedTest::isTriconnected(glWidget->getSuperGraph()))
    QMessageBox::information( this, "Tulip test",
			   "The graph is triconnected"
			   );
  else
    QMessageBox::information( this, "Tulip test",
			   "The graph is not triconnected"
			   );
}
//**********************************************************************
#include <tulip/TreeTest.h>
void viewGl::isTree() {
  if (glWidget == 0) return;
  if (TreeTest::isTree(glWidget->getSuperGraph()))
    QMessageBox::information( this, "Tulip test",
			   "The graph is a tree"
			   );
  else
    QMessageBox::information( this, "Tulip test",
			   "The graph is not a tree"
			   );
}
//**********************************************************************
#include <tulip/PlanarityTest.h>
void viewGl::isPlanar() {
  if (glWidget == 0) return;
  Observable::holdObservers();
  if (PlanarityTest::isPlanar(glWidget->getSuperGraph()))
    QMessageBox::information( this, "Tulip test",
			   "The graph is planar"
			   );
  else
    QMessageBox::information( this, "Tulip test",
			   "The graph is not planar"
			   );
  Observable::unholdObservers();
}
//**********************************************************************

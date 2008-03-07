#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>

#ifdef  _WIN32
// compilation pb workaround
#include <windows.h>
#endif
#include <QtGui/qlistview.h>
#include <Qt3Support/q3table.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qinputdialog.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtGui/qlabel.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qtabwidget.h>
#include "tulip/Qt3ForTulip.h"

#include <tulip/Graph.h>
#include <tulip/DoubleProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/BooleanProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/PropertyWidget.h>
#include <tulip/SGHierarchyWidget.h>
#include <tulip/GlMainWidget.h>
#include <tulip/ForEach.h>
#include <tulip/hash_string.h>

#include "PropertyDialog.h"
#include "CopyPropertyDialog.h"

using namespace std;
using namespace tlp;

//==================================================================================================
PropertyDialog::PropertyDialog(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
  : PropertyDialogData(parent, name, (Qt::WFlags) (fl | Qt::Widget)) {
  _filterSelection=false;
  glWidget=0;
  graph=0;
  connect(localProperties, SIGNAL(selectionChanged(Q3ListViewItem *)), SLOT(changePropertyName(Q3ListViewItem *)));
  connect(inheritedProperties, SIGNAL(selectionChanged(Q3ListViewItem *)), SLOT(changePropertyName(Q3ListViewItem *)));
  connect(removeButton , SIGNAL(clicked()) , SLOT(removeProperty()) );
  connect(newButton,SIGNAL(clicked()),SLOT(newProperty()));
  connect(cloneButton,SIGNAL(clicked()),SLOT(cloneProperty()));

}
//=================================================
PropertyDialog::~PropertyDialog() {
}
//=================================================
void PropertyDialog::changePropertyName(QListViewItem *item) {
  if (item==0) {
    editedProperty=0;
    return;
  }
  string erreurMsg;

  tableNodes->selectNodeOrEdge(true);
  tableEdges->selectNodeOrEdge(false);
  tableNodes->changeProperty(graph,item->text(0).ascii());
  tableEdges->changeProperty(graph,item->text(0).ascii());

  PropertyInterface *tmpProxy=graph->getProperty(item->text(0).ascii());
  editedProperty=tmpProxy;
  editedPropertyName=item->text(0).ascii();
  propertyName->setText(item->text(0));
  
  if (graph->existLocalProperty(item->text(0).ascii()))
    inheritedProperties->clearSelection();
  else
    localProperties->clearSelection();

  tableEdges->update();
  tableNodes->update();
}
//=================================================
void PropertyDialog::filterSelection(bool b) {
  _filterSelection= b;
  tableEdges->filterSelection(b);
  tableNodes->filterSelection(b);
  update();
}
//=================================================
void PropertyDialog::update() {
  tableEdges->update();
  tableNodes->update();
}
//=================================================
void PropertyDialog::selectNode(node n){}
//=================================================
void PropertyDialog::selectEdge(edge e){}
//=================================================
void PropertyDialog::setAllValue() {
  if (tabWidget->currentPageIndex()==0)
    tableNodes->setAll();
  else
    tableEdges->setAll();
  setAllButton->setDown(false);
}
//=================================================
void PropertyDialog::setGlMainWidget(GlMainWidget *gw) {
  glWidget = gw;
  Graph* sg = NULL;
  if (gw)
    sg = gw->getScene()->getGlGraphComposite()->getInputData()->getGraph();
  graph=sg;
  editedProperty=0;

  localProperties->clear();
  inheritedProperties->clear();
  propertyName->setText(QString("Select a Property"));
  //Build the property list
  tableNodes->selectNodeOrEdge(true);
  tableEdges->selectNodeOrEdge(false);
  tableNodes->setGraph(sg);
  tableEdges->setGraph(sg);
  tableEdges->filterSelection(_filterSelection);
  tableNodes->filterSelection(_filterSelection);
  if (sg==0) return;

  Iterator<string> *it=sg->getLocalProperties();
  while (it->hasNext()) {
    string tmp=it->next();
    QListViewItem *tmpItem=new QListViewItem(localProperties);
    tmpItem->setText(0,QString(tmp.c_str()));
  } delete it;
  it=sg->getInheritedProperties();
  while (it->hasNext()) {
    string tmp=it->next();
    QListViewItem *tmpItem=new QListViewItem(inheritedProperties);
    tmpItem->setText(0,QString(tmp.c_str()));
  } delete it;
}
//=================================================
void PropertyDialog::newProperty() {
  if (!graph) return;
  QStringList lst;
  lst << "Color" << "Integer" << "Layout" << "Metric" << "Selection" << "Size" << "String";
  bool ok = FALSE;
  QString res = QInputDialog::getItem( "Property type","Please select the property type", lst, 3, FALSE, &ok, this );
  if ( ok ) {
      QString text = QInputDialog::getText("Property name", "Please enter the property name", QLineEdit::Normal, QString::null, &ok, this );
      if (ok) {
	string erreurMsg;
	if (strcmp(res.ascii(),"Selection")==0) graph->getLocalProperty<BooleanProperty>(text.ascii());
	if (strcmp(res.ascii(),"Metric")==0) graph->getLocalProperty<DoubleProperty>(text.ascii());
	if (strcmp(res.ascii(),"Layout")==0) graph->getLocalProperty<LayoutProperty>(text.ascii());
	if (strcmp(res.ascii(),"String")==0) graph->getLocalProperty<StringProperty>(text.ascii());
	if (strcmp(res.ascii(),"Integer")==0) graph->getLocalProperty<IntegerProperty>(text.ascii());
	if (strcmp(res.ascii(),"Size")==0) graph->getLocalProperty<SizeProperty>(text.ascii());
	if (strcmp(res.ascii(),"Color")==0) graph->getLocalProperty<ColorProperty>(text.ascii());
	setGlMainWidget(glWidget);
      }
  }
}
//=================================================
void PropertyDialog::toStringProperty() {
  if (editedProperty==0) return;
  string name=editedPropertyName;
  if (name == "viewLabel") return;
  Observable::holdObservers();
  PropertyInterface *newLabels=graph->getProperty(name);
  StringProperty *labels=graph->getLocalProperty<StringProperty>("viewLabel");
  if (tabWidget->currentPageIndex()==0) {
    labels->setAllNodeValue( newLabels->getNodeDefaultStringValue() );
    Iterator<node> *itN=graph->getNodes();
    while(itN->hasNext()) {
      node itn=itN->next();
      labels->setNodeValue(itn,  newLabels->getNodeStringValue(itn) );
    } delete itN;
  }
  else {
    labels->setAllEdgeValue( newLabels->getEdgeDefaultStringValue() );
    Iterator<edge> *itE=graph->getEdges();
    while(itE->hasNext()) {
      edge ite=itE->next();
      labels->setEdgeValue(ite,newLabels->getEdgeStringValue(ite));
    } delete itE;
  }
  glWidget->getScene()->getGlGraphComposite()->getInputData()->reloadLabelProperty();
  glWidget->draw();
  Observable::unholdObservers();
}
//=================================================
void PropertyDialog::removeProperty() {
  if (editedProperty==0) return;
  if(graph->existLocalProperty(editedPropertyName)) {
    graph->delLocalProperty(editedPropertyName);
    setGlMainWidget(glWidget);
    editedProperty=0;
  }
  else
    QMessageBox::critical( 0, "Tulip Property Editor Remove Failed",
			   QString("You cannot remove an inherited property,\n")
			   );
}
//=================================================
void PropertyDialog::cloneProperty() {
  if (!graph || !editedProperty) return;
  CopyPropertyDialog dialog(parentWidget());
  vector<string> localProps;
  vector<string> inheritedProps;
  string prop;
  Graph *parent = graph->getSuperGraph();
  if (parent == graph)
    parent = 0;
  forEach(prop, graph->getLocalProperties()) {
    if (typeid(*graph->getProperty(prop)) == typeid(*editedProperty)) {
      if (prop != editedPropertyName)
	localProps.push_back(prop);
      if (parent && parent->existProperty(prop))
	inheritedProps.push_back(prop);
    }
  }
  forEach(prop, graph->getInheritedProperties()) {
    if ((prop != editedPropertyName) &&
	(typeid(*graph->getProperty(prop)) == typeid(*editedProperty)))
      inheritedProps.push_back(prop);
  }
  dialog.setProperties(editedPropertyName, localProps, inheritedProps);
  CopyPropertyDialog::destType type;
  std::string text = dialog.getDestinationProperty(type);
  if (text.size() > 0) {
    if (type != CopyPropertyDialog::INHERITED) {
      if (graph->existProperty(text)) {
	if (typeid(*graph->getProperty(text))!=typeid(*editedProperty)) {
	  QMessageBox::critical(parentWidget(), "Tulip Warning" ,"Properties are not of the same type.");
	  return;
	}
	if (type == CopyPropertyDialog::NEW &&
	    graph->existLocalProperty(text)) {
	  if (text == editedPropertyName) {
	    QMessageBox::critical(parentWidget(), "Tulip Warning",
				  "Properties are the same.");
	    return;
	  } else if (QMessageBox::question(parentWidget(),
					   "Copy confirmation",
					   (std::string("Property ") + text + " already exists,\ndo you really want to overwrite it ?").c_str(),
					   QMessageBox::Ok,
					   QMessageBox::Cancel)
		     == QDialog::Rejected)
	    return;
	}
      }
      Observable::holdObservers();
      if (typeid((*editedProperty)) == typeid(DoubleProperty))
	{*graph->getLocalProperty<DoubleProperty>(text)=*((DoubleProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(LayoutProperty))
	{*graph->getLocalProperty<LayoutProperty>(text)=*((LayoutProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(StringProperty))
	{*graph->getLocalProperty<StringProperty>(text)=*((StringProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(BooleanProperty))
	{*graph->getLocalProperty<BooleanProperty>(text)=*((BooleanProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(IntegerProperty))
	{*graph->getLocalProperty<IntegerProperty>(text)=*((IntegerProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(ColorProperty))
	{*graph->getLocalProperty<ColorProperty>(text)=*((ColorProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(SizeProperty))
	{*graph->getLocalProperty<SizeProperty>(text)=*((SizeProperty*)editedProperty);}
      //}
    } else {
      Graph *parent = graph->getSuperGraph();
      Observable::holdObservers();
      if (typeid((*editedProperty)) == typeid(DoubleProperty))
	{*parent->getProperty<DoubleProperty>(text)=*((DoubleProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(LayoutProperty))
	{*parent->getProperty<LayoutProperty>(text)=*((LayoutProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(StringProperty))
	{*parent->getProperty<StringProperty>(text)=*((StringProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(BooleanProperty))
	{*parent->getProperty<BooleanProperty>(text)=*((BooleanProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(IntegerProperty))
	{*parent->getProperty<IntegerProperty>(text)=*((IntegerProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(ColorProperty))
	{*parent->getProperty<ColorProperty>(text)=*((ColorProperty*)editedProperty);}
      if (typeid((*editedProperty)) == typeid(SizeProperty))
	{*parent->getProperty<SizeProperty>(text)=*((SizeProperty*)editedProperty);}
    }
    setGlMainWidget(glWidget);
    Observable::unholdObservers();
  }
}
//=================================================

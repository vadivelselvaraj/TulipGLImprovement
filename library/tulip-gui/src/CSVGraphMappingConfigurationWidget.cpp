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
#include "tulip/CSVGraphMappingConfigurationWidget.h"
#include "ui_CSVGraphMappingConfigurationWidget.h"
#include "tulip/CSVGraphImport.h"
#include "tulip/PropertyCreationDialog.h"
#include <tulip/Graph.h>
#include <tulip/ForEach.h>
#include <tulip/TlpQtTools.h>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
using namespace tlp;
using namespace std;

CSVColumnComboBox::CSVColumnComboBox(QWidget* parent):QComboBox(parent),defaultText("Choose a csv column.") {
  addItem(defaultText,QVariant(UINT_MAX));
  setEnabled(false);
}

void CSVColumnComboBox::setDefaultText(const QString& newDefaultText) {
  defaultText=newDefaultText;
  //Update the text.
  setItemText(findData(QVariant(UINT_MAX)),newDefaultText);
}

void CSVColumnComboBox::setCsvProperties(const CSVImportParameters& csvProperties) {
  clear();
  addItem(defaultText,QVariant(UINT_MAX));

  if(csvProperties.columnNumber()==0) {
    setEnabled(false);
  }
  else {
    setEnabled(true);

    for(unsigned int i = 0 ; i< csvProperties.columnNumber(); ++i) {
      if(csvProperties.importColumn(i)) {
        addItem(tlpStringToQString(csvProperties.getColumnName(i)),QVariant(i));
      }
    }
  }

}

unsigned int CSVColumnComboBox::getSelectedColumnIndex()const {
  return itemData(currentIndex()).toUInt();
}

GraphPropertiesSelectionComboBox::GraphPropertiesSelectionComboBox(QWidget* parent):QComboBox(parent),currentGraph(NULL) {
  addItem(defaultText);
  //Avoid user to handle it when no graph is given.
  setEnabled(false);
}

void GraphPropertiesSelectionComboBox::setDefaultText(const QString& newDefaultText) {
  defaultText = newDefaultText;
}

void GraphPropertiesSelectionComboBox::setGraph(Graph* graph) {
  currentGraph = graph;
  clear();

  if(graph!=NULL) {
    if(!defaultText.isNull()) {
      addItem(defaultText,QVariant(QString()));
    }

    string propertyName;
    forEach(propertyName,currentGraph->getProperties()) {
      QString name = tlpStringToQString(propertyName);
      addItem(name,QVariant(name));
    }
    //Enable the combobox.
    setEnabled(true);
  }
  else {
    setEnabled(false);
  }
}

string GraphPropertiesSelectionComboBox::getSelectedGraphProperty()const {
  return QStringToTlpString(itemData(currentIndex()).toString());
}

void GraphPropertiesSelectionComboBox::selectProperty(const string& propertyName) {
  int index = findData(QVariant(tlpStringToQString(propertyName)));

  if(index != -1) {
    setCurrentIndex(index);
  }
}

CSVGraphMappingConfigurationWidget::CSVGraphMappingConfigurationWidget(QWidget *parent) :
  QWidget(parent),graph(NULL),
  ui(new Ui::CSVGraphMappingConfigurationWidget) {
  ui->setupUi(this);
  connect(ui->mappingConfigurationStackedWidget,SIGNAL(currentChanged(int)),this,SIGNAL(mappingChanged()));
  connect(ui->nodeMappingColumncomboBox,SIGNAL(currentIndexChanged (int)),this,SIGNAL(mappingChanged()));
  connect(ui->nodeMappingPropertycomboBox,SIGNAL(currentIndexChanged (int)),this,SIGNAL(mappingChanged()));
  connect(ui->sourceColumnComboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
  connect(ui->targetColumnComboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
  connect(ui->graphIndexPropertiesComboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
  connect(ui->edgeMappingColumncomboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
  connect(ui->edgeMappingPropertycomboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));

  connect(ui->createNewPropertyPushButton,SIGNAL(clicked(bool)),this,SLOT(createNewProperty()),Qt::QueuedConnection);

  //ui->graphIndexPropertiesComboBox->setDefaultText(tr("Select property against which newly found values are tested."));
  ui->sourceColumnComboBox->setDefaultText(tr("Choose CSV column containing source entities ids"));
  ui->targetColumnComboBox->setDefaultText(tr("Choose CSV column containing target entities ids"));

  ui->nodeMappingColumncomboBox->setDefaultText(tr("Choose CSV column containing entities ids"));
  ui->nodeMappingPropertycomboBox->setDefaultText(tr("Choose the property containing existing entities ids"));

  ui->edgeMappingColumncomboBox->setDefaultText(tr("Choose CSV column containing relations ids"));
  ui->edgeMappingPropertycomboBox->setDefaultText(tr("Choose the property containing existing relations ids"));

}

CSVGraphMappingConfigurationWidget::~CSVGraphMappingConfigurationWidget() {
  delete ui;
}

void CSVGraphMappingConfigurationWidget::changeEvent(QEvent *e) {
  QWidget::changeEvent(e);

  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;

  default:
    break;
  }
}

void CSVGraphMappingConfigurationWidget::updateWidget(tlp::Graph* graph,const CSVImportParameters& importParameters) {
  this->graph = graph;
  //Init mapping widgets.
  //Update node mapping widget
  ui->nodeMappingColumncomboBox->setCsvProperties(importParameters);
  ui->nodeMappingPropertycomboBox->setGraph(graph);
  //update edge from source and target mapping widget
  ui->sourceColumnComboBox->setCsvProperties(importParameters);
  ui->targetColumnComboBox->setCsvProperties(importParameters);
  ui->graphIndexPropertiesComboBox->setGraph(graph);
  //Update
  ui->edgeMappingColumncomboBox->setCsvProperties(importParameters);
  ui->edgeMappingPropertycomboBox->setGraph(graph);


  //Init default values
  if(importParameters.columnNumber() >0) {
    //Choose the first column as id column
    ui->nodeMappingColumncomboBox->setCurrentIndex(1);
    ui->edgeMappingColumncomboBox->setCurrentIndex(1);

    //Selct default columns for relations import
    if(importParameters.columnNumber() >1) {
      //Choose the first column as source id column
      ui->sourceColumnComboBox->setCurrentIndex(1);
      //Choose the second column as target id column
      ui->targetColumnComboBox->setCurrentIndex(2);
    }

  }

  //Choose viewLabel as default id property
  ui->nodeMappingPropertycomboBox->selectProperty("viewLabel");
  ui->edgeMappingPropertycomboBox->selectProperty("viewLabel");
  ui->graphIndexPropertiesComboBox->selectProperty("viewLabel");
}

CSVToGraphDataMapping *CSVGraphMappingConfigurationWidget::buildMappingObject() const {
  if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNewNodesPage) {
    return new CSVToNewNodeIdMapping(graph);
  }
  else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNodesPage) {
    //If checked user want to map it's csv column with csv param in graph
    string idPropertyName = ui->nodeMappingPropertycomboBox->getSelectedGraphProperty();
    unsigned int columnId = ui->nodeMappingColumncomboBox->getSelectedColumnIndex();

    if(idPropertyName.empty() || columnId==UINT_MAX) {
      return NULL;
    }

    bool createMissingElement = ui->createMissingNodesCheckBox->isChecked();
    return new CSVToGraphNodeIdMapping(graph,columnId,idPropertyName,createMissingElement);
  }
  else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesPages) {
    string idPropertyName = ui->edgeMappingPropertycomboBox->getSelectedGraphProperty();
    unsigned int columnId = ui->edgeMappingColumncomboBox->getSelectedColumnIndex();

    if(idPropertyName.empty() || columnId ==UINT_MAX) {
      return NULL;
    }

    return new CSVToGraphEdgeIdMapping(graph,columnId,idPropertyName);
  }
  else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesFromNodesPage) {
    string idPropertyName = ui->graphIndexPropertiesComboBox->getSelectedGraphProperty();
    unsigned int srcColumnId = ui->sourceColumnComboBox->getSelectedColumnIndex();
    unsigned int tgtColumnId = ui->targetColumnComboBox->getSelectedColumnIndex();

    if(idPropertyName.empty() ||idPropertyName.empty() || srcColumnId ==UINT_MAX || tgtColumnId == UINT_MAX || srcColumnId == tgtColumnId) {
      return NULL;
    }

    bool createMissingElement = ui->addMissingEdgeAndNodeCheckBox->isChecked();
    return new CSVToGraphEdgeSrcTgtMapping(graph,srcColumnId,tgtColumnId,idPropertyName,createMissingElement);
  }
  else {
    return NULL;
  }
}

bool CSVGraphMappingConfigurationWidget::isValid()const {
  if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNewNodesPage) {
    return true;
  }
  else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNodesPage) {
    string idPropertyName = ui->nodeMappingPropertycomboBox->getSelectedGraphProperty();
    unsigned int columnId = ui->nodeMappingColumncomboBox->getSelectedColumnIndex();

    if(idPropertyName.empty() || columnId==UINT_MAX || !graph->existProperty(idPropertyName)) {
      return false;
    }

    return true;
  }
  else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesPages) {
    string idPropertyName = ui->edgeMappingPropertycomboBox->getSelectedGraphProperty();
    unsigned int columnId = ui->edgeMappingColumncomboBox->getSelectedColumnIndex();

    if(idPropertyName.empty() || columnId ==UINT_MAX) {
      return false;
    }

    return true;
  }
  else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesFromNodesPage) {
    string idPropertyName = ui->graphIndexPropertiesComboBox->getSelectedGraphProperty();
    unsigned int srcColumnId = ui->sourceColumnComboBox->getSelectedColumnIndex();
    unsigned int tgtColumnId = ui->targetColumnComboBox->getSelectedColumnIndex();

    if(idPropertyName.empty() || !graph->existProperty(idPropertyName) || srcColumnId ==UINT_MAX || tgtColumnId == UINT_MAX || srcColumnId == tgtColumnId) {
      return false;
    }

    return true;
  }
  else {
    return false;
  }
}

void CSVGraphMappingConfigurationWidget::createNewProperty() {
  PropertyInterface* newProperty = PropertyCreationDialog::createNewProperty(graph,this);

  if(newProperty != NULL) {
    //Update gui with new property.
    string propertyName=newProperty->getName();
    ui->graphIndexPropertiesComboBox->setGraph(graph);
    ui->graphIndexPropertiesComboBox->selectProperty(propertyName);

    //Keep previously selected properties
    propertyName=ui->nodeMappingPropertycomboBox->getSelectedGraphProperty();
    ui->nodeMappingPropertycomboBox->setGraph(graph);
    ui->nodeMappingPropertycomboBox->selectProperty(propertyName);

    propertyName=ui->edgeMappingPropertycomboBox->getSelectedGraphProperty();
    ui->edgeMappingPropertycomboBox->setGraph(graph);
    ui->edgeMappingPropertycomboBox->selectProperty(propertyName);
  }
}

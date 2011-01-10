#include "tulip/CSVGraphMappingConfigurationWidget.h"
#include "tulip/CSVGraphMappingConfigurationWidgetData.h"
#include "tulip/CSVGraphImport.h"
#include <tulip/Graph.h>
#include <tulip/ForEach.h>
#include <tulip/TlpQtTools.h>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QKeyEvent>
using namespace tlp;
using namespace std;

CSVColumnComboBox::CSVColumnComboBox(QWidget* parent):QComboBox(parent){    
}

void CSVColumnComboBox::setCsvProperties(const CSVImportParameters& csvProperties){
    clear();
    addItem("Choose a csv column.");
    for(unsigned int i = 0 ; i< csvProperties.columnNumber(); ++i){
        addItem(tlpStringToQString(csvProperties.getColumnName(i)),QVariant(i));
    }
}

unsigned int CSVColumnComboBox::getSelectedColumnIndex()const{
    if(currentIndex()==0){
        return UINT_MAX;
    }else{
        return itemData(currentIndex()).toUInt();
    }
}

GraphPropertiesSelectionComboBox::GraphPropertiesSelectionComboBox(QWidget* parent):QComboBox(parent),currentGraph(NULL){
    connect(this,SIGNAL(activated( const QString &)),this,SLOT(newGraphPropertySelected(const QString&)));
}

void GraphPropertiesSelectionComboBox::setGraph(Graph* graph){
    currentGraph = graph;
    clear();
    if(graph!=NULL){
        QStringList labels;
        if(isEditable()){
            labels<<QString("Choose an existing property or enter a new one.");
        }else{
            labels<<QString("Choose an existing property.");
        }
        string propertyName;
        forEach(propertyName,currentGraph->getProperties()){
            labels<<tlpStringToQString(propertyName);
        }
        addItems(labels);
    }
}

void GraphPropertiesSelectionComboBox::keyPressEvent(QKeyEvent *e){
    //Qt workaround avoiding QDialog to close automatically
    // let base class handle the event
    QComboBox::keyPressEvent(e);
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        // accept enter/return events so they won't
        // be ever propagated to the parent dialog..
        e->accept();
    }
}

void GraphPropertiesSelectionComboBox::newGraphPropertySelected(const QString& propertyName){
    //If the widget is not in edition mode , if the graph is invalid, if the new property name is empty or if the selected index is the label.
    if(!isEditable() || currentGraph == NULL || propertyName.isEmpty() || currentIndex()==0)
        return;
    //The property don't exists create new one
    if(!currentGraph->existProperty(QStringToTlpString(propertyName))){        
        QDialog *dialog = new QDialog(this);
        dialog->setObjectName("createNewPropertyDialog");
        dialog->setLayout(new QVBoxLayout());
        dialog->setWindowTitle(tr("New graph property creation."));
        QLabel *label = new QLabel(tr("The property ") + propertyName + tr(" don't exists.\n Select type of the property you want to create."));
        label->setObjectName("createNewPropertyDialogLabel");
        dialog->layout()->addWidget(label);        
        QComboBox *propertyTypeSelection = new QComboBox(dialog);
        propertyTypeSelection->setObjectName("createNewPropertyDialogComboBox");
        propertyTypeSelection->addItems(PropertyTools::getPropertyTypeLabelsList());
        dialog->layout()->addWidget(propertyTypeSelection);
        QDialogButtonBox *buttons = new QDialogButtonBox(dialog);
        buttons->setObjectName("createNewPropertyDialogButtonBox");
        QPushButton* createButton = new QPushButton(tr("Create"),buttons);
        createButton->setObjectName("createNewPropertyDialogButton");
        createButton->setDefault(true);
        buttons->addButton(createButton,QDialogButtonBox::AcceptRole);
        buttons->addButton(QDialogButtonBox::Cancel)->setObjectName("cancelCreateNewPropertyButton");
        dialog->layout()->addWidget(buttons);
        connect(buttons,SIGNAL(accepted()),dialog,SLOT(accept()));
        connect(buttons,SIGNAL(rejected()),dialog,SLOT(reject()));
        if(dialog->exec()==QDialog::Accepted){
            string propertyType = PropertyTools::getPropertyTypeFromPropertyTypeLabel(QStringToTlpString(propertyTypeSelection->currentText()));
            PropertyTools::getProperty(currentGraph,QStringToTlpString(propertyName),propertyType);
        }else{
            //Delete the new property.
            removeItem(currentIndex());
            setCurrentIndex(0);
        }
        dialog->deleteLater();
    }
}

string GraphPropertiesSelectionComboBox::getSelectedGraphProperty()const{
    if(currentIndex()==0){
        return "";
    }else{
        return QStringToTlpString(currentText());
    }
}

CSVGraphMappingConfigurationWidget::CSVGraphMappingConfigurationWidget(QWidget *parent) :
        QWidget(parent),graph(NULL),
        ui(new Ui::CSVGraphMappingConfigurationWidget)
{
    ui->setupUi(this);
    connect(ui->mappingConfigurationStackedWidget,SIGNAL(currentChanged(int)),this,SIGNAL(mappingChanged()));
    connect(ui->nodeMappingColumncomboBox,SIGNAL(currentIndexChanged (int)),this,SIGNAL(mappingChanged()));
    connect(ui->nodeMappingPropertycomboBox,SIGNAL(currentIndexChanged (int)),this,SIGNAL(mappingChanged()));
    connect(ui->sourceColumnComboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
    connect(ui->targetColumnComboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
    connect(ui->graphIndexPropertiesComboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
    connect(ui->edgeMappingColumncomboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
    connect(ui->edgeMappingPropertycomboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(mappingChanged()));
}

CSVGraphMappingConfigurationWidget::~CSVGraphMappingConfigurationWidget()
{
    delete ui;
}

void CSVGraphMappingConfigurationWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CSVGraphMappingConfigurationWidget::updateWidget(tlp::Graph* graph,const CSVImportParameters& importParameters){
    this->graph = graph;
    this->importParameters = importParameters;
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
}

CSVToGraphDataMapping *CSVGraphMappingConfigurationWidget::buildMappingObject() const{
    if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNewNodesPage){
        return new CSVToNewNodeIdMapping(graph);
    }else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNodesPage){
        //If checked user want to map it's csv column with csv param in graph
        string idPropertyName = ui->nodeMappingPropertycomboBox->getSelectedGraphProperty();
        unsigned int columnId = ui->nodeMappingColumncomboBox->getSelectedColumnIndex();
        if(idPropertyName.empty() || columnId==UINT_MAX){
            return NULL;
        }
        bool createMissingElement = ui->createMissingNodesCheckBox->isChecked();
        return new CSVToGraphNodeIdMapping(graph,columnId,idPropertyName,importParameters.getFirstLineIndex(),importParameters.getLastLineIndex(),createMissingElement);
    }
    else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesPages){
        string idPropertyName = ui->edgeMappingPropertycomboBox->getSelectedGraphProperty();
        unsigned int columnId = ui->edgeMappingColumncomboBox->getSelectedColumnIndex();
        if(idPropertyName.empty() || columnId ==UINT_MAX){
            return NULL;
        }
        return new CSVToGraphEdgeIdMapping(graph,columnId,idPropertyName,importParameters.getFirstLineIndex(),importParameters.getLastLineIndex());
    }
    else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesFromNodesPage){
        string idPropertyName = ui->graphIndexPropertiesComboBox->getSelectedGraphProperty();
        unsigned int srcColumnId = ui->sourceColumnComboBox->getSelectedColumnIndex();
        unsigned int tgtColumnId = ui->targetColumnComboBox->getSelectedColumnIndex();
        if(idPropertyName.empty() || srcColumnId ==UINT_MAX || tgtColumnId == UINT_MAX){
            return NULL;
        }
        bool createMissingElement = ui->addMissingEdgeAndNodeCheckBox->isChecked();
        return new CSVToGraphEdgeSrcTgtMapping(graph,srcColumnId,tgtColumnId,idPropertyName,idPropertyName,importParameters.getFirstLineIndex(),importParameters.getLastLineIndex(),createMissingElement);
    }else{
        return NULL;
    }
}

bool CSVGraphMappingConfigurationWidget::isValid()const{
    if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNewNodesPage){
        return true;
    }else
        if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importNodesPage){
        string idPropertyName = ui->nodeMappingPropertycomboBox->getSelectedGraphProperty();
        unsigned int columnId = ui->nodeMappingColumncomboBox->getSelectedColumnIndex();
        if(idPropertyName.empty()|| columnId==UINT_MAX){
            return false;
        }
        return true;
    }
    else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesPages){
        string idPropertyName = ui->edgeMappingPropertycomboBox->getSelectedGraphProperty();
        unsigned int columnId = ui->edgeMappingColumncomboBox->getSelectedColumnIndex();
        if(idPropertyName.empty() || columnId ==UINT_MAX){
            return false;
        }
        return true;
    }
    else if(ui->mappingConfigurationStackedWidget->currentWidget()==ui->importEdgesFromNodesPage){
        string idPropertyName = ui->graphIndexPropertiesComboBox->getSelectedGraphProperty();
        unsigned int srcColumnId = ui->sourceColumnComboBox->getSelectedColumnIndex();
        unsigned int tgtColumnId = ui->targetColumnComboBox->getSelectedColumnIndex();
        if(idPropertyName.empty() || srcColumnId ==UINT_MAX || tgtColumnId == UINT_MAX){
            return false;
        }
        return true;
    }else{
        return false;
    }
}

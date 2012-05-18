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
#ifndef CSVGRAPHMAPPINGCONFIGURATIONWIDGET_H
#define CSVGRAPHMAPPINGCONFIGURATIONWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <tulip/tuliphash.h>
#include <tulip/CSVGraphImport.h>
class CSVGraphImport;

namespace Ui {
class CSVGraphMappingConfigurationWidget;
}

namespace tlp {

class Graph;


/**
  * @brief Combo box used to select a CSV column.
  **/
class TLP_QT_SCOPE CSVColumnComboBox : public QComboBox {
public:
  CSVColumnComboBox(QWidget* parent=NULL);
  /**
    * @brief Fill the combobox with the CSV parameters.
    *
    * The first item is not a valid choice it's just a label.
    **/
  void setCsvProperties(const CSVImportParameters& csvProperties);
  /**
    * @brief Get column selected by user or UINT_MAX if no valid column is selected.
    * @return The column selected by the user.
    *
    **/
  unsigned int getSelectedColumnIndex()const;
  /**
    * @brief Allow user to define it's default text.
    **/
  void setDefaultText(const QString& newDefaultText);
private:
  QString defaultText;
};

/**
  * @brief Combo box for property selection and creation in a graph.
  *
  * If the combo box is editable and if user enter the name of a property that doesn't exists this will create it.
  * A popup will querry user for the type of the property to create.
  * There is no way to delete a created property in this widget.
  **/
class TLP_QT_SCOPE GraphPropertiesSelectionComboBox : public QComboBox {
  Q_OBJECT
public:
  GraphPropertiesSelectionComboBox(QWidget* parent=NULL);
  /**
    * @brief Fill the combobox with the properties in the given graph.
    **/
  void setGraph(tlp::Graph*);
  /**
    * @brief return the name of the property selected by the user. If the label is selected an empty string will be returned.
    * @return The name of the selected property or an empty string if no valid property is selected.
    **/
  std::string getSelectedGraphProperty()const;

  /**
    * @brief Select the property with the given name in the list
    **/
  void selectProperty(const std::string& propertyName);
  /**
    * @brief Allow user to define it's default text.
    **/
  void setDefaultText(const QString& newDefaultText);

private:
  tlp::Graph *currentGraph;
  QString defaultText;

};


/**
  * @brief Widget generating the CSVToGraphDataMapping object.
  *
  * This widget allow user to configure a CSVToGraphDataMapping object. This object is used during the CSV import process to map CSV columns to graph elements like nodes or edges.
  **/
class TLP_QT_SCOPE CSVGraphMappingConfigurationWidget : public QWidget {
  Q_OBJECT
public:
  CSVGraphMappingConfigurationWidget(QWidget *parent = NULL);
  ~CSVGraphMappingConfigurationWidget();

  /**
    * @brief Configure the widget with the CSV import parameters.
    **/
  void updateWidget(tlp::Graph* graph,const CSVImportParameters& importParameters);
  /**
    * @brief Build the mapping object between the CSV columns and the graph elements.
    **/
  CSVToGraphDataMapping *buildMappingObject() const;

  /**
    * @brief Check if the values entered by user are valid.
    *
    * If return true you are sure that buildMappingObject never return a NULL object.
    **/
  bool isValid()const;

protected:
  void changeEvent(QEvent *e);
  tlp::Graph* graph;

private:
  Ui::CSVGraphMappingConfigurationWidget *ui;

private slots:
  void createNewProperty();

signals:
  void mappingChanged();
};
}
#endif // CSVGRAPHMAPPINGCONFIGURATIONWIDGET_H

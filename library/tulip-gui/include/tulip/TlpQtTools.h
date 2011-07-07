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
#ifndef _TLPQTTOOLS_H
#define _TLPQTTOOLS_H

#include "tulip/Color.h"
#include "tulip/Reflect.h"
#include "tulip/Graph.h"
#include "tulip/GlMainWidget.h"
#include "tulip/DataSet.h"

#include <QtGui/QColor>

class QWidget;

//Declare tulip type compatible with QVariant
Q_DECLARE_METATYPE(tlp::Graph*)
Q_DECLARE_METATYPE(tlp::DataSet)
Q_DECLARE_METATYPE(tlp::Color)
Q_DECLARE_METATYPE(tlp::Coord)
Q_DECLARE_METATYPE(tlp::Size)
Q_DECLARE_METATYPE(std::vector<bool>)
Q_DECLARE_METATYPE(std::vector<tlp::Color>)
Q_DECLARE_METATYPE(std::vector<tlp::Coord>)
Q_DECLARE_METATYPE(std::vector<double>)
Q_DECLARE_METATYPE(std::vector<int>)
Q_DECLARE_METATYPE(std::vector<tlp::Size>)
Q_DECLARE_METATYPE(std::vector<std::string>)

namespace tlp {

/**
 * Open a QT Dialog Box to fill-up an user DataSet according to a required
 * StructDef
 * inDef    : Input-variable informations
 * outSet   : Output (filled) set
 * [inSet   : Input (optional) set, containing some default values]
 */

TLP_QT_SCOPE bool openDataSetDialog (DataSet & outSet,
                                     const ParameterList *sysDef,
                                     ParameterList *inDef,
                                     const DataSet *inSet = NULL,
                                     const char *inName = NULL,
                                     Graph *inG = NULL,
                                     QWidget *parent = NULL);

/**
 * Create a main layout in GlMainWidget and display the graph on it
 */

TLP_QT_SCOPE void openGraphOnGlMainWidget(Graph *graph,DataSet *dataSet,GlMainWidget *glMainWidget);

TLP_QT_SCOPE bool getColorDialog(const QColor &color,QWidget *parent,const QString &title,QColor &result);

inline QColor colorToQColor(const Color& color) {
  return QColor(color.getR(), color.getG(), color.getB(), color.getA());
}
inline Color QColorToColor(const QColor& color) {
  return Color(color.red(), color.green(), color.blue(), color.alpha());
}
/**
  * @brief Convert a string from Tulip to QString.
  **/
inline std::string QStringToTlpString(const QString& toConvert) {
  return std::string(toConvert.toUtf8());
}
/**
  * @brief Convert a QString to tulip string.
  **/
inline QString tlpStringToQString(const std::string& toConvert) {
  return QString::fromUtf8(toConvert.c_str());
}

/**
    * @brief Convert the property type string to a label to display in the GUI.
    * The property type label is the string to display in the GUI instead of the basic property type string.
    * By example for a property of type "double" the label displayed in the GUI will be "Metric".
    **/
TLP_QT_SCOPE QString propertyTypeToPropertyTypeLabel(const std::string& typeName);

/**
  * @brief Get the string to display as property type for the given property.
  * The property type label is the string to display in the GUI instead of the property type string.
  * By example for a property of type "double" the label displayed in the GUI will be "Metric".
  **/
inline QString propertyInterfaceToPropertyTypeLabel(const tlp::PropertyInterface* const property) {
  return propertyTypeToPropertyTypeLabel(property->getTypename());
}

/**
  * @brief Convert the label of a property type to it's corresponding property type.
  * The property type label is the string to display in the GUI instead of the property type string.
  * By example for a property of type "double" the label displayed in the GUI will be "Metric".
  **/
TLP_QT_SCOPE std::string propertyTypeLabelToPropertyType(const QString& typeNameLabel);

/**
    * @brief Gets the name of the package to retrieve for this version of tulip.
    * The package name uses the Tulip release, platform (windows, unix, ...), architecture (x86, x86_64), and compiler used (GCC, Clang, MSVC) to determine which package this version can use.
    *
    * @param pluginName The name of the plugin for which we want the package name.
    **/
TLP_QT_SCOPE QString getPluginPackageName(const QString& pluginName);

}

#endif



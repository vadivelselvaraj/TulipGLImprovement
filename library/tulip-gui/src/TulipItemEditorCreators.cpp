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
#include "tulip/TulipItemEditorCreators.h"

#include <QtCore/QDebug>
#include <QtGui/QCheckBox>
#include <QtGui/QStylePainter>
#include <QtGui/QApplication>
#include <QtGui/QLinearGradient>
#include <QtGui/QPaintEvent>
#include <tulip/TlpQtTools.h>
#include <tulip/ColorButton.h>
#include <tulip/ColorScaleButton.h>
#include <tulip/TulipFileDescriptorWidget.h>
#include <tulip/SizeEditor.h>
#include <tulip/CoordEditor.h>
#include <tulip/GlyphRenderer.h>
#include <tulip/GlGraphStaticData.h>
#include <tulip/EdgeExtremityGlyphManager.h>
#include <tulip/EdgeExtremityGlyph.h>
#include <tulip/TulipFontWidget.h>
#include <tulip/GlyphManager.h>

using namespace tlp;

/*
  ColorEditorCreator
*/
QWidget* ColorEditorCreator::createWidget(QWidget *parent) const {
  return new ColorButton(parent);
}

bool ColorEditorCreator::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& v) const {
  painter->setBrush(colorToQColor(v.value<tlp::Color>()));
  painter->setPen(Qt::black);
  painter->drawRect(option.rect.x()+6,option.rect.y()+6,option.rect.width()-12,option.rect.height()-12);
  return true;
}

void ColorEditorCreator::setEditorData(QWidget *editor, const QVariant &data,tlp::Graph*) {
  static_cast<ColorButton*>(editor)->setTulipColor(data.value<tlp::Color>());
}

QVariant ColorEditorCreator::editorData(QWidget *editor,tlp::Graph*) {
  return QVariant::fromValue<tlp::Color>(static_cast<ColorButton*>(editor)->tulipColor());
}

/*
  BooleanEditorCreator
*/
QWidget* BooleanEditorCreator::createWidget(QWidget* parent) const {
  return new QCheckBox(parent);
}

QRect checkRect(const QStyleOptionViewItem &option,const QRect& bounding, QWidget* widget) {
  QStyleOptionButton opt;
  opt.QStyleOption::operator=(option);
  opt.rect = bounding;
  QStyle *style = widget ? widget->style() : QApplication::style();
  return style->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt, widget);
}

bool BooleanEditorCreator::paint(QPainter *p, const QStyleOptionViewItem &option, const QVariant &variant) const {
  QCheckBox cb;
  QStyleOptionViewItem opt(option);
  opt.rect = checkRect(option,option.rect,&cb);
  opt.state |= variant.toBool() ? QStyle::State_On : QStyle::State_Off;
  opt.state = opt.state & ~QStyle::State_HasFocus;
  QStyle* style = cb.style();
  style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck,&opt,p,&cb);
  return true;
}

void BooleanEditorCreator::setEditorData(QWidget* editor, const QVariant &data,tlp::Graph*) {
  static_cast<QCheckBox*>(editor)->setChecked(data.toBool());
}

QVariant BooleanEditorCreator::editorData(QWidget* editor,tlp::Graph*) {
  return static_cast<QCheckBox*>(editor)->isChecked();
}

/*
  CoordEditorCreator
*/
QWidget* CoordEditorCreator::createWidget(QWidget* parent) const {
  return new CoordEditor(parent);
}

void CoordEditorCreator::setEditorData(QWidget* w, const QVariant& v,tlp::Graph*) {
  static_cast<CoordEditor*>(w)->setCoord(v.value<tlp::Coord>());
}

QVariant CoordEditorCreator::editorData(QWidget* w,tlp::Graph*) {
  return QVariant::fromValue<tlp::Coord>(static_cast<CoordEditor*>(w)->coord());
}

/*
  SizeEditorCreator
*/
QWidget* SizeEditorCreator::createWidget(QWidget* parent) const {
  return new SizeEditor(parent);
}

void SizeEditorCreator::setEditorData(QWidget* w, const QVariant& v,tlp::Graph*) {
  static_cast<SizeEditor*>(w)->setSize(v.value<tlp::Size>());
}

QVariant SizeEditorCreator::editorData(QWidget* w,tlp::Graph*) {
  return QVariant::fromValue<tlp::Size>(static_cast<SizeEditor*>(w)->size());
}

/*
  PropertyInterfaceEditorCreator
*/
QWidget* PropertyInterfaceEditorCreator::createWidget(QWidget* parent) const {
  return new QComboBox(parent);
}

void PropertyInterfaceEditorCreator::setEditorData(QWidget* w, const QVariant& val,tlp::Graph* g) {
  PropertyInterface* prop = val.value<PropertyInterface*>();

  QSet<QString> locals,inherited;
  std::string name;
  forEach(name,g->getProperties()) {
    if (g->existLocalProperty(name))
      locals.insert(name.c_str());
    else
      inherited.insert(name.c_str());
  }

  QFont f;
  f.setBold(true);
  QComboBox* combo = static_cast<QComboBox*>(w);
  combo->clear();

  int index=0;
  foreach(QString s,inherited) {
    combo->addItem(s);
    combo->setItemData(index,f,Qt::FontRole);
    combo->setItemData(index,QObject::trUtf8("Inherited"),Qt::ToolTipRole);

    if (prop && s == prop->getName().c_str())
      combo->setCurrentIndex(index);

    index++;
  }

  foreach(QString s,locals) {
    combo->addItem(s);
    combo->setItemData(index,QObject::trUtf8("Local"),Qt::ToolTipRole);

    if (prop && s == prop->getName().c_str())
      combo->setCurrentIndex(index);

    index++;
  }

  if (!prop)
    combo->setCurrentIndex(0);
}

QVariant PropertyInterfaceEditorCreator::editorData(QWidget* w,tlp::Graph* g) {
  QComboBox* combo = static_cast<QComboBox*>(w);
  std::string propName = combo->currentText().toStdString();
  PropertyInterface *prop = NULL;

  if (g->existProperty(propName))
    prop = g->getProperty(propName);

  return QVariant::fromValue<PropertyInterface*>(prop);
}

QString PropertyInterfaceEditorCreator::displayText(const QVariant& v) const {
  PropertyInterface *prop = v.value<PropertyInterface*>();

  if (prop==NULL)
    return "";

  return prop->getName().c_str();
}

/*
  ColorScaleEditorCreator
*/
QWidget* ColorScaleEditorCreator::createWidget(QWidget* parent) const {
  return new ColorScaleButton(ColorScale(),parent);
}

bool ColorScaleEditorCreator::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& var) const {
  ColorScaleButton::paintScale(painter,option.rect,var.value<ColorScale>());
  return true;
}

void ColorScaleEditorCreator::setEditorData(QWidget* w, const QVariant& var,tlp::Graph*) {
  static_cast<ColorScaleButton*>(w)->setColorScale(var.value<ColorScale>());
}

QVariant ColorScaleEditorCreator::editorData(QWidget* w,tlp::Graph*) {
  return QVariant::fromValue<ColorScale>(static_cast<ColorScaleButton*>(w)->colorScale());
}

/*
  StringCollectionEditorCreator
*/
QWidget* StringCollectionEditorCreator::createWidget(QWidget* parent) const {
  return new QComboBox(parent);
}

void StringCollectionEditorCreator::setEditorData(QWidget* widget, const QVariant& var, tlp::Graph*) {
  StringCollection col = var.value<StringCollection>();
  QComboBox* combo = static_cast<QComboBox*>(widget);

  for(unsigned int i=0; i<col.size(); ++i)
    combo->addItem(col[i].c_str());

  combo->setCurrentIndex(col.getCurrent());
}

QVariant StringCollectionEditorCreator::editorData(QWidget* widget, tlp::Graph*) {
  QComboBox* combo = static_cast<QComboBox*>(widget);
  StringCollection col;

  for(int i=0; i < combo->count(); ++i)
    col.push_back(combo->itemText(i).toStdString());

  col.setCurrent(combo->currentIndex());
  return QVariant::fromValue<StringCollection>(col);
}

QString StringCollectionEditorCreator::displayText(const QVariant &var) const {
  StringCollection col = var.value<StringCollection>();
  return col[col.getCurrent()].c_str();
}

/*
  TulipFileDescriptorEditorCreator
  */
QWidget* TulipFileDescriptorEditorCreator::createWidget(QWidget* parent) const {
  return new TulipFileDescriptorWidget(parent);
}

void TulipFileDescriptorEditorCreator::setEditorData(QWidget* w, const QVariant& v,tlp::Graph*) {
  static_cast<TulipFileDescriptorWidget*>(w)->setData(v.value<TulipFileDescriptor>());
}

QVariant TulipFileDescriptorEditorCreator::editorData(QWidget* w,tlp::Graph*) {
  return QVariant::fromValue<TulipFileDescriptor>(static_cast<TulipFileDescriptorWidget*>(w)->data());
}

QString TulipFileDescriptorEditorCreator::displayText(const QVariant& v) const {
  TulipFileDescriptor desc = v.value<TulipFileDescriptor>();
  return desc.absolutePath;
}

///NodeShapeEditorCreator

QWidget* NodeShapeEditorCreator::createWidget(QWidget*parent) const {
  QComboBox* combobox = new QComboBox(parent);
  std::list<std::string> glyphs(PluginLister::instance()->availablePlugins<Glyph>());

  for(std::list<std::string>::const_iterator it = glyphs.begin(); it != glyphs.end(); ++it) {
    std::string glyphName(*it);
    int glyphIndex = GlyphManager::getInst().glyphId(glyphName);
    //Create the glyph entry
    combobox->addItem(GlyphRenderer::getInst().render(glyphIndex),tlpStringToQString(glyphName),glyphIndex);
  }

  return combobox;
}
void NodeShapeEditorCreator::setEditorData(QWidget* editor, const QVariant&data ,Graph*) {
  QComboBox* combobox = static_cast<QComboBox*>(editor);
  combobox->setCurrentIndex(combobox->findData(data.value<NodeShape>().nodeShapeId));
}

QVariant NodeShapeEditorCreator::editorData(QWidget*editor,Graph*) {
  QComboBox* combobox = static_cast<QComboBox*>(editor);
  return QVariant::fromValue<NodeShape>(NodeShape(combobox->itemData(combobox->currentIndex()).toInt()));
}

QString NodeShapeEditorCreator::displayText(const QVariant & data) const {
  return tlpStringToQString(GlyphManager::getInst().glyphName(data.value<NodeShape>().nodeShapeId));
}

bool NodeShapeEditorCreator::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& data) const {

  QStyleOptionViewItemV4 opt = option;
  opt.features |= QStyleOptionViewItemV2::HasDecoration;
  QPixmap pixmap = GlyphRenderer::getInst().render(data.value<NodeShape>().nodeShapeId);
  opt.icon = QIcon(pixmap);
  opt.decorationSize = pixmap.size();

  opt.features |= QStyleOptionViewItemV2::HasDisplay;
  opt.text = displayText(data);

  QStyle *style = QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, NULL);
  return true;
}

///EdgeExtremityShapeEditorCreator
QWidget* EdgeExtremityShapeEditorCreator::createWidget(QWidget* parent) const {
  QComboBox* combobox = new QComboBox(parent);
  combobox->addItem(QString("NONE"),EdgeExtremityGlyphManager::NoEdgeExtremetiesId);
  std::list<std::string> glyphs(PluginLister::instance()->availablePlugins<EdgeExtremityGlyph>());

  for(std::list<std::string>::const_iterator it = glyphs.begin(); it != glyphs.end(); ++it) {
    std::string glyphName(*it);
    int glyphIndex = EdgeExtremityGlyphManager::getInst().glyphId(glyphName);
    //Create the glyph entry
    combobox->addItem(EdgeExtremityGlyphRenderer::getInst().render(glyphIndex),tlpStringToQString(glyphName),glyphIndex);
  }

  return combobox;
}
void EdgeExtremityShapeEditorCreator::setEditorData(QWidget* editor, const QVariant& data,Graph*) {
  QComboBox* combobox = static_cast<QComboBox*>(editor);
  combobox->setCurrentIndex(combobox->findData(data.value<EdgeExtremityShape>().edgeExtremityShapeId));
}

QVariant EdgeExtremityShapeEditorCreator::editorData(QWidget* editor,Graph*) {
  QComboBox* combobox = static_cast<QComboBox*>(editor);
  return QVariant::fromValue<EdgeExtremityShape>(EdgeExtremityShape(combobox->itemData(combobox->currentIndex()).toInt()));
}

QString EdgeExtremityShapeEditorCreator::displayText(const QVariant &data) const {
  return tlpStringToQString(EdgeExtremityGlyphManager::getInst().glyphName(data.value<EdgeExtremityShape>().edgeExtremityShapeId));
}

bool EdgeExtremityShapeEditorCreator::paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& data) const {
  QStyleOptionViewItemV4 opt = option;
  opt.features |= QStyleOptionViewItemV2::HasDecoration;
  QPixmap pixmap = EdgeExtremityGlyphRenderer::getInst().render(data.value<EdgeExtremityShape>().edgeExtremityShapeId);
  opt.icon = QIcon(pixmap);
  opt.decorationSize = pixmap.size();

  opt.features |= QStyleOptionViewItemV2::HasDisplay;
  opt.text = displayText(data);

  QStyle *style = QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, NULL);
  return true;
}

///EdgeShapeEditorCreator
QWidget* EdgeShapeEditorCreator::createWidget(QWidget* parent) const {
  QComboBox* combobox = new QComboBox(parent);

  for (int i = 0; i < GlGraphStaticData::edgeShapesCount; i++) {
    combobox->addItem(tlpStringToQString(GlGraphStaticData::edgeShapeName(GlGraphStaticData::edgeShapeIds[i])),QVariant(GlGraphStaticData::edgeShapeIds[i]));
  }

  return combobox;
}
void EdgeShapeEditorCreator::setEditorData(QWidget* editor, const QVariant& data,Graph*) {
  QComboBox* combobox = static_cast<QComboBox*>(editor);
  combobox->setCurrentIndex(combobox->findData(static_cast<int>(data.value<EdgeShape>())));
}

QVariant EdgeShapeEditorCreator::editorData(QWidget* editor,Graph*) {
  QComboBox* combobox = static_cast<QComboBox*>(editor);
  return QVariant::fromValue<EdgeShape>(static_cast<EdgeShape>(combobox->itemData(combobox->currentIndex()).toInt()));
}

QString EdgeShapeEditorCreator::displayText(const QVariant &data) const {
  return tlpStringToQString(GlGraphStaticData::edgeShapeName(data.value<EdgeShape>()));
}

QWidget* TulipFontEditorCreator::createWidget(QWidget* parent) const {
  return new TulipFontWidget(parent);
}
void TulipFontEditorCreator::setEditorData(QWidget*editor, const QVariant&data,tlp::Graph*) {
  TulipFont font =data.value<TulipFont>();
  TulipFontWidget* fontWidget = static_cast<TulipFontWidget*>(editor);
  fontWidget->setFont(font);
}

QVariant TulipFontEditorCreator::editorData(QWidget* editor,tlp::Graph*) {
  TulipFontWidget* fontWidget = static_cast<TulipFontWidget*>(editor);
  return QVariant::fromValue<TulipFont>(fontWidget->font());
}

QString TulipFontEditorCreator::displayText(const QVariant & data) const {
  TulipFont font =data.value<TulipFont>();
  return font.fontName();
}

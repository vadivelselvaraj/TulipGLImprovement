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

#include <QtGui/QToolBar>
#include <QtGui/QIcon>

#include "PythonScriptViewWidget.h"
#include "PythonScriptView.h"

PythonScriptViewWidget::PythonScriptViewWidget(PythonScriptView *view, QWidget *parent) : QWidget(parent), fontZoom(0) , pythonScriptView(view) {
	setupUi(this);
	consoleOutputWidget->installEventFilter(new GragKeyboardFocusEventFilter());
	mainScriptToolBar = new QToolBar(mainScriptToolBarWidget);
	newMainScriptAction = mainScriptToolBar->addAction(QIcon(":/icons/doc_new.png"), "New main script");
	loadMainScriptAction = mainScriptToolBar->addAction(QIcon(":/icons/doc_import.png"), "Load main script from file");
	saveMainScriptAction = mainScriptToolBar->addAction(QIcon(":/icons/doc_export.png"), "Save main script to file");

	modulesToolBar = new QToolBar(modulesToolBarWidget);
	modulesToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	newStringModuleAction = modulesToolBar->addAction(QIcon(":/icons/doc_plus.png"), "New string module");
	newFileModuleAction = modulesToolBar->addAction(QIcon(":/icons/doc_new.png"), "New file module");
	loadModuleAction = modulesToolBar->addAction(QIcon(":/icons/doc_import.png"), "Import module from file");
	saveModuleAction = modulesToolBar->addAction(QIcon(":/icons/doc_export.png"), "Save module to file");

	pluginsToolBar = new QToolBar(pluginsToolBarWidget);
	pluginsToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	newPluginAction = pluginsToolBar->addAction(QIcon(":/icons/doc_plus.png"), "New Tulip plugin");
	loadPluginAction = pluginsToolBar->addAction(QIcon(":/icons/doc_import.png"), "Import Tulip plugin from file");
	savePluginAction = pluginsToolBar->addAction(QIcon(":/icons/doc_export.png"), "Save Tulip plugin to file");


	modulesTabWidget->clear();
	mainScriptsTabWidget->clear();
	pluginsTabWidget->clear();
	QList<int> sizes;
	sizes.push_back(550);
	sizes.push_back(150);
	splitter->setSizes(sizes);
	splitter->setCollapsible(0, false);
	pluginControlFrame->hide();

	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(resizeToolBars()));
	connect(decreaseFontSizeButton, SIGNAL(clicked()), this, SLOT(decreaseFontSize()));
	connect(increaseFontSizeButton, SIGNAL(clicked()), this, SLOT(increaseFontSize()));
	connect(decreaseFontSizeButton2, SIGNAL(clicked()), this, SLOT(decreaseFontSize()));
	connect(increaseFontSizeButton2, SIGNAL(clicked()), this, SLOT(increaseFontSize()));
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
}

std::string PythonScriptViewWidget::getCurrentMainScriptCode() const {
	return static_cast<PythonCodeEditor *>(mainScriptsTabWidget->currentWidget())->text().replace("\r\n", "\n").toStdString();
}

std::string PythonScriptViewWidget::getMainScriptCode(int idx) const {
	return static_cast<PythonCodeEditor *>(mainScriptsTabWidget->widget(idx))->text().replace("\r\n", "\n").toStdString();
}

std::string PythonScriptViewWidget::getModuleCode(int idx) const {
	return static_cast<PythonCodeEditor *>(modulesTabWidget->widget(idx))->text().replace("\r\n", "\n").toStdString();
}

std::string PythonScriptViewWidget::getPluginCode(int idx) const {
	return static_cast<PythonCodeEditor *>(pluginsTabWidget->widget(idx))->text().replace("\r\n", "\n").toStdString();
}

void PythonScriptViewWidget::resizeEvent(QResizeEvent *e) {
	QWidget::resizeEvent(e);
	resizeToolBars();
}

void PythonScriptViewWidget::showEvent(QShowEvent *e) {
	QWidget::showEvent(e);
	resizeToolBars();
}

void PythonScriptViewWidget::resizeToolBars() {
	modulesToolBar->resize(modulesToolBarWidget->size());
	mainScriptToolBar->resize(mainScriptToolBarWidget->size());
	pluginsToolBar->resize(pluginsToolBarWidget->size());
}

int PythonScriptViewWidget::addMainScriptEditor() {
	PythonCodeEditor *codeEditor = new PythonCodeEditor(this, fontZoom);
	codeEditor->installEventFilter(this);
	codeEditor->installEventFilter(pythonScriptView);
	codeEditor->setFocus(Qt::ActiveWindowFocusReason);
	connect(codeEditor, SIGNAL(textChanged()), this, SLOT(mainScriptTextChanged()));
	int idx = mainScriptsTabWidget->addTab(codeEditor, "");
	mainScriptsTabWidget->setCurrentIndex(idx);
	return idx;
}

int PythonScriptViewWidget::addModuleEditor() {
	PythonCodeEditor *codeEditor = new PythonCodeEditor(this);
	codeEditor->installEventFilter(this);
	codeEditor->installEventFilter(pythonScriptView);
	codeEditor->setFocus(Qt::ActiveWindowFocusReason);
	connect(codeEditor, SIGNAL(textChanged()), this, SLOT(moduleScriptTextChanged()));
	int idx = modulesTabWidget->addTab(codeEditor, "");
	modulesTabWidget->setCurrentIndex(idx);
	return idx;
}

int PythonScriptViewWidget::addPluginEditor() {
	PythonCodeEditor *codeEditor = new PythonCodeEditor(this);
	codeEditor->installEventFilter(this);
	codeEditor->installEventFilter(pythonScriptView);
	codeEditor->setFocus(Qt::ActiveWindowFocusReason);
	connect(codeEditor, SIGNAL(textChanged()), this, SLOT(pluginScriptTextChanged()));
	int idx = pluginsTabWidget->addTab(codeEditor, "");
	pluginsTabWidget->setCurrentIndex(idx);
	return idx;
}

PythonCodeEditor *PythonScriptViewWidget::getCurrentMainScriptEditor() const {
	return static_cast<PythonCodeEditor *>(mainScriptsTabWidget->currentWidget());
}

PythonCodeEditor *PythonScriptViewWidget::getCurrentModuleEditor() const {
	return static_cast<PythonCodeEditor *>(modulesTabWidget->currentWidget());
}

PythonCodeEditor *PythonScriptViewWidget::getMainScriptEditor(int idx) const {
	if (idx < mainScriptsTabWidget->count()) {
		return static_cast<PythonCodeEditor *>(mainScriptsTabWidget->widget(idx));
	} else {
		return NULL;
	}
}

PythonCodeEditor *PythonScriptViewWidget::getModuleEditor(int idx) const {
	if (idx < modulesTabWidget->count()) {
		return static_cast<PythonCodeEditor *>(modulesTabWidget->widget(idx));
	} else {
		return NULL;
	}
}

PythonCodeEditor *PythonScriptViewWidget::getPluginEditor(int idx) const {
	if (idx < pluginsTabWidget->count()) {
		return static_cast<PythonCodeEditor *>(pluginsTabWidget->widget(idx));
	} else {
		return NULL;
	}
}

PythonCodeEditor *PythonScriptViewWidget::getCurrentPluginEditor() const {
	return static_cast<PythonCodeEditor *>(pluginsTabWidget->currentWidget());
}

bool PythonScriptViewWidget::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvt = static_cast<QKeyEvent *>(event);
		if (keyEvt->modifiers() == Qt::ControlModifier && keyEvt->key() == Qt::Key_D) {
			commentSelectedCode(obj);
			return true;
		} else if (keyEvt->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && keyEvt->key() == Qt::Key_D) {
			uncommentSelectedCode(obj);
			return true;
		}
	}
	return false;
}

void PythonScriptViewWidget::commentSelectedCode(QObject *obj) {
	QsciScintilla *codeEditor = dynamic_cast<QsciScintilla *>(obj);
	if (codeEditor && codeEditor->hasSelectedText()) {
		int lineFrom = 0;
		int indexFrom = 0;
		int lineTo = 0;
		int indexTo = 0;
		codeEditor->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
		for (int i = lineFrom ; i <= lineTo ; ++i) {
			codeEditor->insertAt("#", i, 0);
		}
		codeEditor->setSelection(lineFrom, 0, lineTo, codeEditor->text(lineTo).length() - 1);
	}
}
void PythonScriptViewWidget::uncommentSelectedCode(QObject *obj) {
	QsciScintilla *codeEditor = dynamic_cast<QsciScintilla *>(obj);
	if (codeEditor && codeEditor->hasSelectedText()) {
		int lineFrom = 0;
		int indexFrom = 0;
		int lineTo = 0;
		int indexTo = 0;
		codeEditor->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
		for (int i = lineFrom ; i <= lineTo ; ++i) {
			codeEditor->setSelection(i, 0, i, 1);
			if (codeEditor->selectedText() == "#") {
				codeEditor->removeSelectedText();
			}
		}
		codeEditor->setSelection(lineFrom, 0, lineTo, codeEditor->text(lineTo).length() - 1);
	}
}

void PythonScriptViewWidget::decreaseFontSize() {
	for (int i = 0 ; i < mainScriptsTabWidget->count() ; ++i) {
		static_cast<QsciScintilla *>(mainScriptsTabWidget->widget(i))->zoomOut();
	}
	for (int i = 0 ; i < modulesTabWidget->count() ; ++i) {
		static_cast<QsciScintilla *>(modulesTabWidget->widget(i))->zoomOut();
	}
	for (int i = 0 ; i < pluginsTabWidget->count() ; ++i) {
		static_cast<QsciScintilla *>(pluginsTabWidget->widget(i))->zoomOut();
	}
	pythonShellWidget->zoomOut();
	--fontZoom;
}

void PythonScriptViewWidget::increaseFontSize() {
	for (int i = 0 ; i < mainScriptsTabWidget->count() ; ++i) {
		static_cast<QsciScintilla *>(mainScriptsTabWidget->widget(i))->zoomIn();
	}
	for (int i = 0 ; i < modulesTabWidget->count() ; ++i) {
		static_cast<QsciScintilla *>(modulesTabWidget->widget(i))->zoomIn();
	}
	for (int i = 0 ; i < pluginsTabWidget->count() ; ++i) {
		static_cast<QsciScintilla *>(pluginsTabWidget->widget(i))->zoomIn();
	}
	pythonShellWidget->zoomIn();
	++fontZoom;
}

void PythonScriptViewWidget::mainScriptTextChanged() {
	QString curTabText = mainScriptsTabWidget->tabText(mainScriptsTabWidget->currentIndex());
	if (curTabText == "")
		return;
	if (!curTabText.contains("no file") && curTabText[curTabText.size() -1] != '*') {
		curTabText += "*";
		mainScriptsTabWidget->setTabText(mainScriptsTabWidget->currentIndex(), curTabText);
	}
}

void PythonScriptViewWidget::moduleScriptTextChanged() {
	QString curTabText = modulesTabWidget->tabText(modulesTabWidget->currentIndex());
	if (curTabText == "")
		return;
	if (curTabText[curTabText.size() -1] != '*') {
		curTabText += "*";
		modulesTabWidget->setTabText(modulesTabWidget->currentIndex(), curTabText);
	}
}

void PythonScriptViewWidget::pluginScriptTextChanged() {
	QString curTabText = pluginsTabWidget->tabText(pluginsTabWidget->currentIndex());
	if (curTabText == "")
		return;
	if (curTabText[curTabText.size() -1] != '*') {
		curTabText += "*";
		pluginsTabWidget->setTabText(pluginsTabWidget->currentIndex(), curTabText);
	}
}

void PythonScriptViewWidget::currentTabChanged(int index) {
	static int lastTabIndex = 0;
	static QList<int> lastSizes = splitter->sizes();
	if (lastTabIndex != 2 && lastTabIndex != 3) {
		lastSizes = splitter->sizes();
	}
	QList<int> sizes;
	if (index == 2 || index == 3) {
		sizes.push_back(height());
		sizes.push_back(0);
		runScriptButton->setEnabled(false);
	} else {
		runScriptButton->setEnabled(true);
		sizes = lastSizes;
	}
	splitter->setSizes(sizes);

	if (index == 3) {
		scriptControlFrame->hide();
		pluginControlFrame->show();
	} else {
		scriptControlFrame->show();
		pluginControlFrame->hide();
	}

	lastTabIndex = index;
}

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
#include "PluginInformationsListItem.h"
#include <QtGui/QPainter>
#include "ui_PluginInformationsListItem.h"
#include <QtNetwork/QNetworkReply>

using namespace tlp;

PluginInformationsListItem::PluginInformationsListItem(PluginManager::PluginInformations infos, QWidget *parent): QWidget(parent), _ui(new Ui::PluginInformationsListItemData), _infos(infos) {
  _ui->setupUi(this);
  _ui->progressFrame->hide();
  _ui->installFrame->hide();
  _ui->rebootFrame->hide();
  _ui->installationControls->hide();

  PluginManager::PluginVersionInformations versionInfos = infos.installedVersion;

  if (!versionInfos.isValid) {
    versionInfos = infos.availableVersion;
    _ui->statusIcon->hide();
    _ui->installFrame->show();
  }
  else {
    if (infos.availableVersion.isValid && infos.availableVersion.version != versionInfos.version) {
      _ui->statusIcon->setPixmap(QPixmap(":/tulip/app/icons/16/package-upgrade.png"));
      _ui->installFrame->show();
    }

    else
      _ui->statusIcon->setPixmap(QPixmap(":/tulip/app/icons/16/package-installed-updated.png"));
  }

  _ui->icon->setPixmap(QPixmap(versionInfos.icon));
  _ui->name->setText(infos.name + " " + versionInfos.version);
  _ui->desc->setText(versionInfos.description + "\n\n" + trUtf8("Author: ") + versionInfos.author);
  _ui->installButton->setText(trUtf8("Install ") + infos.availableVersion.version);
}

PluginInformationsListItem::~PluginInformationsListItem() {
  delete _ui;
}

void PluginInformationsListItem::focusOut() {
  _ui->installationControls->hide();
  _ui->contentsFrame->setProperty("highlighted",false);
  _ui->contentsFrame->setStyleSheet(_ui->contentsFrame->styleSheet());
}

void PluginInformationsListItem::focusIn() {
  _ui->installationControls->show();
  _ui->contentsFrame->setProperty("highlighted",true);
  _ui->contentsFrame->setStyleSheet(_ui->contentsFrame->styleSheet());
}

void PluginInformationsListItem::install() {
  _ui->installFrame->hide();
  _ui->progressFrame->show();
  PluginManager::markForInstallation(_infos.name,this,SLOT(downloadProgress(qint64,qint64)));
  _ui->rebootFrame->show();
}

void PluginInformationsListItem::focusInEvent(QFocusEvent *) {
  emit focused();
}

void PluginInformationsListItem::downloadProgress(qint64 received, qint64 total) {
  _ui->progressBar->setMaximum(total);
  _ui->progressBar->setValue(received);
}

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
#include "TulipPerspectiveCrashHandler.h"

#include "ui_TulipPerspectiveCrashHandler.h"

#include <QtNetwork/QNetworkReply>
#include <QtCore/QFile>
#include "FormPost.h"

TulipPerspectiveCrashHandler::TulipPerspectiveCrashHandler(QWidget *parent)
  : QDialog(parent), _ui(new Ui::TulipPerspectiveCrashHandlerData), _isDetailedView(false) {
  _ui->setupUi(this);
  setDetailedView(false);
  adjustHeight();
  connect(_ui->detailsLink,SIGNAL(linkActivated(QString)),this,SLOT(toggleDetailedView()));
  connect(_ui->sendReportButton,SIGNAL(clicked()),this,SLOT(sendReport()));
  connect(_ui->saveButton,SIGNAL(clicked()),this,SLOT(saveData()));
}

void TulipPerspectiveCrashHandler::setDetailedView(bool f) {
  _isDetailedView = f;
  _ui->detailsFrame->setVisible(f);
  adjustHeight();
}

void TulipPerspectiveCrashHandler::adjustHeight() {
  setFixedSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
  int w = width();
  adjustSize();
  resize(w,height());
}

void TulipPerspectiveCrashHandler::toggleDetailedView() {
  setDetailedView(!_isDetailedView);
}

void TulipPerspectiveCrashHandler::sendReport() {
  _poster = new FormPost;
  _poster->addField("summary","[ Tulip " + _ui->versionValue->text() + " ] " + _ui->perspectiveNameValue->text() + " " + _ui->perspectiveArgumentsValue->text());
  _poster->addField("description",_ui->dumpEdit->toPlainText());
  _poster->addField("platform",_ui->compilerValue->text());
  _poster->addField("os",_ui->plateformValue->text());
  _poster->addField("os_build",_ui->archValue->text());

  //FIXME: temorary url until script is merged on Drupal
  connect(_poster->postData("http://auliyaa.dyndns.org/test.php"),SIGNAL(finished()),this,SLOT(reportPosted()));

  _ui->sendReportButton->setText(trUtf8("Sending report"));
  _ui->sendReportButton->setEnabled(false);
}

void TulipPerspectiveCrashHandler::reportPosted() {
  QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
  if (reply->error() == QNetworkReply::NoError) {
    _ui->sendReportButton->setText(trUtf8("Report sent"));
    _ui->errorReportTitle->setText(trUtf8("<b>Report has been sent. Thank you for supporting Tulip !</b>"));
  }
  else {
    _ui->sendReportButton->setText(trUtf8("Error while sending report"));
    _ui->errorReportTitle->setText("<i>" + reply->errorString() + "</i>");
    _ui->sendReportButton->setEnabled(true);
  }
  sender()->deleteLater();
  _poster->deleteLater();
}

void TulipPerspectiveCrashHandler::saveData() {

}

void TulipPerspectiveCrashHandler::setEnvData(const QString &plateform, const QString &arch, const QString &compiler, const QString &version, const QString &stackTrace) {
  _ui->plateformValue->setText(plateform);
  _ui->archValue->setText(arch);
  _ui->compilerValue->setText(compiler);
  _ui->versionValue->setText(version);
  _ui->dumpEdit->setPlainText(stackTrace);
}

void TulipPerspectiveCrashHandler::setPerspectiveData(PerspectiveProcessInfos infos) {
  _perspectiveInfos = infos;
  _ui->perspectiveNameValue->setText(infos.name);
  QString args;
  QString a;
  foreach(a,infos.args.keys())
  args += "--" + a + "=" + infos.args[a].toString() + " ";
  _ui->perspectiveArgumentsValue->setText(args);
}

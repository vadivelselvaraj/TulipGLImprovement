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

#include "TulipWelcomePage.h"

#include <QtGui/QDesktopServices>
#include <QtGui/QApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QXmlSimpleReader>

#include <tulip/TulipSettings.h>
#include <tulip/PluginManager.h>

#include "PerspectiveItemWidget.h"
#include "RssParser.h"
#include "ui_TulipWelcomePage.h"

static const QString RSS_URL = "http://tulip.labri.fr/TulipDrupal/?q=newsFeed.xml";
static const int RSS_LIMIT = 3;

TulipWelcomePage::TulipWelcomePage(QWidget *parent): QWidget(parent), _ui(new Ui::TulipWelcomePageData) {
  _ui->setupUi(this);

  //Finalize Ui
  connect(_ui->websiteLabel,SIGNAL(linkActivated(QString)),this,SLOT(openLink(QString)));
  _ui->openProjectButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon));
  connect(_ui->openProjectButton,SIGNAL(clicked()),this,SIGNAL(openProject()));

  // Fetch RSS
  _ui->rssScroll->setVisible(false);
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(rssReply(QNetworkReply*)));
  manager->get(QNetworkRequest(QUrl(RSS_URL)));

  // Recent documents list
  QList<QString> recentDocs = TulipSettings::instance().recentDocuments();

  if (recentDocs.size() > 0) {
    QString txt;

    for (QList<QString>::iterator it = recentDocs.begin(); it != recentDocs.end(); ++it)
      txt += trUtf8(("<p><span><img src=\":/tulip/app/ui/list_bullet_arrow.png\"></img>   <a href=\"" +
                     *it + "\">" + *it + "</a>" +
                     "</span></p><p/>").toStdString().c_str());

    _ui->recentDocumentsLabel->setText(txt);
  }

  // Perspectives list
  QVBoxLayout *perspectiveLayout = buildPerspectiveListLayout(this, SLOT(perspectiveSelected()));
  _ui->perspectivesFrame->setLayout(perspectiveLayout);
}

QVBoxLayout *TulipWelcomePage::buildPerspectiveListLayout(const QObject *receiver, const char *slot) {
  QVBoxLayout *perspectivesLayout = new QVBoxLayout();
  perspectivesLayout->setContentsMargins(6,6,6,6);
  QList<tlp::PluginInformations *> localPlugins = tlp::PluginManager::pluginsList(tlp::PluginManager::Local);
  tlp::PluginInformations *info;
  foreach(info,localPlugins) {
    if (info->type() != "Perspective")
      continue;

    PerspectiveItemWidget *item = new PerspectiveItemWidget(info);
    perspectivesLayout->addWidget(item);
    connect(item,SIGNAL(selected()),receiver,slot);
  }
  perspectivesLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Maximum,QSizePolicy::Expanding));
  return perspectivesLayout;
}

TulipWelcomePage::~TulipWelcomePage() {
}

void TulipWelcomePage::rssReply(QNetworkReply *reply) {
  sender()->deleteLater();
  QXmlSimpleReader rssReader;
  QXmlInputSource *rssSource = new QXmlInputSource(reply);
  RssParser *parser = new RssParser;
  rssReader.setContentHandler(parser);

  if (rssReader.parse(rssSource)) {
    _ui->rssError->setVisible(false);
    _ui->rssScroll->setVisible(true);

    QVBoxLayout *rssLayout = new QVBoxLayout;
    rssLayout->setContentsMargins(0,0,0,0);
    rssLayout->setSpacing(30);
    _ui->rssScroll->widget()->setLayout(rssLayout);

    QList<RssParser::RssItem> rssItems = parser->result();
    int i=0;

    for (QList<RssParser::RssItem>::iterator it = rssItems.begin(); it != rssItems.end(); ++it) {
      if (i++ >= RSS_LIMIT)
        break;

      QString text = "<p><span style=\"color:#626262; font-size:large;\">";
      text += it->title;
      text += "</span></p><p><span>";
      text += it->description;
      text += "</span></p></body></html>";
      QLabel *label = new QLabel(text,0);
      label->setMinimumWidth(1);
      label->setWordWrap(true);
      label->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
      connect(label,SIGNAL(linkActivated(QString)),this,SLOT(openLink(QString)));
      rssLayout->addWidget(label);
    }
  }

  delete rssSource;
  delete parser;
}

void TulipWelcomePage::openLink(const QString &link) {
  QDesktopServices::openUrl(link);
}

void TulipWelcomePage::perspectiveSelected() {
  PerspectiveItemWidget *item = static_cast<PerspectiveItemWidget *>(sender());
  emit openPerspective(item->perspectiveId());
}

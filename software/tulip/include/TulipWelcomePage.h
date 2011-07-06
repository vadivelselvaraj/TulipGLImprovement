#ifndef TULIPWELCOMEPAGE_H
#define TULIPWELCOMEPAGE_H

#include <QtGui/QWidget>

class QVBoxLayout;

namespace Ui {
class TulipWelcomePageData;
}

class QNetworkReply;

/**
  @brief The widget containing the Welcome screen in tulip_app.
  This widget will allow a user to select an installed perspective, read latest news from the Tulip website, and display startup tips.
  */
class TulipWelcomePage: public QWidget {
  Q_OBJECT
public:
  explicit TulipWelcomePage(QWidget *parent=0);
  virtual ~TulipWelcomePage();

  static QVBoxLayout *buildPerspectiveListLayout(const QObject *receiver, const char *slot);

signals:
  void openPerspective(QString);

protected slots:
  void rssReply(QNetworkReply *);
  void openLink(const QString &link);
  void perspectiveSelected();

private:
  Ui::TulipWelcomePageData *_ui;
};

#endif // TULIPWELCOMEPAGE_H

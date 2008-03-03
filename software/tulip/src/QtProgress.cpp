#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <qapplication.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include "QtProgress.h"
#include <tulip/GlGraphWidget.h>
#include <qprogressbar.h>
using namespace std;
using namespace tlp;

//=====================================
QtProgress::QtProgress(QWidget* parent,string text,GlGraphWidget *glGraphWidget):
  QtProgressData( parent, text.c_str(), true),
  firstCall(true),label(text),parent(parent),glGraphWidget(glGraphWidget) {
}
//=====================================
QtProgress::~QtProgress() {
}
//=====================================
void QtProgress::progress_handler(int i,int j) {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  progressBar->setTotalSteps(j);
  progressBar->setProgress (i);
  if (state()!=TLP_CONTINUE) { 
    return;
  }
  qApp->processEvents();
  if (firstCall) show();
  firstCall=false;
  if (glGraphWidget!=0 && preview->isChecked()) {
    glGraphWidget->getScene()->centerScene();
    glGraphWidget->draw();
  }
}
//=====================================
void QtProgress::setComment(string msg) {
  comment->setText(QString(msg.c_str()));
  if (firstCall) show();
  firstCall=false;
  qApp->processEvents();
}
//=====================================
void QtProgress::showPreview(bool flag) {
  if (flag)
    preview->show();
  else
    preview->hide();
}
//=====================================
void QtProgress::stopCompute(){
  //  cerr << __PRETTY_FUNCTION__ << endl;
  PluginProgress::stop();
}
//=====================================
void QtProgress::cancelCompute(){
  //  cerr << __PRETTY_FUNCTION__ << endl;
  PluginProgress::cancel();
}
//=====================================
void QtProgress::changePreview(bool b){
  // cerr << __PRETTY_FUNCTION__ << endl;
  PluginProgress::setPreviewMode(b);
}

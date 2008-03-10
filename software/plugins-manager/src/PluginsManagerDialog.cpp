#include "PluginsManagerDialog.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QMessageBox>

#include "PluginsManagerMainWindow.h"

using namespace std;

namespace tlp {

  void PluginsManagerDialog::createWidget(PluginsManagerMainWindow *pluginsManager) {
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pluginsManager);
    QDialog::setLayout(layout);
  }

  PluginsManagerDialog::PluginsManagerDialog(std::vector<LocalPluginInfo> &plugins,QWidget *parent):QDialog(parent){
    PluginsManagerMainWindow *pluginsManager=new PluginsManagerMainWindow(plugins,parent);
    createWidget(pluginsManager);
  }

  PluginsManagerDialog::PluginsManagerDialog(MultiServerManager *msm,QWidget *parent):QDialog(parent){
    PluginsManagerMainWindow *pluginsManager=new PluginsManagerMainWindow(msm,parent);
    createWidget(pluginsManager);
  }
}

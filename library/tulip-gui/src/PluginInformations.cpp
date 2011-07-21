#include <tulip/PluginInformations.h>

#include <tulip/WithDependency.h>
#include <tulip/AbstractPluginInfo.h>
#include <tulip/PluginLister.h>
#include <tulip/TulipRelease.h>

#include <QtCore/QFileInfo>
#include <tulip/TlpQtTools.h>
#include <tulip/DownloadManager.h>
#include <QDesktopServices>

using namespace tlp;

template <class T, class U>
static std::list<tlp::Dependency> getPluginDependencies(T* factory, U context) {
  return StaticPluginLister<T, U>::getPluginDependencies(factory->getName());
}

PluginInformations::PluginInformations(const tlp::AbstractPluginInfo& info, const std::string& type, const std::string& library)
  :_type(type.c_str()), _iconPath(":/tulip/gui/icons/logo32x32.png"), _longDescriptionPath("http://www.perdu.com"), _isLocal(true), _installedVersion(info.getRelease().c_str()), _updateAvailable(false) {
  _versions << info.getRelease().c_str();
//   PluginInfoWithDependencies pluginInfo(info, dependencies);
  _infos[info.getName().c_str()] = &info;
}

PluginInformations::PluginInformations(const tlp::AbstractPluginInfo& info, const std::string& type, const QString& basePath)
:_type(type.c_str()), _iconPath(basePath + "/icon.png"), _longDescriptionPath(basePath + "/html/index.html"), _isLocal(false), _installedVersion(QString::null), 
_updateAvailable(false), _remoteLocation(basePath), _remoteArchive(_remoteLocation + "/" + tlp::getPluginPackageName(info.getName().c_str())) {
  _versions << info.getRelease().c_str();
//   PluginInfoWithDependencies pluginInfo(info, dependencies);
  _infos[info.getName().c_str()] = &info;
}

void PluginInformations::AddPluginInformations(const tlp::AbstractPluginInfo* info) {
  QString newVersion = info->getRelease().c_str();

  if(_installedVersion > newVersion) {
    _updateAvailable = true;
  }

  _versions << newVersion;
//     PluginInfoWithDependencies pluginInfo(info, dependencies);
  _infos[info->getName().c_str()] = info;
}

void PluginInformations::AddPluginInformations(const tlp::PluginInformations* info) {
  foreach(const QString& version, info->versions()) {
    if(version > _installedVersion) {
      _updateAvailable = true;
    }

    _versions << version;
  }
}

QString PluginInformations::identifier() const {
  return _infos.begin().value()->getName().c_str();
}

QString PluginInformations::name() const {
  return _infos.begin().value()->getName().c_str();
}

QString PluginInformations::shortDescription() const {
  return _infos.begin().value()->getInfo().c_str();
}

QString PluginInformations::longDescriptionPath() const {
  return _longDescriptionPath;
}

QString PluginInformations::iconPath() const {
  return _iconPath;
}

QDateTime PluginInformations::installDate() const {
  return QDateTime::currentDateTime();
}

QString PluginInformations::type() const {
  return _type;
}

const QStringList PluginInformations::dependencies(QString version) const {
  QStringList result;
  result.reserve(_infos[version]->getDependencies().size());

  for(std::list<tlp::Dependency>::const_iterator it = _infos[version]->getDependencies().begin(); it != _infos[version]->getDependencies().end(); ++it) {
    result.append(it->pluginName.c_str());
  }

  return result;
}

const QStringList& PluginInformations::versions() const {
  return _versions;
}

QString PluginInformations::installedVersion() const {
  return _installedVersion;
}

bool PluginInformations::isInstalled(QString version) const {
  return version == _installedVersion;
}

bool PluginInformations::isInstalled() const {
  return !(installedVersion().isEmpty());
}

bool PluginInformations::updateAvailable() const {
  return _updateAvailable;
}

bool PluginInformations::fetch() const {
  const QString archiveName = tlp::getPluginPackageName(name());
  DownloadManager::getInstance()->downloadPlugin(_remoteArchive, QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/staging/" + archiveName);
  return false;
}

bool PluginInformations::remove() const {
  //FIXME implement me
  return false;
}

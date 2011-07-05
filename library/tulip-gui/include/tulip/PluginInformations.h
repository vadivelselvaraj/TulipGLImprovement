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

#ifndef PLUGININFORMATIONS_H
#define PLUGININFORMATIONS_H

#include <QtCore/QDateTime>
#include <QtGui/QIcon>

#include <tulip/tulipconf.h>
#include <tulip/AbstractPluginInfo.h>
#include <tulip/WithDependency.h>
#include <QMap>

namespace tlp {

/**
  * @brief This implementation of AbstractPluginInfo represents a plugin located on a remote server, thus not yet installed.
  * All the informations are 'manually' entered into this class.
  *
  **/
class TLP_QT_SCOPE DistantPluginInfo : public tlp::AbstractPluginInfo {
public:
  DistantPluginInfo(const std::string& author, const std::string& date, const std::string& group, const std::string& name, const std::string& info, const std::string& release,
                    const std::string& tulipRelease, const std::list<tlp::Dependency> dependencies)
  : _author(author), _date(date), _group(group), _name(name), _info(info), _release(release), _tulipRelease(tulipRelease), _dependencies(dependencies) {
  }
  virtual std::string getAuthor() const {
    return _author;
  }
  virtual std::string getDate() const {
    return _date;
  }
  virtual std::string getGroup() const {
    return _group;
  }
  virtual std::string getName() const {
    return _name;
  }
  virtual std::string getInfo() const {
    return _info;
  }
  virtual std::string getRelease() const {
    return _release;
  }
  virtual std::string getTulipRelease() const {
    return _tulipRelease;
  }
  virtual std::list<tlp::Dependency> getDependencies() const {
    return _dependencies;
  }
  
private:
  const std::string _author;
  const std::string _date;
  const std::string _group;
  const std::string _name;
  const std::string _info;
  const std::string _release;
  const std::string _tulipRelease;
  const std::list<tlp::Dependency> _dependencies;
};

/**
 * @brief Straightforward implementation of PluginInformationsInterface, useable for both remote and local plugins.
 **/
class TLP_QT_SCOPE PluginInformations {
  public:
    /**
     * @brief This constructor is used for local plugin description, and the library is used to determine the path of the icon and long description.
     *
     * @param info The AbstractPluginInfo representing the plugin to describe.
     * @param type The type of the plugin.
     * @param dependencies The dependencies of the plugin.
     * @param library The library file from which the plugin was loaded.
     **/
    PluginInformations(const tlp::AbstractPluginInfo* info, const std::string& type, const std::string& library);

    /**
     * @brief This constructor is used for remote plugin description, the long description and icon's paths are directly provided.
     *
     * @param info The AbstractPluginInfo representing the plugin to describe.
     * @param type The type of the plugin.
     * @param dependencies The dependencies of the plugin.
     * @param longDescriptionPath The URL where the long description resides.
     * @param iconPath The URL where the icon resides.
     **/
    PluginInformations(const tlp::AbstractPluginInfo* info, const std::string& type, const QString& longDescriptionPath, const QString& iconPath);

    void AddPluginInformations(const tlp::AbstractPluginInfo* info, const std::string& type, const std::list<tlp::Dependency>& dependencies);
    void AddPluginInformations(const tlp::PluginInformations* info);

    QString identifier() const;
    //TODO this should be a displayname, not the name used to register into the plugin system
    QString name() const;
    QString author() const { return "author"; }
    QString group() const { return "group"; }
    
    QString shortDescription() const;
    QString longDescriptionPath() const;
    
    QString iconPath() const;
    QDateTime installDate() const;
    
    QString type() const;
    const QStringList& dependencies(QString version) const;
    const QStringList& versions() const;
    QString latestVersion() const { return "latestVersion"; }

    QString installedVersion() const;
    bool isInstalled(QString version) const;
    bool isInstalled() const;
    bool updateAvailable() const;

    bool fetch(QString version) const;
    bool remove() const;
  private:
    QString _lastVersion;
    const QString _type;
    const QString _iconPath;
    const QString _longDescriptionPath;
    const bool _isLocal;
    const QString _installedVersion;
    bool _updateAvailable;
    QStringList _versions;
    QMap<QString, const tlp::AbstractPluginInfo*> _infos;
};

}

#endif //PLUGININFORMATIONS_H

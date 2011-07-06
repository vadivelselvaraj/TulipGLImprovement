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

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtXml/QDomDocument>

#include <tulip/SystemDefinition.h>
#include <tulip/PluginLister.h>
#include <tulip/TulipRelease.h>
#include <tulip/PluginInformations.h>
#include <tulip/PluginLoaderTxt.h>
#include <tulip/QuaZIPFacade.h>

#include <fcntl.h>
#include <tulip/TlpQtTools.h>

using namespace std;
using namespace tlp;

class PluginInformationsCollector : public PluginLoader {
public:
  virtual void loaded(const tlp::AbstractPluginInfo* info, const std::list< Dependency >& dependencies) {
    QString pluginName = QString::fromStdString(info->getName());
    QString pluginLibrary;
    std::string pluginType;

    for(std::map<std::string, PluginListerInterface*>::const_iterator it = PluginListerInterface::allFactories->begin(); it != PluginListerInterface::allFactories->end(); ++it) {
      PluginListerInterface* currentLister = it->second;

      if(currentLister->pluginExists(pluginName.toStdString())) {
        pluginLibrary = QString::fromStdString(currentLister->getPluginLibrary(pluginName.toStdString()));
        pluginType = currentLister->getPluginsClassName();
      }
    }

    //creates the xml document for this plugin
    QDomDocument pluginInfoDocument;
    QDomElement infoElement = pluginInfoDocument.createElement("plugin");
    infoElement.setTagName("plugin");
    infoElement.setAttribute("name", pluginName);
    infoElement.setAttribute("type", QString::fromStdString(pluginType));
    infoElement.setAttribute("author", QString::fromStdString(info->getAuthor()));
    infoElement.setAttribute("date", QString::fromStdString(info->getDate()));
    infoElement.setAttribute("info", QString::fromStdString(info->getInfo()));
    infoElement.setAttribute("fileName", pluginLibrary);
    infoElement.setAttribute("release", QString::fromStdString(info->getRelease()));
    infoElement.setAttribute("group", QString::fromStdString(info->getGroup()));
    infoElement.setAttribute("tulipRelease", QString::fromStdString(info->getTulipRelease()));

    for(std::list<Dependency>::const_iterator it = dependencies.begin(); it != dependencies.end(); ++it) {
      QDomElement dependencyElement = pluginInfoDocument.createElement("dependency");
      dependencyElement.setAttribute("name", QString::fromStdString(it->pluginName));
      dependencyElement.setAttribute("type", QString::fromStdString(it->factoryName));
      dependencyElement.setAttribute("version", QString::fromStdString(it->pluginRelease));
      infoElement.appendChild(dependencyElement);
    }

    _xmlElements.push_back(infoElement);
  }

  virtual void aborted(const std::string& plugin, const std::string& message) {
    std::cout << "failed to load plugin " << plugin << ": " << message << std::endl;
  }
  virtual void loading(const std::string&) {}
  virtual void finished(bool, const std::string&) {}
  virtual void start(const std::string&) {}

  const QList<QDomElement>& xmlElements() {
    return _xmlElements;
  }

private:
  QList<QDomElement> _xmlElements;
};

int main(int argc,char **argv) {
  //the plugin path is the root of the plugin folder. In the following example, it is the edgeseparation folder.
//   pluginserver/
//   ├── edgeseparation
//   │   ├── lib
//   │   │   └── tulip
//   │   │       └── libedgeseparation-4.0.0.so
//   │   └── share
//   │       └── doc
//   │           └── edgeseparation
//   │               └── index.html
//
  if(argc < 2) {
    std::cout << "packagePlugin pluginPath [destinationDir]" << std::endl;
    cout << "destinationDir defaults to the current dir" << endl;
    exit(0);
  }

  QString destinationDir;

  if(argc < 3) {
    destinationDir = QDir::currentPath();
  }
  else {
    destinationDir = argv[2];
    QDir destination(QDir::current());

    if(!destination.exists(destinationDir)) {
      destination.mkdir(destinationDir);
    }
  }

  initTulipLib();
  PluginInformationsCollector collector;
  QDir pluginServerDir(argv[1]);
  PluginListerInterface::currentLoader = &collector;

  QStringList libraries;
#if defined(_WIN32) || defined(_WIN64)
  libraries.push_back("*.dll");
#elif defined(__APPLE__)
  libraries.push_back("*.dylib");
#else
  libraries.push_back("*.so");
#endif

  foreach(const QString component, pluginServerDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
    QString plugindir = pluginServerDir.canonicalPath() + "/" + component;

    QDir pluginDirectory(plugindir);
    pluginDirectory.cd("lib");
    pluginDirectory.cd("tulip");
    foreach(const QString& pluginFile, pluginDirectory.entryList(libraries, QDir::Files | QDir::NoSymLinks)) {
      QString pluginFileName = pluginDirectory.canonicalPath() + "/" + pluginFile;
      PluginLibraryLoader::loadPluginLibrary(pluginFileName.toStdString(), &collector);
    }

    QString pluginSimplifiedName = component.simplified().remove(' ').toLower();

    QString archiveName = tlp::getPluginPackageName(pluginSimplifiedName);

    QDir pluginDir(destinationDir + "/");
    pluginDir.mkdir(pluginSimplifiedName);
    bool compressed = QuaZIPFacade::zipDir(plugindir, destinationDir + "/" + pluginSimplifiedName + "/" + archiveName);

    if(!compressed) {
      cout << "failed to compress folder " << plugindir.toStdString() << " as archive: " << destinationDir.toStdString() << "/archives/" << archiveName.toStdString() << endl;
    }
    else {
      std::cout << "created archive: " << destinationDir.toStdString() << "/" + pluginSimplifiedName.toStdString( )+ "/" << archiveName.toStdString() << std::endl;
    }

  }

  QDomDocument serverDescription;
  QDomElement description = serverDescription.createElement("server");
  serverDescription.appendChild(description);
  description.setAttribute("serverName", destinationDir);
  description.setAttribute("lastUpdate", QDateTime::currentDateTime().toString(Qt::ISODate));

  foreach(const QDomElement& element, collector.xmlElements()) {
    description.appendChild(element);
  }

  QFile outputXML(destinationDir + "/serverDescription.xml");
  outputXML.open(QIODevice::ReadWrite | QIODevice::Truncate);
  outputXML.write(serverDescription.toByteArray());
  outputXML.close();

  return 0;
}

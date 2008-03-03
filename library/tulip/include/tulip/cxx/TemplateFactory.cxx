//-*-c++-*-
/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 26/09/2001
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/
//====================================================================================
#include <iostream>
#include <tulip/StlIterator.h>

#include <tulip/Plugin.h>

template<class ObjectFactory, class ObjectType, class Context>
tlp::Iterator<std::string>* tlp::TemplateFactory<ObjectFactory, ObjectType, Context>::availablePlugins() {
  return new tlp::StlIterator<std::string,std::set<std::string>::const_iterator>(objNames.begin(), objNames.end());
}

template<class ObjectFactory, class ObjectType, class Context>
bool tlp::TemplateFactory<ObjectFactory, ObjectType, Context>::pluginExists(const std::string &pluginName) {
  return (objMap.find(pluginName) != objMap.end());
}

template<class ObjectFactory, class ObjectType, class Context>
void tlp::TemplateFactory<ObjectFactory,ObjectType,Context>::registerPlugin(ObjectFactory *objectFactory) {
  std::string pluginName = objectFactory->getName();
  objNames.insert(pluginName);
  objMap[pluginName]=objectFactory;
  ObjectType *withParam=objectFactory->createPluginObject(Context());
  objParam[pluginName] = withParam->getParameters();
  // loop over dependencies
  // to demangle the class names
  std::list<tlp::Dependency> dependencies = withParam->getDependencies();
  std::list<tlp::Dependency>::iterator itD = dependencies.begin();
  for (; itD != dependencies.end(); itD++) {
    const char *factoryDepName = (*itD).factoryName.c_str();
    (*itD).factoryName = std::string(tlp::demangleTlpClassName(factoryDepName));
  }
  objDeps[pluginName] = dependencies;
  objRels[pluginName] = objectFactory->getRelease();
  if (currentLoader!=0) currentLoader->loaded(
					      pluginName,
					      objectFactory->getAuthor(),
					      objectFactory->getDate(),
					      objectFactory->getInfo(),
					      objectFactory->getRelease(),
					      objectFactory->getTulipRelease(),
					      dependencies
					      );
}

template<class ObjectFactory, class ObjectType, class Context>
void tlp::TemplateFactory<ObjectFactory,ObjectType,Context>::removePlugin(const std::string &name) {
  objNames.erase(name);
  objMap.erase(name);
  objParam.erase(name);
  objDeps.erase(name);
  objRels.erase(name);
}

template<class ObjectFactory, class ObjectType, class Context>
ObjectType * tlp::TemplateFactory<ObjectFactory,ObjectType,Context>::getPluginObject(const std::string& name, Context c) {
  typename ObjectCreator::iterator it;
  it=objMap.find(name);
  if (it!=objMap.end()) return (*it).second->createPluginObject(c);
  return 0;
}

template<class ObjectFactory, class ObjectType, class Context>
tlp::StructDef tlp::TemplateFactory<ObjectFactory,ObjectType,Context>::getPluginParameters(std::string name) {
  assert(objMap.find(name)!=objMap.end());
  return objParam[name];
}

template<class ObjectFactory, class ObjectType, class Context>
std::string tlp::TemplateFactory<ObjectFactory,ObjectType,Context>::getPluginRelease(std::string name) {
  assert(objMap.find(name)!=objMap.end());
  return objRels[name];
}

template<class ObjectFactory, class ObjectType, class Context>
std::list<tlp::Dependency> tlp::TemplateFactory<ObjectFactory,ObjectType,Context>::getPluginDependencies(std::string name) {
  assert(objMap.find(name)!=objMap.end());
  return objDeps[name];
}

template<class ObjectFactory, class ObjectType, class Context> std::string tlp::TemplateFactory<ObjectFactory,ObjectType,Context>::getPluginsClassName() {
  return std::string(demangleTlpClassName(typeid(ObjectType).name()));
}

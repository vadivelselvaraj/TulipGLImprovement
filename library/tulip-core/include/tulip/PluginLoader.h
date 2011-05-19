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
#ifndef TLP_PLUGINLOADER
#define TLP_PLUGINLOADER
#include <list>
#include <string>
#include <tulip/WithDependency.h>

namespace tlp {

class AbstractPluginInfo;

struct TLP_SCOPE PluginLoader {
  virtual ~PluginLoader() {}
  virtual void start(const std::string &path)=0;
  virtual void numberOfFiles(int) {}
  virtual void loading(const std::string &filename)=0;
  virtual void loaded(const AbstractPluginInfo* infos, const std::list <Dependency>& dependencies)=0;
  virtual void aborted(const std::string &filename,const  std::string &errormsg)=0;
  virtual void finished(bool state,const std::string &msg)=0;
};

}
#endif

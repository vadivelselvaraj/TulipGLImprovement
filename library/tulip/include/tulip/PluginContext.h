//-*-c++-*-
/*
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef _PLUGINCONTEXT
#define _PLUGINCONTEXT

#include "tulip/Reflect.h"

namespace tlp {

class Graph;
class PropertyInterface;
class PluginProgress;

/**
 * \defgroup plugins
 */ 
/*@{*/

///Parameter for calling clustering plugins
class AlgorithmContext {
  public :
  ///
  Graph *graph;
  PluginProgress *pluginProgress;
  DataSet *dataSet;
  ///
  AlgorithmContext ():graph(0),pluginProgress(0), dataSet(0) {}
  ///
  ~AlgorithmContext(){}
};

///Parameter for calling Property plugins
class PropertyContext {
  public :
  ///
  Graph *graph;
  PropertyInterface *propertyProxy;
  PluginProgress *pluginProgress;
  DataSet *dataSet;
  ///
  PropertyContext ():graph(0),propertyProxy(0),pluginProgress(0),dataSet(0){}
  ///
  ~PropertyContext(){}
};
/*@}*/

}
#endif









//-*-c++-*-
#ifndef _CLUSTERING_H
#define _CLUSTERING_H

#include <list>
#include <string>
#include "tulip/Graph.h"
#include "tulip/Reflect.h"
#include "tulip/WithParameter.h"
#include "tulip/WithDependency.h"
#include "tulip/PluginProgress.h"
#include "tulip/Plugin.h"
#include "tulip/TemplateFactory.h"

/**
 * \addtogroup plugins
 */ 
namespace tlp {

/*@{*/
///Interface for general algorithm plug-ins
class Algorithm : public WithParameter, public WithDependency
{ 
public :
  Algorithm (AlgorithmContext context):graph(context.graph),pluginProgress(context.pluginProgress),dataSet(context.dataSet){}
  virtual ~Algorithm() {}
  virtual bool run() = 0;
  virtual bool check(std::string &) {return true;}
  virtual void reset() {}  

protected:
  Graph *graph;
  PluginProgress *pluginProgress;
  DataSet *dataSet;
};

class AlgorithmFactory:public Plugin{
public:
  static TLP_SCOPE TemplateFactory<AlgorithmFactory, Algorithm,AlgorithmContext > *factory;
  static void initFactory() {
    if (!factory) {
      factory = new TemplateFactory<AlgorithmFactory, Algorithm,AlgorithmContext >;
    }
  }
  virtual ~AlgorithmFactory() {}
  virtual Algorithm * createPluginObject(AlgorithmContext)=0;
  virtual  std::string getMajor() const {
    return tlp::getMajor(getRelease());
  }
  virtual  std::string getMinor() const  {
    return tlp::getMinor(getRelease());
  }
  virtual  std::string getTulipMajor() const {
    return tlp::getMajor(getTulipRelease());
  }
  virtual  std::string getTulipMinor() const  {
    return tlp::getMinor(getTulipRelease());
  }
};
/*@}*/

}
#endif







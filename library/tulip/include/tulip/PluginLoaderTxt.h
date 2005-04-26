#ifndef TLP_PLUGINLOADERTXT
#define TLP_PLUGINLOADERTXT
#include "PluginLoader.h"
struct PluginLoaderTxt:public PluginLoader {
  virtual void start(const std::string &path,const std::string &type);
  virtual void numberOfFile(int nbFile);
  virtual void loading(const std::string &filename);
  virtual void loaded(const std::string &name,
		      const std::string &author,
		      const std::string &date, 
		      const std::string &info,
		      const std::string &release,
		      const std::string &version);
  virtual void aborted(const std::string &filename,const  std::string &erreurmsg);
  virtual void finished(bool state,const std::string &msg);
};

#endif

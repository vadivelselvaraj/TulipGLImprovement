#ifndef TLP_PROGRESS
#define TLP_PROGRESS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <tulip/tulipconf.h>
#include <string>

namespace tlp {

enum ProgressState{TLP_CONTINUE, TLP_CANCEL , TLP_STOP };

class TLP_SCOPE PluginProgress {
 public:
  PluginProgress();
  virtual ~PluginProgress();
  ProgressState progress(int step, int max_step);
  void cancel();
  void stop();
  bool isPreviewMode() const ;
  void setPreviewMode(bool);
  virtual void showPreview(bool);
  ProgressState state() const;
  std::string getError() { return _error; }
  void setError(std::string error) { _error = error; }
  virtual void setComment(std::string msg) {}
  virtual void progress_handler(int step, int max_step);
 private:
  ProgressState _state;
  bool _preview;
  std::string _error;
};

}
#endif

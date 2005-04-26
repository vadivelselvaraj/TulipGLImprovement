//-*-c++-*-
#ifndef Tulip_LongestPath_H
#define Tulip_LongestPath_H

#include <string>
#include <tulip/TulipPlugin.h>
///
using namespace std;
///
class LongestPath:public Selection { 
public:
  LongestPath(const PropertyContext &);
  ~LongestPath();
  bool run();
  bool check(string&);

private:
  void setNodeValue(node);

private:
  double max;
  MetricProxy *metricLevel;
};

#endif





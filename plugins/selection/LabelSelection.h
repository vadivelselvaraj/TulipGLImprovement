//-*-c++-*-
#ifndef Tulip_LabelSelection_H
#define Tulip_LabelSelection_H
#include <string>
#include <tulip/TulipPlugin.h>

class LabelSelection:public Selection { 
public:
  LabelSelection(const PropertyContext &);
  ~LabelSelection();
  bool check(std::string &errMsg);
  bool getNodeValue(node n);

private:
  StringProxy *stringProxy;
  std::vector<std::string> *searchStrings;
  bool searchType;
  std::vector<std::string> searchStringsDefault;
};

#endif





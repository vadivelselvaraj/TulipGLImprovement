//-*-c++-*-
/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef _PATHLENGTHMETRIC_H
#define _PATHLENGTHMETRIC_H

#include <tulip/TulipPlugin.h>

class PathLengthMetric:public Metric
{ 
public:
  PathLengthMetric(const PropertyContext &);
  ~PathLengthMetric();
  double getNodeValue(const node n);
  bool check(std::string &);
};

#endif

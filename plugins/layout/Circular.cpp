/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#include <math.h>
#include "Circular.h"

LAYOUTPLUGIN(Circular,"Circular","David Auber","06/12/1999","Ok","0","1");

using namespace std;

Circular::Circular(const PropertyContext &context):Layout(context){}

Circular::~Circular() {}

bool Circular::run() {
  Size resultSize;
  double gamma=0;
  int i;
  double rayon;
  double sizeTm;

  layoutProxy->setAllEdgeValue(vector<Coord>(0));

  if (superGraph->numberOfNodes()<2) {
    rayon=0;
    resultSize.set(1,1,1);
  }
  else {
    //compute circonference
    Iterator<node> *itN=superGraph->getNodes();
    while(itN->hasNext()) {
      node itn=itN->next();
      layoutProxy->setNodeValue(itn,Coord(rayon*cos(gamma*i)+rayon*2,rayon*sin(gamma*i)+rayon*2,0));
      ++i;
    } delete itN;



    rayon=100;
    gamma=2*M_PI/superGraph->numberOfNodes();
    double x1,y1,x2,y2;
    i=0;x1=rayon*cos(gamma*i)+rayon*2;y1=rayon*sin(gamma*i)+rayon*2,0;i=1;
    x2=rayon*cos(gamma*i)+rayon*2;y2=rayon*sin(gamma*i)+rayon*2,0;
    sizeTm=sqrt( (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    sizeTm=sizeTm/2;
    resultSize.set(sizeTm,sizeTm,sizeTm);
  }
  superGraph->getLocalProperty<SizesProxy>("viewSize")->setAllNodeValue(resultSize);
  resultSize.set(sizeTm/16,sizeTm/16,sizeTm/4);
  superGraph->getLocalProperty<SizesProxy>("viewSize")->setAllEdgeValue(resultSize);
  i=0;
  Iterator<node> *itN=superGraph->getNodes();
  while(itN->hasNext()) {
    node itn=itN->next();
    layoutProxy->setNodeValue(itn,Coord(rayon*cos(gamma*i)+rayon*2,rayon*sin(gamma*i)+rayon*2,0));
    ++i;
  } delete itN;
  return true;
}




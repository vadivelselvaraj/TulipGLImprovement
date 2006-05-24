#include <cmath>
#include <tulip/TreeTest.h>
#include "TreeLeaf.h"


LAYOUTPLUGINOFGROUP(TreeLeaf,"Tree Leaf","David Auber","01/12/1999","ok","0","1","Tree");

using namespace std;

int TreeLeaf::dfsPlacement(node n,int &curPos,int depth) {
  int resultMin=0;
  int resultMax=0;
  int result=0;
  if (superGraph->outdeg(n)==0) {
    curPos+=2;
    Coord tmpC;
    tmpC.set(curPos,depth,0);
    layoutProxy->setNodeValue(n,tmpC);
    return curPos;
  }
  Iterator<node> *itN=superGraph->getOutNodes(n);
  if (itN->hasNext()) {
    node itn=itN->next();
    result=dfsPlacement(itn,curPos,depth+2);
    resultMin=result;
    resultMax=result;
  }
  for (;itN->hasNext();) {
    node itn=itN->next();
    result=dfsPlacement(itn,curPos,depth+2);
    if (result>resultMax)
      resultMax=result;
    if (result<resultMin)
      resultMin=result;
  }
  delete itN;
  result=(resultMin+resultMax)/2;
  layoutProxy->setNodeValue(n,Coord(result,depth,0));
  return result;
}

TreeLeaf::TreeLeaf(const PropertyContext &context):Layout(context){}

TreeLeaf::~TreeLeaf() {}

bool TreeLeaf::run() {
  superGraph->getLocalProperty<SizesProxy>("viewSize")->setAllNodeValue(Size(1,1,1));
  superGraph->getLocalProperty<SizesProxy>("viewSize")->setAllEdgeValue(Size(0.125,0.125,0.5));
  Iterator<node> *itN=superGraph->getNodes();
  node tmpNode;
  for (;itN->hasNext();) {
    node itn=itN->next();
    if (superGraph->indeg(itn)==0) {
      tmpNode=itn;
      break;
    }
  } delete itN;
  int x=0;
  dfsPlacement(tmpNode,x,0);
  return true;
}

bool TreeLeaf::check(string &erreurMsg) {
  if (TreeTest::isTree(superGraph)) {
    erreurMsg="";
    return true;
  }
  else {
    erreurMsg="The Graph must be a Tree";
    return false;
  }
}

void TreeLeaf::reset()
{}













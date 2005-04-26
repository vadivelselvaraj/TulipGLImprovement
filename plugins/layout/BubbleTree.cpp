#include <map>
#include <cmath>
#include <climits>
#include <fstream>
#include <qmessagebox.h>
#include <tulip/Circle.h>
#include <tulip/TreeTest.h>
#include <tulip/TlpTools.h>
#include "BubbleTree.h"

LAYOUTPLUGIN(BubbleTree,"Bubble Tree","D.Auber/S.Grivet","16/05/2003","Stable","1","1");

using namespace std;
using namespace stdext;
using namespace tlp;

struct greaterRadius {
  const std::vector<double> &radius;
  greaterRadius(const std::vector<double> &r):radius(r) {}
  bool operator()(unsigned i1,unsigned i2) const {
    return radius[i1]>radius[i2];
  }
};

double BubbleTree::computeRelativePosition(node n, hash_map<node,Vector<double, 5 > > *relativePosition) {

  Size tmpSizeFather = nodeSize->getNodeValue(n);
  tmpSizeFather[2] = 0; //remove z-coordiantes because the drawing is 2D
  double sizeFather= tmpSizeFather.norm() / 2.0;
  double sizeVirtualNode=1.0;
  /*
   * Iniatilize node position
   */  
  (*relativePosition)[n][0]=0;
  (*relativePosition)[n][1]=0;
  /*
   * Special case if the node is a leaf.
   */
  if (superGraph->outdeg(n)==0) {
    (*relativePosition)[n][2]=0;
    (*relativePosition)[n][3]=0;
    Size tmpSizeNode = nodeSize->getNodeValue(n);
    tmpSizeNode[2] = 0;
    (*relativePosition)[n][4] = tmpSizeNode.norm() / 2.0;
    return (*relativePosition)[n][4];
  }
  /*
   * Recursive call to obtain the set of radius of the children of n
   * A node is dynamically inserted in the neighborhood of n in order to
   * reserve space for the connection of the father of n
   */
  unsigned int Nc = superGraph->outdeg(n)+1;
  vector<double> angularSector(Nc);
  std::vector<double> realCircleRadius(Nc);
  realCircleRadius[0] = sizeVirtualNode;
  double sumRadius = sizeVirtualNode;

  Iterator<node> *itN=superGraph->getOutNodes(n);
  for (unsigned int i=1;itN->hasNext();++i)  {
    node itn = itN->next();
    realCircleRadius[i]= computeRelativePosition(itn,relativePosition);
    sumRadius += realCircleRadius[i];
  } delete itN;


  double resolution=0;

  if (nAlgo) {
    std::vector<double> subCircleRadius(Nc);
    subCircleRadius[0] = realCircleRadius[0];
    double maxRadius = sizeVirtualNode;
    unsigned int maxRadiusIndex=0;
    for (unsigned int i=0; i<Nc; ++i)  {
      subCircleRadius[i] = realCircleRadius[i];
      if (maxRadius < subCircleRadius[i]) {
	maxRadius = subCircleRadius[i];
	maxRadiusIndex = i;
      }
    }
    if ( maxRadius > (sumRadius/2.0)) {
      double ratio = (maxRadius/(sumRadius - maxRadius));
      for (unsigned int i=0; i<Nc; ++i) {
	if (i!=maxRadiusIndex) 
	  subCircleRadius[i] *= ratio;
      }
      sumRadius=2.0*maxRadius;
    }
    for (unsigned int i=0;i<Nc;++i) {
      angularSector[i] = 2*M_PI*subCircleRadius[i]/sumRadius;
    }
  } 
  else {
    resolution = 2.0*M_PI;
    std::vector<unsigned> index(Nc);
    for(unsigned i=0; i<Nc; ++i)
      index[i]=i;
    sort(index.begin(), index.end(), greaterRadius(realCircleRadius));
    std::vector<unsigned>::const_iterator  i=index.begin();
    for (;i!=index.end();++i) {
      double radius = realCircleRadius[*i];
      double angleMax = 2.0*asin(radius/(radius+sizeFather));
      double angle = radius*resolution / sumRadius;
      if (angle>angleMax) {
	angularSector[*i] = angleMax;
	sumRadius -= radius;
	resolution -= angleMax;
      } else break;
    }
    if (i!=index.end()) {
      for (;i!=index.end(); ++i) {
	double radius = realCircleRadius[*i];
	double angle = radius*resolution/sumRadius;
	angularSector[*i] = angle;
      }
      resolution = 0; 
    } else 
      resolution /= Nc;
  }
  
  double angle = 0;
  vector<tlp::Circle<double> > circles(Nc);
  for (unsigned int i=0; i<Nc; ++i) {
    tlp::Vector<double,2> point;
    double packRadius= realCircleRadius[i]/sin(angularSector[i]/2.0);
    packRadius >?= sizeFather+realCircleRadius[i];
    if (i>0)
      angle+=(angularSector[i-1]+angularSector[i])/2.0+resolution;
    circles[i][0] = packRadius*cos(angle);
    circles[i][1] = packRadius*sin(angle);
    circles[i].radius = realCircleRadius[i];
  }
  Circle<double> circleH=tlp::enclosingCircle(circles);
  (*relativePosition)[n][2] = -circleH[0];
  (*relativePosition)[n][3] = -circleH[1];
  (*relativePosition)[n][4] = sqrt(circleH.radius*circleH.radius - circleH[1]*circleH[1])-fabs(circleH[0]);
  /*
   * Set relative position of all children
   * according to the center of the enclosing circle
   */
  itN=superGraph->getOutNodes(n);
  for (unsigned int i=1;i<Nc;++i) {
    node itn = itN->next();
    (*relativePosition)[itn][0] = circles[i][0]-circleH[0];
    (*relativePosition)[itn][1] = circles[i][1]-circleH[1];
  } delete itN;
  return circleH.radius;
}

void BubbleTree::calcLayout2(node n, hash_map<node,Vector<double, 5 > > *relativePosition,
				 const Vector<double,3> &enclosingCircleCenter, 
				 const Vector<double,3> &originNodePosition) {
  /*
   * Make rotation around the center of the enclosing circle in order to align :
   * the virtual node, the enclosing circle' center and the grand father of the node. 
   */
  Vector<double,3> bend,zeta,zetaOriginal;
  bend.fill(0);
  bend[0] = (*relativePosition)[n][4];

  zeta[0] = (*relativePosition)[n][2];
  zeta[1]=(*relativePosition)[n][3];
  zeta[2]=0;
  zetaOriginal = zeta;

  Vector<double,3> vect,vect3;
  vect = originNodePosition-enclosingCircleCenter;
  vect /= vect.norm();
  vect3 = zeta+bend;
  vect3 /= vect3.norm();

  double cosAlpha,sinAlpha;
  cosAlpha = (vect3.dotProduct(vect));
  sinAlpha = (vect^vect3)[2];

  Vector<double,3> rot1,rot2;
  rot1[0] = cosAlpha;rot1[1]=-sinAlpha;rot1[2]=0;
  rot2[0] = sinAlpha;rot2[1]=cosAlpha;rot2[2]=0;
  zeta = rot1*zeta[0]+rot2*zeta[1];
  
  layoutProxy->setNodeValue(n, Coord(enclosingCircleCenter[0]+zeta[0],
				     enclosingCircleCenter[1]+zeta[1],
				     0) );
  /*
   * Place bend on edge to prevent overlaping
   */
  if(superGraph->outdeg(n)>0) {
    bend += zetaOriginal;
    bend = rot1*bend[0]+rot2*bend[1];
    bend += enclosingCircleCenter;
    Vector<double,3> a = enclosingCircleCenter+zeta-bend;
    Vector<double,3> b = originNodePosition-bend;
    a /= a.norm();
    b /= b.norm();
    if ((1.0-fabs(a.dotProduct(b)))>0.001) {
	Iterator<edge> *itE=superGraph->getInEdges(n);
	edge ite=itE->next();
	delete itE;
	vector<Coord>tmp(1);
	tmp[0]=Coord(bend[0],bend[1],0);
	layoutProxy->setEdgeValue(ite,tmp);
    }
  }
  /*
   * Make the recursive call, to place the children of n.
   */
  Iterator<node> *it=superGraph->getOutNodes(n);
  while (it->hasNext()) {
    node itn = it->next();
    Vector<double,3> newpos;
    newpos[0] = (*relativePosition)[itn][0];
    newpos[1] = (*relativePosition)[itn][1];
    newpos[2] = 0;
    newpos = rot1*newpos[0]+rot2*newpos[1];
    newpos += enclosingCircleCenter;
    calcLayout2(itn, relativePosition, newpos, enclosingCircleCenter+zeta);
  } delete it;
}

void BubbleTree::calcLayout(node n, hash_map<node,Vector<double, 5 > > *relativePosition) {
  /*
   * Make the recursive call, to place the children of n.
   */
 layoutProxy->setNodeValue(n,Coord(0,0,0));
  Iterator<node> *it=superGraph->getOutNodes(n);
  while (it->hasNext()) {
    node itn=it->next();
    Coord newpos((*relativePosition)[itn][0]-(*relativePosition)[n][2],
		 (*relativePosition)[itn][1]-(*relativePosition)[n][3], 0);
    Vector<double,3> origin,tmp;
    origin[0] = (*relativePosition)[itn][0]-(*relativePosition)[n][2];
    origin[1] = (*relativePosition)[itn][1]-(*relativePosition)[n][3];
    origin[2] = 0;
    tmp.fill(0);
    calcLayout2(itn, relativePosition, origin, tmp);
  } delete it;
}

namespace {
  const char * paramHelp[] = {
    // nodeSize
    HTML_HELP_OPEN() \
    HTML_HELP_DEF( "type", "SizeProxy" ) \
    HTML_HELP_DEF( "values", "An existing size property" ) \
    HTML_HELP_DEF( "default", "viewSize" ) \
    HTML_HELP_BODY() \
    "This parameter defines the property used for node's sizes." \
    HTML_HELP_CLOSE(),
    //Complexity
    HTML_HELP_OPEN() \
    HTML_HELP_DEF( "type", "bool" ) \
    HTML_HELP_DEF( "values", "[true, false] o(nlog(n)) / o(n)" ) \
    HTML_HELP_DEF( "default", "true" ) \
    HTML_HELP_BODY() \
    "This parameter enables to choose the complexity of the algorithm." \
    HTML_HELP_CLOSE()
  };
}

BubbleTree::BubbleTree(const PropertyContext &context):Layout(context) {
  addParameter<SizesProxy>("nodeSize",paramHelp[0],"viewSize");
  addParameter<bool>("complexity",paramHelp[1],"true");
}

BubbleTree::~BubbleTree() {}

bool BubbleTree::run() {
  if ( dataSet==0 || !dataSet->get("nodeSize",nodeSize)) {
    if (superGraph->existProperty("viewSize"))
      nodeSize = superGraph->getProperty<SizesProxy>("viewSize");    
    else {
      nodeSize = superGraph->getProperty<SizesProxy>("viewSize");  
      nodeSize->setAllNodeValue(Size(1.0,1.0,1.0));
    }
  }
  bool result=false;
  if (dataSet==0 || !dataSet->get("complexity",nAlgo))
    nAlgo = true;

  layoutProxy->setAllEdgeValue(vector<Coord>(0));
  stdext::hash_map<node,Vector<double,5> > relativePosition;
  node startNode;
  tlp::getSource(superGraph, startNode);
  computeRelativePosition(startNode,&relativePosition);
  calcLayout(startNode,&relativePosition);
  return true;
}

bool BubbleTree::check(string &erreurMsg) {
  if (TreeTest::isTree(superGraph)) {
    erreurMsg = "";
    return true;
  }
  else {
    erreurMsg = "The Graph must be a Tree";
    return false;
  }
}

void BubbleTree::reset() {}

#include <cmath>
#include <iostream>
#include <iomanip>

#include <tulip/TulipPlugin.h>
#include <tulip/PropertyProxy.h>

using namespace std;

void printFloat(ostream &os, const string &name , float f){
  float a=floor(f);
  float b=(f-a)*1000;
  if (b<0) b*=-1;
  os << name << (int)a << "." << (int)b << endl; 
}

void printCoord(ostream &os,const Coord &v){
  printFloat(os,"x ",v.getX());
  printFloat(os,"y ",v.getY());
  printFloat(os,"z ",v.getZ());
}
void printPoint(ostream &os,const Coord &v){ 
  os << "point [" << endl;
  printCoord(os,v);
  os << "]" << endl;
}
void printSize(ostream &os,const Size &v){
  printFloat(os,"h ",v.getW());
  printFloat(os,"w ",v.getH());
  printFloat(os,"d ",v.getD());
}

/** \addtogroup export */
/*@{*/
struct GML:public ExportModule {
  GML(ClusterContext context):ExportModule(context)
  {}

  ~GML(){}

  string convert(string tmp) {
    string newStr;
    for (unsigned int i=0;i<tmp.length();i++) {
      if (tmp[i]=='\"')
	newStr+="\\\"";
      else
	newStr+=tmp[i];
    }
    return newStr;
  }

  bool exportGraph(ostream &os,SuperGraph *currentGraph) {

    os << "graph [" << endl;
    os << "directed 1" << endl;
    os << "version 2" << endl;

    LayoutProxy *layout = currentGraph->getProperty<LayoutProxy>("viewLayout");
    StringProxy *label = currentGraph->getProperty<StringProxy>("viewLabel");
    //    IntProxy *shape =getProperty<IntProxy>(currentGraph->getPropertyManager(),"viewShape");
    ColorsProxy *colors = currentGraph->getProperty<ColorsProxy>("viewColor");    
    SizesProxy  *sizes = currentGraph->getProperty<SizesProxy>("viewSize");  
    //Save Nodes
    Iterator<node> *itN=currentGraph->getNodes();
    if (itN->hasNext())  {

      for (;itN->hasNext();) {
	node itn=itN->next();
	os << "node [" << endl;
	os << "id "<< itn.id << endl ;
	os << "label \"" << convert(label->getNodeValue(itn)) << "\"" << endl;
	os << "graphics [" << endl;
	printCoord(os,layout->getNodeValue(itn));
	printSize(os,sizes->getNodeValue(itn));
	os << "type \"rectangle\"" << endl;
	os << "width 0.12" << endl;
	os << "fill \"#"<< hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(itn).getR()
	   << hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(itn).getG()
	   << hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(itn).getB() << "\""<< endl;

	//	    os << "outline \"#"<< hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(itn).getR()
	//	       << hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(itn).getG()
	//	       << hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(itn).getB() << "\""<< endl;

	os << "outline \"#000000\"" << endl;
	os << dec << setfill(' ') << setw(6) << "]" << endl;
	os << ']' << endl;
      }
    }
    delete itN;

    //Save edges
    Iterator<edge> *itE=currentGraph->getEdges();
    for (;itE->hasNext();)
      {
	edge ite=itE->next();
	os << "edge [" << endl;
	os << "source " << superGraph->source(ite).id << endl; 
	os << "target " << superGraph->target(ite).id << endl;
	os << "id " << ite.id << endl;
	os << "label \"" << label->getEdgeValue(ite) << "\"" << endl;
	os << "graphics [" << endl;
	os << "type \"line\"" << endl;
	os << "arrow \"last\"" << endl;
	os << "width 0.1" << endl;
	os << "Line [" << endl;
	vector<Coord> lcoord;
	vector<Coord>::const_iterator it;
	lcoord=layout->getEdgeValue(ite);
	if (!lcoord.empty())
	  {
	    node itn=superGraph->source(ite);
	    printPoint(os,layout->getNodeValue(itn));
	  }
	for (it=lcoord.begin();it!=lcoord.end();++it)
	  {
	    printPoint(os,*it);
	  }
	if (!lcoord.empty())
	  {
	    node itn=superGraph->target(ite);
	    printPoint(os,layout->getNodeValue(itn));
	  }
	os << "]" << endl;
	os << "]" << endl;
	os << "]" << endl;
      }
    delete itE;
    os << "]" << endl;
    return true;
  }
};
/*@}*/
EXPORTPLUGIN(GML,"GML","Auber David","31/07/2001","0","0","1")

#include <time.h>
#include <tulip/TulipPlugin.h>
#include <math.h>

using namespace std;



namespace {

  const char * paramHelp[] =
    {
      // minsize
      HTML_HELP_OPEN() \
      HTML_HELP_DEF( "type", "int" ) \
      HTML_HELP_DEF( "default", "10" ) \
      HTML_HELP_BODY() \
      "This parameter defines the minimal amount of node used to build the randomized tree." \
      HTML_HELP_CLOSE(),
      // maxsize
      HTML_HELP_OPEN() \
      HTML_HELP_DEF( "type", "int" ) \
      HTML_HELP_DEF( "default", "100" ) \
      HTML_HELP_BODY() \
      "This parameter defines the maximal amount of node used to build the randomized tree." \
      HTML_HELP_CLOSE(),
      // maxdegree
      HTML_HELP_OPEN() \
      HTML_HELP_DEF( "type", "int" ) \
      HTML_HELP_DEF( "default", "5" ) \
      HTML_HELP_BODY() \
      "This parameter defines the maximal degree of node used to build the randomized tree." \
      HTML_HELP_CLOSE(),
    };
}



struct RandomTreeGeneral:public ImportModule {
  RandomTreeGeneral(ClusterContext context):ImportModule(context) {
    addParameter<int>("minsize",paramHelp[0],"10");
    addParameter<int>("maxsize",paramHelp[1],"100");
    addParameter<int>("maxdegree",paramHelp[2],"5");
  }
  ~RandomTreeGeneral() {
  }
  bool buildNode(node n,unsigned int sizeM,int arityMax) {
    if (superGraph->numberOfNodes()>sizeM) return true;
    bool result=true;
    int randNumber=rand();
    int i = 0;
    while (randNumber < RAND_MAX/pow(2.0,1.0+i))
      ++i;
    i = i % arityMax;
    for (;i>0;i--) {
      node n1;
      n1=superGraph->addNode();
      superGraph->addEdge(n,n1);
      result= result && buildNode(n1,sizeM,arityMax);
    }
    return result;
  }

  bool import(const string &name) {
    srand(clock()); 

    int sizeMin  = 10;
    int sizeMax  = 100;
    int arityMax = 5;
    if (dataSet!=0) {
      dataSet->get("minsize", sizeMin);   
      dataSet->get("maxsize", sizeMax);
      dataSet->get("maxdegree", arityMax);
    }
    bool ok=true;
    int i=0;
    unsigned int nbTest=0;
    while (ok) {
      ++nbTest;
      if (nbTest%100 == 0) {
	if (pluginProgress->progress((i/100)%100,100)!=TLP_CONTINUE) break;
      }
      i++;
      superGraph->clear();
      node n=superGraph->addNode();
      ok=!buildNode(n,sizeMax,arityMax);
      ok=false;
      if (superGraph->numberOfNodes()<sizeMin) ok=true;
    }
    return pluginProgress->progress(100,100)!=TLP_CANCEL;
  }
};
IMPORTPLUGIN(RandomTreeGeneral,"Random General Tree","Auber","16/02/2001","0","0","1")

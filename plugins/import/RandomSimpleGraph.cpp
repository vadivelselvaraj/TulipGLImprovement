#include <time.h>
#include <tulip/TulipPlugin.h>

using namespace std;

struct edgeS {
  unsigned  source,target;
};

namespace std {
  template<>
  struct less<edgeS> {
    bool operator()(const edgeS &c,const edgeS &d) const {
      int cs,ct,ds,dt;
      if (c.source<=c.target) { 
	cs=c.source;ct=c.target;
      }
      else {
	ct=c.source;cs=c.target;
      }
      if (d.source<=d.target) {
	ds=d.source;dt=d.target;
      }
      else {
	dt=d.source;ds=d.target;
      }
      if (cs<ds) return true;
      if (cs>ds) return false;
      if (ct<dt) return true;
      if (ct>dt) return false;
      return false;
    }
  };
};


namespace
{

	const char * paramHelp[] =
	{
		// nodes
		HTML_HELP_OPEN() \
		HTML_HELP_DEF( "type", "int" ) \
		HTML_HELP_DEF( "default", "5" ) \
		HTML_HELP_BODY() \
		"This parameter defines the amount of node used to build the randomized graph." \
		HTML_HELP_CLOSE(),

		// edges
		HTML_HELP_OPEN() \
		HTML_HELP_DEF( "type", "int" ) \
		HTML_HELP_DEF( "default", "9" ) \
		HTML_HELP_BODY() \
		"This parameter defines the amount of edge used to build the randomized graph." \
		HTML_HELP_CLOSE(),
	};

}


struct RandomSimpleGraph:public ImportModule {
  RandomSimpleGraph(ClusterContext context):ImportModule(context) {
    addParameter<int>("nodes",paramHelp[0],"5");
    addParameter<int>("edges",paramHelp[1],"9");
  }
  ~RandomSimpleGraph(){}
  
  bool import(const string &name) {
    srand(clock());
    int nbNodes  = 5;
    int nbEdges  = 9;
    if (dataSet!=0) {
      dataSet->get("nodes", nbNodes);
      dataSet->get("edges", nbEdges);
    }

    int ite = nbNodes*nbEdges;
    int nbIteration = ite;
    set<edgeS> myGraph;
    while (ite>0) {
      if (ite%nbNodes==1) if (pluginProgress->progress(nbIteration-ite,nbIteration)!=TLP_CONTINUE) 
	return pluginProgress->state()!=TLP_CANCEL;
      edgeS tmp;
      tmp.source=rand()%nbNodes;
      tmp.target=rand()%nbNodes;
      while (tmp.source==tmp.target) {
	tmp.source=rand()%nbNodes;
	tmp.target=rand()%nbNodes;
      }
      if (myGraph.find(tmp)!=myGraph.end())
	myGraph.erase(tmp);
      else 
	if (myGraph.size()<nbEdges) myGraph.insert(tmp);
      ite--;
    }
    vector<node> tmpVect(nbNodes);
    for (int i=0; i<nbNodes; ++i) {
      tmpVect[i]=superGraph->addNode();
    }
    set<edgeS>::iterator it;
    for (it=myGraph.begin(); it!=myGraph.end(); ++it)   {
      superGraph->addEdge(tmpVect[(*it).source],tmpVect[(*it).target]);
    }
    return true;
  }
};
IMPORTPLUGIN(RandomSimpleGraph,"Random Simple Graph","Auber","16/06/2002","0","0","1")

#include <tulip/MetricProxy.h>
#include <tulip/ForEach.h>
#include <deque>
#include "ClusterMetric.h"

METRICPLUGIN(ClusterMetric,"Cluster","David Auber","26/02/2003","Beta","0","2");

using namespace std;

namespace {
  const char * paramHelp[] = {
    // property
    HTML_HELP_OPEN() \
    HTML_HELP_DEF( "type", "int" ) \
    HTML_HELP_DEF( "values", "[1 , |G|]" ) \
    HTML_HELP_DEF( "default", "1" ) \
    HTML_HELP_BODY() \
    "This parameter defines the maximal depth of a computed cluster." \
    HTML_HELP_CLOSE()
  };
}
//=================================================
ClusterMetric::ClusterMetric(const PropertyContext &context):Metric(context) {
  addParameter<unsigned int>("depth",paramHelp[0],"1");
}
//=================================================
void ClusterMetric::buildSubGraph(node n,node startNode,set<node> &selected,unsigned int depth) {
  deque<node> fifo;
  SuperGraph * graph = superGraph;
  MutableContainer<bool> visited;
  MutableContainer<unsigned int> distance;
  visited.setAll(false);
  distance.setAll(graph->numberOfNodes());
  fifo.push_back(n);
  visited.set(n.id, true);
  distance.set(n.id, 0);
  while(!fifo.empty()) {
    node current = fifo.front();
    fifo.pop_front();
    if (distance.get(current.id) < depth) {
      Iterator<node> *itN = graph->getInOutNodes(current);
      while(itN->hasNext()) {
	node itn = itN->next();
	if (!visited.get(itn.id)) {
	  fifo.push_back(itn);
	  selected.insert(itn);
	  visited.set(itn.id,true);
	  distance.set(itn.id, distance.get(current.id) + 1);
	}
      } delete itN;
    }
  }
}
//=================================================
double ClusterMetric::getEdgeValue(const edge e ) {
  node src = superGraph->source(e);
  node tgt = superGraph->target(e);
  double v1 = metricProxy->getNodeValue(src);
  double v2 = metricProxy->getNodeValue(tgt);
  if (v1*v1 + v2*v2 > 0)
    return 1.-fabs(v1 - v2)/sqrt(v1*v1 + v2*v2);
  return 0.;
}
//=================================================
double ClusterMetric::getNodeValue(const node n ) {
  set<node> reachableNodes;
  buildSubGraph(n,n,reachableNodes,maxDepth);
  double nbEdge=0; //e(N_v)*2$
  for (set<node>::iterator itSN=reachableNodes.begin();itSN!=reachableNodes.end();++itSN) {
    node itn=*itSN;
    Iterator<edge> *itE=superGraph->getInOutEdges(itn);
    while (itE->hasNext()) {
      edge ite=itE->next();
      node source=superGraph->source(ite);
      node target=superGraph->target(ite);
      if ( (reachableNodes.find(source)!=reachableNodes.end()) && 
	   (reachableNodes.find(target)!=reachableNodes.end())) {
	nbEdge++;
      }
    } delete itE;
  }
  
  double nNode= reachableNodes.size(); //$|N_v|$
  if (reachableNodes.size()>1) {
    double result = double(nbEdge)/(nNode*(nNode-1));
    return result; //$e(N_v)/(\frac{k*(k-1)}{2}}$
  }
  else
    return 0;
}
//=================================================
bool ClusterMetric::run() {
  cerr << __PRETTY_FUNCTION__ << endl;
  maxDepth=1;
  if (dataSet!=0) dataSet->get("depth",maxDepth);
  node n;
  forEach(n, superGraph->getNodes())
    metricProxy->setNodeValue(n, getNodeValue(n));
  edge e;
  forEach(e, superGraph->getEdges())
    metricProxy->setEdgeValue(e, getEdgeValue(e));
  return true;
}
//=================================================

#include "tulip/SuperGraph.h"
#include "tulip/AcyclicTest.h"
#include "tulip/SelectionProxy.h"
#include "tulip/StableIterator.h"

using namespace std;

#ifdef _WIN32 
#ifdef DLL_EXPORT
AcyclicTest * AcyclicTest::instance=0;
#endif
#else
AcyclicTest * AcyclicTest::instance=0;
#endif
//**********************************************************************
AcyclicTest::AcyclicTest(){
}
//**********************************************************************
bool AcyclicTest::isAcyclic(const SuperGraph *graph) {
  if (instance==0)
    instance = new AcyclicTest();

  if (instance->resultsBuffer.find((unsigned int)graph) == instance->resultsBuffer.end()) {
    instance->resultsBuffer[(unsigned int)graph] = acyclicTest(graph);
    graph->addObserver(instance);
  }
  
  return instance->resultsBuffer[(unsigned int)graph];
}
//**********************************************************************
void AcyclicTest::makeAcyclic(SuperGraph* graph,vector<edge> &reversed, vector<tlp::SelfLoops> &selfLoops) {
  if (AcyclicTest::isAcyclic(graph)) return;

  //replace self loops by three edges and two nodes.
  StableIterator<edge> itE(graph->getEdges());
  while (itE.hasNext()) {
    edge e = itE.next();
    if (graph->source(e) == graph->target(e)) {
      node n1 = graph->addNode();
      node n2 = graph->addNode();
      selfLoops.push_back(tlp::SelfLoops(n1 , n2 , 
					 graph->addEdge(graph->source(e), n1) , 
					 graph->addEdge(n1,n2) , 
					 graph->addEdge(graph->source(e), n2) , 
					 e ));
      graph->delEdge(e);
    }
  }

  //find obstruction edges
  reversed.clear();
  acyclicTest(graph, &reversed);
  cerr << "reversed : " << reversed.size() << endl;
  if (reversed.size() > graph->numberOfEdges() / 2) {
    cerr << "[Warning]: " << __FUNCTION__ << ", is not efficient" << endl;
  }

  vector<edge>::const_iterator it = reversed.begin();
  for (; it != reversed.end(); ++it)
    graph->reverse(*it);

  assert(AcyclicTest::isAcyclic(graph));
 }

//=================================================================
bool AcyclicTest::dfsAcyclicTest(const SuperGraph *graph, const node n, 
				 MutableContainer<bool> &visited, 
				 MutableContainer<bool> &finished,
				 vector<edge> *obstructionEdges) {
  visited.set(n.id,true);
  bool result = true;
  Iterator<edge> *it = graph->getOutEdges(n);
  while (it->hasNext()) {
    edge tmp = it->next();
    node nextNode = graph->target(tmp);
    if (visited.get(nextNode.id)) {
      if (!finished.get(nextNode.id)) {
	result = false;
	if (obstructionEdges != 0)
	  obstructionEdges->push_back(tmp);
	else {
	  break;
	}
      }
    }
    else {
      bool tmp = dfsAcyclicTest(graph, nextNode, visited, finished, obstructionEdges);
      result = tmp && result;
      if ((!result) && (obstructionEdges==0)) break;
    }
  } delete it;
  finished.set(n.id,true);
  return result;
}
//**********************************************************************
bool AcyclicTest::acyclicTest(const SuperGraph *graph, vector<edge> *obstructionEdges) {
  MutableContainer<bool> visited;
  MutableContainer<bool> finished;
  visited.setAll(false);
  finished.setAll(false);
  bool result = true;
  Iterator<node> *it = graph->getNodes();
  while (it->hasNext()) {
    node curNode=it->next();
    if (!visited.get(curNode.id)) {
      if (!dfsAcyclicTest(graph, curNode, visited, finished, obstructionEdges)) {
	result = false;
	if (obstructionEdges == 0) {
	  break;
	}
      }
    }
  } delete it;
  return result;
}
//**********************************************************************
void AcyclicTest::destroy(SuperGraph *graph) {
  graph->removeObserver(this);
  resultsBuffer.erase((unsigned int)graph);
}
//**********************************************************************
void AcyclicTest::reverseEdge(SuperGraph *graph,const edge e) {
  graph->removeObserver(this);
  resultsBuffer.erase((unsigned int)graph);
}
//**********************************************************************
void AcyclicTest::addEdge(SuperGraph *graph,const edge e) {
  if (resultsBuffer[(unsigned int)graph]==false) return;
  graph->removeObserver(this);
  resultsBuffer.erase((unsigned int)graph);
}
//**********************************************************************
void AcyclicTest::delEdge(SuperGraph *graph,const edge e) {
  if (resultsBuffer[(unsigned int)graph]==true) return;
  graph->removeObserver(this);
  resultsBuffer.erase((unsigned int)graph);
}
//**********************************************************************

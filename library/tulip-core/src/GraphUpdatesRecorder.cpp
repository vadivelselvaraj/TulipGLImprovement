/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include <tulip/GraphUpdatesRecorder.h>
#include <tulip/Graph.h>
#include <tulip/ForEach.h>
#include <tulip/GraphImpl.h>

using namespace std;
using namespace stdext;
using namespace tlp;

GraphUpdatesRecorder::GraphUpdatesRecorder(bool allowRestart) : GraphObserver(), PropertyObserver(),
#if !defined(NDEBUG)
  recordingStopped(true),
#endif
  updatesReverted(false),
  restartAllowed(allowRestart),
  newValuesRecorded(false), oldIdsState(NULL), newIdsState(NULL)
{}

GraphUpdatesRecorder::~GraphUpdatesRecorder() {
  deleteDeletedObjects();
  deleteValues(oldNodeValues);
  deleteValues(newNodeValues);
  deleteValues(oldEdgeValues);
  deleteValues(newEdgeValues);
  deleteDefaultValues(oldNodeDefaultValues);
  deleteDefaultValues(newNodeDefaultValues);
  deleteDefaultValues(oldEdgeDefaultValues);
  deleteDefaultValues(newEdgeDefaultValues);

  if (oldIdsState)
    delete oldIdsState;

  if (newIdsState)
    delete newIdsState;

  // delete 'sets' of added nodes
  IteratorValue* it =
    graphAddedNodes.findAllValues(NULL, false);

  while(it->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> ctnr;
    it->nextValue(ctnr);
    delete ctnr.value;
  } delete it;

  // delete 'sets' of deleted nodes
  it = graphDeletedNodes.findAllValues(NULL, false);
  while(it->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> ctnr;
    it->nextValue(ctnr);
    delete ctnr.value;
  } delete it;  

  // delete 'sets' of added edges
  it = graphAddedEdges.findAllValues(NULL, false);

  while(it->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> ctnr;
    it->nextValue(ctnr);
    delete ctnr.value;
  } delete it;

  // delete 'sets' of deleted edges
  it = graphDeletedEdges.findAllValues(NULL, false);
  while(it->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> ctnr;
    it->nextValue(ctnr);
    delete ctnr.value;
  } delete it;

  // delete ends of added edges
  it = addedEdgesEnds.findAllValues(NULL, false);
  
  while(it->hasNext()) {
    TypedValueContainer<std::pair<node, node>*> ctnr;
    it->nextValue(ctnr);
    delete ctnr.value;
  } delete it;

  // delete ends of deleted edges
  it = deletedEdgesEnds.findAllValues(NULL, false);
  while(it->hasNext()) {
    TypedValueContainer<std::pair<node, node>*> ctnr;
    it->nextValue(ctnr);
    delete ctnr.value;
  } delete it;
}

void GraphUpdatesRecorder::treatEvent(const Event& ev) {
  if (typeid(ev) == typeid(GraphEvent))
    GraphObserver::treatEvent(ev);
  else
    PropertyObserver::treatEvent(ev);
}

// delete the objects collected as to be deleted
void GraphUpdatesRecorder::deleteDeletedObjects() {
  TLP_HASH_MAP<Graph*, set<Graph *> >& subGraphsToDelete =
    updatesReverted ? addedSubGraphs : deletedSubGraphs;
  TLP_HASH_MAP<Graph*, set<PropertyRecord> >& propertiesToDelete =
    updatesReverted ? addedProperties : deletedProperties;

  // loop on sub graphs
  TLP_HASH_MAP<Graph*, set<Graph *> >::iterator itds =
    subGraphsToDelete.begin();

  while(itds != subGraphsToDelete.end()) {
    set<Graph*>::iterator its = (*itds).second.begin();
    set<Graph*>::iterator ite = (*itds).second.end();

    while(its != ite) {
      // avoid subgraphs deletion of graph to delete
      (*its)->clearSubGraphs();
      delete (*its);
      ++its;
    }

    ++itds;
  }

  // loop on properties
  TLP_HASH_MAP<Graph*, set<PropertyRecord> >::iterator itdp =
    propertiesToDelete.begin();

  while(itdp != propertiesToDelete.end()) {
    set<PropertyRecord>::iterator itp =  (*itdp).second.begin();
    set<PropertyRecord>::iterator ite = (*itdp).second.end();

    while(itp != ite) {
      delete ((*itp).prop);
      ++itp;
    }

    ++itdp;
  }
}

// clean up all the clones of properties
void GraphUpdatesRecorder::deleteValues(TLP_HASH_MAP<PropertyInterface*,
                                        RecordedValues>& values) {
  TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itnv =
    values.begin();

  while(itnv != values.end()) {
    // delete the clone
    if (itnv->second.values)
      delete itnv->second.values;

    if (itnv->second.defaultValues)
      delete itnv->second.defaultValues;

    ++itnv;
  }

  values.clear();
}

// delete all the DataMem referenced by a TLP_HASH_MAP
void GraphUpdatesRecorder::deleteDefaultValues(TLP_HASH_MAP<PropertyInterface*,
    DataMem*>& values) {
  TLP_HASH_MAP<PropertyInterface*, DataMem*>::iterator itv = values.begin();
  TLP_HASH_MAP<PropertyInterface*, DataMem*>::iterator itve = values.end();

  while(itv != itve) {
    delete itv->second;
    ++itv;
  }

  values.clear();
}

void GraphUpdatesRecorder::recordEdgeContainer(TLP_HASH_MAP<node, vector<edge> >& containers, GraphImpl* g, node n) {
  TLP_HASH_MAP<node, vector<edge> >::iterator itc =
    containers.find(n);

  if (itc == containers.end()) {
    // record edges of n
    vector<edge> vEdges;
    Iterator<edge>* it = g->storage.getInOutEdges(n);

    while(it->hasNext()) {
      vEdges.push_back(it->next());
    }

    delete it;
    containers[n] = vEdges;
  }
}

void GraphUpdatesRecorder::removeFromEdgeContainer(TLP_HASH_MAP<node, vector<edge> >& containers, edge e, node n) {
  TLP_HASH_MAP<node, vector<edge> >::iterator itc =
    containers.find(n);

  if (itc != containers.end()) {
    vector<edge>::iterator it = (*itc).second.begin();
    vector<edge>::iterator ite = (*itc).second.end();

    while(it != ite) {
      if ((*it) == e) {
        (*itc).second.erase(it);
        break;
      }

      ++it;
    }
  }
}

void GraphUpdatesRecorder::recordNewValues(GraphImpl* g) {
  assert(restartAllowed);

  if (!newValuesRecorded) {
    // from now on it will be done
    newValuesRecorded = true;

    // get ids memento
    GraphImpl* root = (GraphImpl*) g;
    assert(newIdsState == NULL);
    newIdsState = root->storage.getIdsMemento();
    // record new edges containers
    IteratorValue *itae = addedEdgesEnds.findAllValues(NULL, false);

    while(itae->hasNext()) {
      TypedValueContainer<std::pair<node, node>*> ends;
      itae->nextValue(ends);
      recordEdgeContainer(newContainers, root, ends.value->first);
      recordEdgeContainer(newContainers, root, ends.value->second);
    } delete itae;

    // record new properties default values & new values
    // loop on oldNodeDefaultValues
    TLP_HASH_MAP<PropertyInterface*, DataMem*>::iterator itdv =
      oldNodeDefaultValues.begin();

    while(itdv != oldNodeDefaultValues.end()) {
      PropertyInterface* p = itdv->first;
      newNodeDefaultValues[p] = p->getNodeDefaultDataMemValue();
      recordNewNodeValues(p);
      ++itdv;
    }

    // loop on oldNodeValues
    TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itov =
      oldNodeValues.begin();

    while(itov != oldNodeValues.end()) {
      PropertyInterface* p = itov->first;

      if (oldNodeDefaultValues.find(p) == oldNodeDefaultValues.end())
        recordNewNodeValues(p);

      ++itov;
    }

    // loop on updatedPropsAddedNodes
    TLP_HASH_MAP<PropertyInterface*, std::set<node> >::iterator itan =
      updatedPropsAddedNodes.begin();

    while(itan != updatedPropsAddedNodes.end()) {
      PropertyInterface* p = itan->first;
      TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itnv =
        newNodeValues.find(p);
      PropertyInterface*  nv;
      bool created = itnv == newNodeValues.end();
      bool hasNewValues = false;

      if (created)
        nv = p->clonePrototype(p->getGraph(), "");
      else
        nv = itnv->second.values;

      set<node>::iterator itn = (*itan).second.begin();
      set<node>::iterator itne = (*itan).second.end();

      while(itn != itne) {
        bool notDefault = nv->copy((*itn), (*itn), p, true);

        if (!hasNewValues)
          hasNewValues = notDefault;

        ++itn;
      }

      if (created) {
        if (hasNewValues)
          newNodeValues[p] = nv;
        else
          delete nv;
      }

      ++itan;
    }

    // loop on oldEdgeDefaultValues
    itdv = oldEdgeDefaultValues.begin();

    while(itdv != oldEdgeDefaultValues.end()) {
      PropertyInterface* p = itdv->first;
      newEdgeDefaultValues[p] = p->getEdgeDefaultDataMemValue();
      recordNewEdgeValues(p);
      ++itdv;
    }

    // loop on oldEdgeValues
    itov = oldEdgeValues.begin();

    while(itov != oldEdgeValues.end()) {
      PropertyInterface* p = itov->first;

      if (oldEdgeDefaultValues.find(p) == oldEdgeDefaultValues.end())
        recordNewEdgeValues(p);

      ++itov;
    }

    // loop on updatedPropsAddedEdges
    TLP_HASH_MAP<PropertyInterface*, std::set<edge> >::iterator iten =
      updatedPropsAddedEdges.begin();

    while(iten != updatedPropsAddedEdges.end()) {
      PropertyInterface* p = iten->first;
      TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itnv =
        newEdgeValues.find(p);
      PropertyInterface*  nv;
      bool created = itnv == newEdgeValues.end();
      bool hasNewValues = false;

      if (created)
        nv = p->clonePrototype(p->getGraph(), "");
      else
        nv = itnv->second.values;

      set<edge>::iterator ite = (*iten).second.begin();
      set<edge>::iterator itee = (*iten).second.end();

      while(ite != itee) {
        bool notDefault = nv->copy((*ite), (*ite), p, true);

        if (!hasNewValues)
          hasNewValues = notDefault;

        ++ite;
      }

      if (created) {
        if (hasNewValues)
          newEdgeValues[p] = nv;
        else
          delete nv;
      }

      ++iten;
    }

    // record graph attribute new values
    TLP_HASH_MAP<Graph*, DataSet>::iterator itav =
      oldAttributeValues.begin();

    while (itav != oldAttributeValues.end()) {
      Graph* g = itav->first;
      Iterator<pair<string, DataType*> > *itv = (*itav).second.getValues();
      const DataSet& gAttValues = g->getAttributes();
      DataSet& nAttValues = newAttributeValues[g];

      while(itv->hasNext()) {
        pair<string, DataType*> pval = itv->next();
        nAttValues.setData(pval.first, gAttValues.getData(pval.first));
      }

      delete itv;
      ++itav;
    }
  }
}

void GraphUpdatesRecorder::recordNewNodeValues(PropertyInterface* p) {
  assert(newNodeValues.find(p) == newNodeValues.end());
  PropertyInterface*  nv = p->clonePrototype(p->getGraph(), "");
  bool hasNewValues = false;

  // record updated nodes new values
  if  (oldNodeDefaultValues.find(p) != oldNodeDefaultValues.end()) {
    // loop on non default valuated nodes
    Iterator<node>* itn = p->getNonDefaultValuatedNodes();

    while(itn->hasNext()) {
      node n(itn->next());
      bool nonDefault = nv->copy(n, n, p, true);

      if (!hasNewValues)
        hasNewValues = nonDefault;
    }

    delete itn;
  }
  else {
    TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itp =
      oldNodeValues.find(p);

    if (itp != oldNodeValues.end()) {
      PropertyInterface* opv = (*itp).second.values;

      if (opv) {
        Iterator<node>* itov = opv->getNonDefaultValuatedNodes();

        while(itov->hasNext()) {
          node n(itov->next());
          bool nonDefault = nv->copy(n, n, p, true);

          if (!hasNewValues)
            hasNewValues = nonDefault;
        }

        delete itov;
      }

      MutableContainer<bool>* dv = (*itp).second.defaultValues;

      if (dv) {
        Iterator<unsigned int>* itov = dv->findAll(true);

        while(itov->hasNext()) {
          node n(itov->next());
          bool nonDefault = nv->copy(n, n, p, true);

          if (!hasNewValues)
            hasNewValues = nonDefault;
        }

        delete itov;
      }
    }
  }

  if (hasNewValues)
    newNodeValues[p] = RecordedValues(nv);
  else
    delete nv;
}

void GraphUpdatesRecorder::recordNewEdgeValues(PropertyInterface* p) {
  assert(newEdgeValues.find(p) == newEdgeValues.end());
  PropertyInterface*  nv = p->clonePrototype(p->getGraph(), "");
  bool hasNewValues = false;

  // record updated edges new values
  if  (oldEdgeDefaultValues.find(p) != oldEdgeDefaultValues.end()) {
    // loop on non default valuated edges
    Iterator<edge>* ite = p->getNonDefaultValuatedEdges();

    while(ite->hasNext()) {
      edge e(ite->next());
      bool nonDefault = nv->copy(e, e, p, true);

      if (!hasNewValues)
        hasNewValues = nonDefault;
    }

    delete ite;
  }
  else {
    TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itp =
      oldEdgeValues.find(p);

    if (itp != oldEdgeValues.end()) {
      PropertyInterface* opv = (*itp).second.values;

      if (opv) {
        Iterator<edge>* itov = opv->getNonDefaultValuatedEdges();

        while(itov->hasNext()) {
          edge e(itov->next());
          bool nonDefault = nv->copy(e, e, p, true);

          if (!hasNewValues)
            hasNewValues = nonDefault;
        }

        delete itov;
      }

      MutableContainer<bool>* dv = (*itp).second.defaultValues;

      if (dv) {
        Iterator<unsigned int>* itov = dv->findAll(true);

        while(itov->hasNext()) {
          edge e(itov->next());
          bool nonDefault = nv->copy(e, e, p, true);

          if (!hasNewValues)
            hasNewValues = nonDefault;
        }

        delete itov;
      }
    }
  }

  if (hasNewValues)
    newEdgeValues[p] = RecordedValues(nv);
  else
    delete nv;
}

void GraphUpdatesRecorder::startRecording(GraphImpl* g) {
  if (g->getSuperGraph() == g) {
    assert(oldIdsState == NULL);
    oldIdsState = ((GraphImpl*) g)->storage.getIdsMemento();
  }

  restartRecording(g);
}

void GraphUpdatesRecorder::restartRecording(Graph* g) {
#if !defined(NDEBUG)

  if (g->getSuperGraph() == g) {
    assert(recordingStopped);
    recordingStopped = false;
  }

#endif

  if (newValuesRecorded) {
    deleteValues(newNodeValues);
    deleteValues(newEdgeValues);
    deleteDefaultValues(newNodeDefaultValues);
    deleteDefaultValues(newEdgeDefaultValues);
    assert(newIdsState != NULL);
    delete newIdsState;
    newIdsState = NULL;
    newValuesRecorded = false;
  }

  g->addListener(this);

  // add self as a PropertyObserver for all previously
  // existing properties
  TLP_HASH_MAP<Graph*, set<PropertyRecord> >::iterator itp =
    addedProperties.find(g);
  set<PropertyRecord>*  newProps = NULL;

  if (itp != addedProperties.end())
    newProps = &((*itp).second);

  PropertyInterface* prop;
  forEach(prop, g->getLocalObjectProperties()) {
    if (newProps) {
      PropertyRecord p(prop,  prop->getName());

      if (newProps->find(p) != newProps->end())
        continue;
    }

    prop->addListener(this);
  }

  // add self as a GraphObserver for all previously
  // existing subgraphs
  TLP_HASH_MAP<Graph*, set<Graph*> >::iterator itg =
    addedSubGraphs.find(g);
  set<Graph*>* newSubGraphs = NULL;

  if (itg != addedSubGraphs.end())
    newSubGraphs = &((*itg).second);

  Graph* sg;
  forEach(sg, g->getSubGraphs()) {
    if (!newSubGraphs || newSubGraphs->find(sg) == newSubGraphs->end())
      restartRecording(sg);
  }
}

void GraphUpdatesRecorder::stopRecording(Graph* g) {
#if !defined(NDEBUG)

  if (g->getSuperGraph() == g) {
    assert(!recordingStopped);
    recordingStopped = true;
  }

#endif
  g->removeListener(this);
  PropertyInterface* prop;
  forEach(prop, g->getLocalObjectProperties())
  prop->removeListener(this);
  Graph* sg;
  forEach(sg, g->getSubGraphs())
  stopRecording(sg);
}

void GraphUpdatesRecorder::doUpdates(GraphImpl* g, bool undo) {
  assert(updatesReverted != undo);
  updatesReverted = undo;

  Observable::holdObservers();
  // loop on propsToDel
  TLP_HASH_MAP<Graph*,  set<PropertyRecord> >& propsToDel =
    undo ? addedProperties : deletedProperties;
  TLP_HASH_MAP<Graph*,  set<PropertyRecord> >::iterator itpg =
    propsToDel.begin();

  while(itpg != propsToDel.end()) {
    Graph* g = (Graph*) (*itpg).first;
    set<PropertyRecord>::iterator itp = (*itpg).second.begin();
    set<PropertyRecord>::iterator itpe = (*itpg).second.end();

    while(itp != itpe) {
      g->delLocalProperty((*itp).name);
      ++itp;
    }

    ++itpg;
  }

  // loop on subGraphsToDel
  TLP_HASH_MAP<Graph*, std::set<Graph*> >& subGraphsToDel =
    undo ? addedSubGraphs : deletedSubGraphs;
  TLP_HASH_MAP<Graph*, std::set<Graph*> >::iterator its =
    subGraphsToDel.begin();

  while(its != subGraphsToDel.end()) {
    Graph* g = (Graph*) (*its).first;
    set<Graph*>::iterator itg = (*its).second.begin();
    set<Graph*>::iterator itge = (*its).second.end();

    while(itg != itge) {
      Graph* sg = (*itg);
      // remove from list of subgraphs + notify observers
      g->notifyBeforeDelSubGraph(sg);
      g->removeSubGraph(sg);
      g->notifyAfterDelSubGraph(sg);
      sg->notifyDestroy();

      if (!undo) {
        // restore its subgraphs as subgraph of its supergraph
        // only if we are redoing its deletion
        Iterator<Graph *> *itss = sg->getSubGraphs();

        while (itss->hasNext()) {
          Graph* ssg = itss->next();
          g->restoreSubGraph(ssg);
        }

        delete itss;
      }

      ++itg;
    }

    ++its;
  }

  // loop on edgesToDel
  MutableContainer<GraphEltsRecord*>& edgesToDel =
    undo ? graphAddedEdges : graphDeletedEdges;
  // edges must be removed in the decreasing order of the graphs ids
  // because for a coherent observation of deleted edges
  // they must be first deleted from a graph before being deleted
  // from the supergraph
  std::set<GraphEltsRecord*> geSet;

  IteratorValue* itge = edgesToDel.findAllValues(NULL,false);
  while(itge->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> ctnr;
    itge->nextValue(ctnr);
    geSet.insert(ctnr.value);
  } delete itge;

  std::set<GraphEltsRecord*>::const_reverse_iterator itrse = geSet.rbegin();
  while(itrse != geSet.rend()) {
    GraphEltsRecord* ger = (*itrse);
    // loop on graph's recorded edges
    Iterator<unsigned int>* ite = ger->elts.findAll(true, true);
    while(ite->hasNext()) {
      edge e(ite->next());
      if (ger->graph->isElement(e))
	ger->graph->removeEdge(e);
    } delete ite;
    ++itrse;
  }

  // loop on nodesToDel
  MutableContainer<GraphEltsRecord*>& nodesToDel =
    undo ? graphAddedNodes : graphDeletedNodes;
  IteratorValue* itgn = nodesToDel.findAllValues(NULL, false);

  while(itgn->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> gnr;
    itgn->nextValue(gnr);

    // loop on graph's recorded nodes
    Iterator<unsigned int>* itn = gnr.value->elts.findAll(true, true);
    while(itn->hasNext()) {
      gnr.value->graph->removeNode(node(itn->next()));
    } delete itn;
  } delete itgn;

  // loop on subGraphsToAdd
  TLP_HASH_MAP<Graph*, std::set<Graph*> >& subGraphsToAdd =
    undo ? deletedSubGraphs : addedSubGraphs;
  its = subGraphsToAdd.begin();

  while(its != subGraphsToAdd.end()) {
    Graph* g = (Graph*) (*its).first;
    set<Graph*>::iterator itg = (*its).second.begin();
    set<Graph*>::iterator itge = (*its).second.end();

    while(itg != itge) {
      Graph* sg = *itg;
      // notify its addition
      g->notifyBeforeAddSubGraph(sg);
      // restore sg as subgraph of g
      g->restoreSubGraph(sg);
      // notify its addition
      g->notifyAfterAddSubGraph(sg);
      Iterator<Graph *> *itss = sg->getSubGraphs();

      // and sg subgraphs are no longer subgraphs of g
      while (itss->hasNext()) {
        Graph* ssg = itss->next();
        g->removeSubGraph(ssg);
        ssg->setSuperGraph(sg);
      }

      delete itss;

      ++itg;
    }

    ++its;
  }

  // loop on nodesToAdd
  MutableContainer<GraphEltsRecord*>&  nodesToAdd =
    undo ? graphDeletedNodes : graphAddedNodes;
  itgn = nodesToAdd.findAllValues(NULL, false);

  while(itgn->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> gnr;
    itgn->nextValue(gnr);

    // loop on graph's recorded nodes
    Iterator<unsigned int>* itn = gnr.value->elts.findAll(true, true);
    while(itn->hasNext()) {
      gnr.value->graph->restoreNode(node(itn->next()));
    } delete itn;
  } delete itgn;

  // now restore ids manager state
  // this is done before the loop on the edges to add
  // because of some assertion in debug mode
  // while calling the restoreEdge method
  g->storage.restoreIdsMemento(undo? oldIdsState : newIdsState);

  // loop on revertedEdges
  set<edge>::iterator itre = revertedEdges.begin();

  while(itre != revertedEdges.end()) {
    g->reverse((*itre));
    ++itre;
  }

  // loop on edgesEnds
  TLP_HASH_MAP<edge, pair<node, node> >& updatedEdgesEnds =
    undo ? oldEdgesEnds : newEdgesEnds;
  TLP_HASH_MAP<edge, pair<node, node> >::iterator itee =
    updatedEdgesEnds.begin();

  while(itee != updatedEdgesEnds.end()) {
    g->setEnds((*itee).first, (*itee).second.first, (*itee).second.second);
    ++itee;
  }

  // loop on containers
  TLP_HASH_MAP<node, vector<edge> >& containers =
    undo ? oldContainers : newContainers;
  TLP_HASH_MAP<node, vector<edge> >::iterator itc = containers.begin();

  while(itc != containers.end()) {
    g->storage.restoreAdj((*itc).first, (*itc).second);
    ++itc;
  }

  // loop on edgesToAdd
  MutableContainer<GraphEltsRecord*>& edgesToAdd =
    undo ? graphDeletedEdges : graphAddedEdges;
  MutableContainer<std::pair<node, node>*>& edgesEnds =
    undo ? deletedEdgesEnds : addedEdgesEnds;
  // edges must be restored in the increasing order of the graphs ids
  // because for a coherent observation of added edges
  // they must be first added to the supergraph before being added
  // to a graph
  geSet.clear();
  itge = edgesToAdd.findAllValues(NULL,false);

  while(itge->hasNext()) {
    TypedValueContainer<GraphEltsRecord*> ger;
    itge->nextValue(ger);
    geSet.insert(ger.value);
  } delete itge;

  std::set<GraphEltsRecord*>::const_iterator itse = geSet.begin();
  while(itse != geSet.end()) {
    GraphEltsRecord* ger = (*itse);
    // loop on graph's recorded edges
    Iterator<unsigned int>* ite = ger->elts.findAll(true, true);
    while(ite->hasNext()) {
      edge e(ite->next());
      std::pair<node, node>* eEnds = edgesEnds.get(e);
      ger->graph->restoreEdge(e, eEnds->first, eEnds->second);
    } delete ite;
    ++itse;
  }

  // loop on propsToAdd
  TLP_HASH_MAP<Graph*,  set<PropertyRecord> >& propsToAdd =
    undo ? deletedProperties : addedProperties;
  itpg = propsToAdd.begin();

  while(itpg != propsToAdd.end()) {
    Graph* g = (Graph*) (*itpg).first;
    set<PropertyRecord>::iterator itp = (*itpg).second.begin();
    set<PropertyRecord>::iterator itpe = (*itpg).second.end();

    while(itp != itpe) {
      g->addLocalProperty((*itp).name, (*itp).prop);
      ++itp;
    }

    ++itpg;
  }

  // loop on nodeDefaultValues
  TLP_HASH_MAP<PropertyInterface*, DataMem*>& nodeDefaultValues =
    undo ? oldNodeDefaultValues : newNodeDefaultValues;
  TLP_HASH_MAP<PropertyInterface*, DataMem*>::iterator itdv =
    nodeDefaultValues.begin();

  while(itdv != nodeDefaultValues.end()) {
    PropertyInterface* prop = itdv->first;
    prop->setAllNodeDataMemValue((*itdv).second);
    ++itdv;
  }

  // loop on edgeDefaultValues
  TLP_HASH_MAP<PropertyInterface*, DataMem*>& edgeDefaultValues =
    undo ? oldEdgeDefaultValues : newEdgeDefaultValues;
  itdv = edgeDefaultValues.begin();

  while(itdv != edgeDefaultValues.end()) {
    PropertyInterface* prop = itdv->first;
    prop->setAllEdgeDataMemValue((*itdv).second);
    ++itdv;
  }

  // loop on nodeValues
  TLP_HASH_MAP<PropertyInterface*, RecordedValues>& nodeValues =
    undo ? oldNodeValues : newNodeValues;
  TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itnv =
    nodeValues.begin();

  while(itnv != nodeValues.end()) {
    PropertyInterface* prop = itnv->first;
    PropertyInterface* nv = itnv->second.values;

    if (nv) {
      Iterator<node>* itv = nv->getNonDefaultValuatedNodes();

      while(itv->hasNext()) {
        node n(itv->next());
        prop->copy(n, n, nv);
      }

      delete itv;
    }

    MutableContainer<bool>* dv = itnv->second.defaultValues;

    if (dv) {
      Iterator<unsigned int>* itv = dv->findAll(true);
      DataMem* defaultValue = prop->getNodeDefaultDataMemValue();

      while(itv->hasNext()) {
        node n(itv->next());
        prop->setNodeDataMemValue(n, defaultValue);
      }

      delete defaultValue;
      delete itv;
    }

    ++itnv;
  }

  // loop on edgeValues
  TLP_HASH_MAP<PropertyInterface*, RecordedValues>& edgeValues =
    undo ? oldEdgeValues : newEdgeValues;
  TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator itev =
    edgeValues.begin();

  while(itev != edgeValues.end()) {
    PropertyInterface* prop = itev->first;
    PropertyInterface* nv = itev->second.values;

    if (nv) {
      Iterator<edge>* itv = nv->getNonDefaultValuatedEdges();

      while(itv->hasNext()) {
        edge e(itv->next());
        prop->copy(e, e, nv);
      }

      delete itv;
    }

    MutableContainer<bool>* dv = itev->second.defaultValues;

    if (dv) {
      Iterator<unsigned int>* itv = dv->findAll(true);
      DataMem* defaultValue = prop->getEdgeDefaultDataMemValue();

      while(itv->hasNext()) {
        edge e(itv->next());
        prop->setEdgeDataMemValue(e, defaultValue);
      }

      delete defaultValue;
      delete itv;
    }

    ++itev;
  }

  // loop on attribute values to restore
  TLP_HASH_MAP<Graph*, DataSet>& attValues =
    undo ? oldAttributeValues : newAttributeValues;
  TLP_HASH_MAP<Graph*, DataSet>::iterator itav = attValues.begin();

  while (itav != attValues.end()) {
    Graph* g = itav->first;
    Iterator<pair<string, DataType*> > *itv = (*itav).second.getValues();

    while(itv->hasNext()) {
      pair<string, DataType*> pval = itv->next();

      if (pval.second)
        g->getNonConstAttributes().setData(pval.first, pval.second);
      else
        g->getNonConstAttributes().remove(pval.first);
    }

    delete itv;
    ++itav;
  }

  Observable::unholdObservers();
}

bool GraphUpdatesRecorder::dontObserveProperty(PropertyInterface* prop) {
  if (!restartAllowed) {
    // check if nothing is yet recorded for prop
    if ((oldNodeDefaultValues.find(prop) == oldNodeDefaultValues.end()) &&
        (oldEdgeDefaultValues.find(prop) == oldEdgeDefaultValues.end()) &&
        (oldNodeValues.find(prop) == oldNodeValues.end()) &&
        (oldEdgeValues.find(prop) == oldEdgeValues.end()) &&
        (updatedPropsAddedNodes.find(prop) == updatedPropsAddedNodes.end()) &&
        (updatedPropsAddedEdges.find(prop) == updatedPropsAddedEdges.end())) {
      // prop is no longer observed
      prop->removeListener(this);
      // may be a newly added property
      PropertyRecord p(prop,  prop->getName());
      Graph* g = prop->getGraph();
      TLP_HASH_MAP<Graph*, set<PropertyRecord> >::iterator it =
        addedProperties.find(g);

      if (it != addedProperties.end() &&
          ((*it).second.find(p) != (*it).second.end()))
        // the property is no longer recorded
        (*it).second.erase(p);

      return true;
    }
  }

  return false;
}

bool GraphUpdatesRecorder::isAddedOrDeletedProperty(Graph* g,
    PropertyInterface *prop) {
  PropertyRecord p(prop,  prop->getName());
  TLP_HASH_MAP<Graph*, set<PropertyRecord> >::iterator it =
    addedProperties.find(g);

  if (it != addedProperties.end() &&
      ((*it).second.find(p) != (*it).second.end()))
    return true;

  it = deletedProperties.find(g);
  return it != deletedProperties.end() &&
         ((*it).second.find(p) != (*it).second.end());
}


void GraphUpdatesRecorder::addNode(Graph* g, node n) {
  GraphEltsRecord *gnr = graphAddedNodes.get(g->getId());

  if (gnr == NULL) {
    gnr = new GraphEltsRecord(g);
    graphAddedNodes.set(g->getId(), gnr);
  }
  gnr->elts.set(n, true);
  addedNodes.set(n, true);
}

void GraphUpdatesRecorder::addEdge(Graph* g, edge e) {
  GraphEltsRecord *ger = graphAddedEdges.get(g->getId());

  if (ger == NULL) {
    ger = new GraphEltsRecord(g);
    graphAddedEdges.set(g->getId(), ger);
  }

  ger->elts.set(e, true);
  addedEdgesEnds.set(e, new std::pair<node, node>(g->ends(e)));
}

void GraphUpdatesRecorder::delNode(Graph* g, node n) {
  GraphEltsRecord *gnr = graphAddedNodes.get(g->getId());

  if (gnr != NULL && gnr->elts.get(n)) {
    // remove n from graph's recorded nodes if it is a newly added node
    gnr->elts.set(n, false);
    // but don't remove it from addedNodes
    // to ensure further erasal from property will not
    // record a value as if it was a preexisting node
    return;
  }

  // insert n into graphDeletedNodes
  gnr = graphDeletedNodes.get(g->getId());

  if (gnr == NULL) {
    gnr = new GraphEltsRecord(g);
    graphDeletedNodes.set(g->getId(), gnr);
  }
  gnr->elts.set(n, true);

  // no need of the loop below because properties are observed too
  // loop on properties to save the node's associated values

  /*PropertyInterface* prop;
  forEach(prop, g->getLocalObjectProperties()) {
    beforeSetNodeValue(prop, n);
    }*/
  if (g == g->getSuperGraph())
    recordEdgeContainer(oldContainers, (GraphImpl*) g, n);
}

void GraphUpdatesRecorder::delEdge(Graph* g, edge e) {
  GraphEltsRecord* ger = graphAddedEdges.get(g->getId());

  // remove e from addedEdges if it is a newly added edge
  if (ger != NULL && ger->elts.get(e)) {
    ger->elts.set(e, false);
    // do not remove from addedEdgesEnds
    // to ensure further erasal from property will not
    // record a value as if it was a preexisting edge
    /* if (graphs.empty())
       addedEdges.erase(it); */
    // remove from revertedEdges if needed
    set<edge>::iterator itR = revertedEdges.find(e);

    if (itR != revertedEdges.end())
      revertedEdges.erase(itR);

    // remove edge from nodes newContainers if needed
    std::pair<node, node>* eEnds = addedEdgesEnds.get(e);
    removeFromEdgeContainer(newContainers, e, eEnds->first);
    removeFromEdgeContainer(newContainers, e, eEnds->second);
    return;
  }

  // insert e into graph's deleted edges
  ger = graphDeletedEdges.get(g->getId());

  if (ger == NULL) {
    ger = new GraphEltsRecord(g);
    graphDeletedEdges.set(g->getId(), ger);
  }

  if (deletedEdgesEnds.get(e) == NULL) {
    const pair<node, node> &eEnds = g->ends(e);
    if (g == g->getSuperGraph()) {
      // remove from revertedEdges if needed
      set<edge>::iterator it = revertedEdges.find(e);

      if (it != revertedEdges.end()) {
        revertedEdges.erase(it);
	deletedEdgesEnds.set(e, new std::pair<node, node>(eEnds.second, eEnds.first));
      }
      else {
	TLP_HASH_MAP<edge, pair<node, node> >::iterator ite =
	  oldEdgesEnds.find(e);

        if (ite == oldEdgesEnds.end())
	  deletedEdgesEnds.set(e, new std::pair<node, node>(eEnds));
        else {
	  deletedEdgesEnds.set(e, new std::pair<node, node>((*ite).second));
          // remove from oldEdgesEnds
          oldEdgesEnds.erase(ite);
          // remove from newEdgesEnds
          newEdgesEnds.erase(e);
        }
      }
    }
    else
      deletedEdgesEnds.set(e, new std::pair<node, node>(eEnds));
  }
  ger->elts.set(e, true);

  // no need of the loop below because properties are observed too
  // loop on properties

  /*PropertyInterface* prop;
  // loop on properties to save the edge's associated values
  forEach(prop, g->getLocalObjectProperties()) {
    beforeSetEdgeValue(prop, e);
    }*/
  if (g == g->getSuperGraph()) {
    // record source & target old containers
    const pair<node, node> &eEnds = g->ends(e);
    recordEdgeContainer(oldContainers, (GraphImpl*) g, eEnds.first);
    recordEdgeContainer(oldContainers, (GraphImpl*) g, eEnds.second);
  }
}

void GraphUpdatesRecorder::reverseEdge(Graph* g, edge e) {
  if (g == g->getSuperGraph()) {
    pair<node, node>* eEnds = addedEdgesEnds.get(e);

    // if it is a newly added edge revert its source and target
    if (eEnds != NULL) {
      node src = eEnds->first;
      eEnds->first = eEnds->second;
      eEnds->second = src;
      return;
    }

    TLP_HASH_MAP<edge, pair<node, node> >::iterator itne = newEdgesEnds.find(e);

    if (itne != newEdgesEnds.end()) {
      // revert ends of itne
      node src = (*itne).second.first;
      (*itne).second.first = (*itne).second.second;
      (*itne).second.second = src;
    }
    else {   // update reverted edges
      set<edge>::iterator it = revertedEdges.find(e);

      if (it != revertedEdges.end())
        revertedEdges.erase(it);
      else {
        revertedEdges.insert(e);
        // record source & target old containers
        const pair<node, node>& eEnds = g->ends(e);
        recordEdgeContainer(oldContainers, (GraphImpl*) g, eEnds.first);
        recordEdgeContainer(oldContainers, (GraphImpl*) g, eEnds.second);
      }
    }
  }
}

void GraphUpdatesRecorder::beforeSetEnds(Graph* g, edge e) {
  if (g == g->getSuperGraph() && oldEdgesEnds.find(e) == oldEdgesEnds.end()
      && addedEdgesEnds.get(e) == NULL) {
    pair<node, node> ends = g->ends(e);
    set<edge>::iterator it = revertedEdges.find(e);

    // if it is a reverted edge
    // remove it from the set
    if (it != revertedEdges.end()) {
      revertedEdges.erase(it);
      // revert ends of it
      node tgt = ends.first;
      ends.first = ends.second;
      ends.second = tgt;
    }
    else {
      // record source & target old containers
      recordEdgeContainer(oldContainers, (GraphImpl*) g, ends.first);
      recordEdgeContainer(oldContainers, (GraphImpl*) g, ends.second);
    }

    // add e old ends in oldEdgesEnds
    oldEdgesEnds[e] = ends;
  }
}

void GraphUpdatesRecorder::afterSetEnds(Graph* g, edge e) {
  if (g == g->getSuperGraph()) {
    const pair<node, node>& ends = g->ends(e);
    std::pair<node, node>* eEnds = addedEdgesEnds.get(e);

    // if it is a newly added edge update its source and target
    if (eEnds != NULL) {
      eEnds->first = ends.first;
      eEnds->second = ends.second;
      return;
    }

    // update new ends in newEdgesEnds
    newEdgesEnds[e] = ends;
  }
}

void GraphUpdatesRecorder::addSubGraph(Graph* g, Graph* sg) {
  TLP_HASH_MAP<Graph*, set<Graph*> >::iterator it =
    addedSubGraphs.find(g);

  if (it == addedSubGraphs.end()) {
    set<Graph*> subgraphs;
    subgraphs.insert(sg);
    addedSubGraphs[g] = subgraphs;
  }
  else
    addedSubGraphs[g].insert(sg);

  // no need to observe the newly added subgraph
}

void GraphUpdatesRecorder::delSubGraph(Graph* g, Graph* sg) {
  TLP_HASH_MAP<Graph*, set<Graph*> >::iterator it =
    addedSubGraphs.find(g);

  // remove sg from addedSubGraphs if it is a newly added subgraph
  if (it != addedSubGraphs.end() &&
      ((*it).second.find(sg) != (*it).second.end())) {
    (*it).second.erase(sg);
    // but set its subgraphs as added in its supergraph
    Iterator<Graph *> *itss = sg->getSubGraphs();

    while (itss->hasNext()) {
      Graph* ssg = itss->next();
      addSubGraph(g, ssg);
    }

    delete itss;
    return;
  }

  // insert sg into deletedSubGraphs
  it = deletedSubGraphs.find(g);

  if (it == deletedSubGraphs.end()) {
    set<Graph*> subgraphs;
    subgraphs.insert(sg);
    deletedSubGraphs[g] = subgraphs;
  }
  else
    deletedSubGraphs[g].insert(sg);

  // sg is no longer observed
  sg->removeListener(this);
  // but it must not be really deleted
  g->setSubGraphToKeep(sg);
}

void GraphUpdatesRecorder::addLocalProperty(Graph* g, const string& name) {
  PropertyRecord p(g->getProperty(name),  name);
  TLP_HASH_MAP<Graph*, set<PropertyRecord> >::iterator it =
    addedProperties.find(g);

  if (it == addedProperties.end()) {
    set<PropertyRecord>  props;
    props.insert(p);
    addedProperties[g] = props;
  }
  else
    addedProperties[g].insert(p);

  // no need to observe the newly added property
}

void GraphUpdatesRecorder::delLocalProperty(Graph* g, const string& name) {
  PropertyRecord p(g->getProperty(name),  name);
  TLP_HASH_MAP<Graph*, set<PropertyRecord> >::iterator it =
    addedProperties.find(g);

  // remove p from addedProperties if it is a newly added one
  if (it != addedProperties.end() && ((*it).second.find(p) != (*it).second.end())) {
    // the property is no longer recorded
    (*it).second.erase(p);
    return;
  }

  // insert p into deletedProperties
  it = deletedProperties.find(g);

  if (it == deletedProperties.end()) {
    set<PropertyRecord>  props;
    props.insert(p);
    deletedProperties[g] = props;
  }
  else
    deletedProperties[g].insert(p);

  // the property is no longer observed
  p.prop->removeListener(this);
}

void GraphUpdatesRecorder::beforeSetNodeValue(PropertyInterface* p, node n) {
  // dont record the old value if the default one has been changed
  if (oldNodeDefaultValues.find(p) != oldNodeDefaultValues.end())
    return;

  // don't record old values for newly added nodes
  if (addedNodes.get(n)) {
    if (!restartAllowed)
      return;
    else {
      if (p->getGraph()->isElement(n))
        updatedPropsAddedNodes[p].insert(n);
      else
        // n has been deleted in the whole graph hierarchy, so we don't
        // need to backup its property value in the next push as the node
        // does not belong to a graph anymore
        updatedPropsAddedNodes[p].erase(n);
    }

  }
  else {
    TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator it =
      oldNodeValues.find(p);

    DataMem* value = p->getNonDefaultDataMemValue(n);

    if (it == oldNodeValues.end()) {
      if (value) {
        // it is not the default value
        PropertyInterface* pv = p->clonePrototype(p->getGraph(), "");
        pv->setNodeDataMemValue(n, value);
        oldNodeValues[p] = RecordedValues(pv);
        delete value;
      }
      else {
        MutableContainer<bool>* dv = new MutableContainer<bool>();
        dv->set(n, true);
        oldNodeValues[p] = RecordedValues(NULL, dv);
      }
    }
    else {
      if (value) {
        // it is not the default value
        if (!it->second.values)
          it->second.values = p->clonePrototype(p->getGraph(), "");

        it->second.values->setNodeDataMemValue(n, value);
        delete value;
      }
      else {
        if (!it->second.defaultValues)
          it->second.defaultValues = new MutableContainer<bool>();

        it->second.defaultValues->set(n, true);
      }
    }
  }
}

void GraphUpdatesRecorder::beforeSetAllNodeValue(PropertyInterface* p) {
  if  (oldNodeDefaultValues.find(p) == oldNodeDefaultValues.end()) {
    // first save the already existing value for all non default valuated nodes
    node n;
    forEach(n, p->getNonDefaultValuatedNodes())
      beforeSetNodeValue(p, n);
    // then record the old default value
    // because beforeSetNodeValue does nothing if it has already been changed
    oldNodeDefaultValues[p] = p->getNodeDefaultDataMemValue();
  }
}

void GraphUpdatesRecorder::beforeSetEdgeValue(PropertyInterface* p, edge e) {
  // dont record the old value if the default one has been changed
  if (oldEdgeDefaultValues.find(p) !=  oldEdgeDefaultValues.end())
    return;

  // dont record old value for newly added edge
  if (addedEdgesEnds.get(e)) {
    if (!restartAllowed)
      return;

    if (p->getGraph()->isElement(e))
      updatedPropsAddedEdges[p].insert(e);
    else {
      // e has been deleted in the whole graph hierarchy, so we don't
      // need to backup its property value in the next push as the edge
      // does not belong to a graph anymore
      updatedPropsAddedEdges[p].erase(e);
    }
  }
  else {
    TLP_HASH_MAP<PropertyInterface*, RecordedValues>::iterator it =
      oldEdgeValues.find(p);

    DataMem* value = p->getNonDefaultDataMemValue(e);

    if (it == oldEdgeValues.end()) {
      if (value) {
        // it is not the default value
        PropertyInterface* pv = p->clonePrototype(p->getGraph(), "");
        pv->setEdgeDataMemValue(e, value);
        oldEdgeValues[p] = RecordedValues(pv);
        delete value;
      }
      else {
        MutableContainer<bool>* dv = new MutableContainer<bool>();
        dv->set(e, true);
        oldEdgeValues[p] = RecordedValues(NULL, dv);
      }
    }
    else {
      if (value) {
        // it is not the default value
        if (!it->second.values)
          it->second.values = p->clonePrototype(p->getGraph(), "");

        it->second.values->setEdgeDataMemValue(e, value);
        delete value;
      }
      else {
        if (!it->second.defaultValues)
          it->second.defaultValues = new MutableContainer<bool>();

        it->second.defaultValues->set(e, true);
      }
    }
  }
}

void GraphUpdatesRecorder::beforeSetAllEdgeValue(PropertyInterface* p) {
  if (oldEdgeDefaultValues.find(p) == oldEdgeDefaultValues.end()) {
    // first save the already existing value for all non default valuated edges
    edge e;
    forEach(e, p->getNonDefaultValuatedEdges())
    beforeSetEdgeValue(p, e);
    // then record the old default value
    // because beforeSetEdgeValue does nothing if it has already been changed
    oldEdgeDefaultValues[p] = p->getEdgeDefaultDataMemValue();
  }
}

void GraphUpdatesRecorder::beforeSetAttribute(Graph* g,
    const std::string& name) {
  TLP_HASH_MAP<Graph*, DataSet>::iterator it =
    oldAttributeValues.find(g);

  if (it != oldAttributeValues.end() && it->second.exist(name))
    return;

  // save the previously existing value
  DataType* valType = g->getAttributes().getData(name);
  oldAttributeValues[g].setData(name, valType);
}

void GraphUpdatesRecorder::removeAttribute(Graph* g,
    const std::string& name) {
  beforeSetAttribute(g, name);
}

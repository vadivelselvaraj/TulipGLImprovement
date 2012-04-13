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

#if _MSC_VER
// disable Visual Studio warning about using this in base member initializer list
// this is not critical in our case as we only store a pointer
#pragma warning(disable: 4355)
#endif

#include <iostream>
#include <sstream>
#include <map>

#include <tulip/Observable.h>
#include <tulip/conversioniterator.h>
#include <tulip/filteriterator.h>

using namespace std;
using namespace tlp;

namespace {
//======================================================================
struct Node2Observable {
  Observable* operator()(tlp::node n) {
    return static_cast<Observable*>(object[n]);
  }
  tlp::NodeProperty<Observable *> object;
} node2Observable;
//======================================================================
struct Node2Onlooker {
  Observable* operator()(tlp::node n) {
    return static_cast<Observable*>(object[n]);
  }
  tlp::NodeProperty<Observable *> object;
} node2Onlooker;
//======================================================================
struct AliveFilter {
  bool operator()(tlp::node n) {
    return alive[n];
  }
  tlp::NodeProperty<bool> alive;
} objectAlive;
//======================================================================
template<unsigned int linkType>
struct LinkFilter {
  bool operator()(tlp::node n) {
    edge link(oGraph.existEdge(n, oNode));
    return (link.isValid() && (oType[link] & linkType));
  }

  LinkFilter(const VectorGraph& graph,
             const EdgeProperty<unsigned char>& type,
             node n): oGraph(graph), oType(type), oNode(n) {}

  const VectorGraph& oGraph;
  const EdgeProperty<unsigned char>& oType;
  node oNode;
};

}


namespace tlp {

//=================================
VectorGraph                 Observable::_oGraph;
unsigned int                Observable::_oNotifying   = 0;
unsigned int                Observable::_oUnholding   = 0;
unsigned int                Observable::_oHoldCounter = 0;
NodeProperty<Observable *>  Observable::_oPointer;
NodeProperty<bool >         Observable::_oAlive;
EdgeProperty<unsigned char> Observable::_oType;
std::set<std::pair<tlp::node, tlp::node> > Observable::_oDelayedEvents = std::set<std::pair<tlp::node, tlp::node> >();
vector<node>              Observable::_oDelayedDelNode;
bool                      Observable::_oInitialized = Observable::init();
bool                      Observable::eventQueued = false;

//----------------------------------
Iterator<node> * Observable::getInObjects() const {
  assert(_n.isValid());
  return new FilterIterator<node, AliveFilter>(_oGraph.getInNodes(_n), objectAlive);
}
//----------------------------------
Iterator<node> * Observable::getOutObjects() const {
  assert(_n.isValid());
  return new FilterIterator<node, AliveFilter>(_oGraph.getOutNodes(_n), objectAlive);
}
//----------------------------------
node Observable::getNode() const {
  return _n;
}
//----------------------------------
node Observable::getBoundNode() {
  if (_n.isValid() == false) {
    _n = _oGraph.addNode();
    _oPointer[_n] = this;
    _oAlive[_n] = true;
  }

  return _n;
}
//----------------------------------
unsigned int Observable::getSent() const {
#ifndef NDEBUG
  return sent;
#endif
  return 0;
}
//----------------------------------
unsigned int Observable::getReceived() const {
#ifndef NDEBUG
  return received;
#endif
  return 0;
}
//----------------------------------
bool Observable::isAlive(tlp::node n) {
  return _oAlive[n];
}
//----------------------------------
Observable* Observable::getObject(node n) {
  if (!_oAlive[n])
    throw ObservableException("That object has been deleted it is no more accessbile");

  return _oPointer[n];
}
//----------------------------------
bool Observable::init() {
  Observable::_oGraph.alloc(Observable::_oPointer);
  Observable::_oGraph.alloc(Observable::_oAlive);
  Observable::_oGraph.alloc(Observable::_oType);

  objectAlive.alive = Observable::_oAlive;
  node2Observable.object = Observable::_oPointer;
  node2Onlooker.object = Observable::_oPointer;
  return true;
}
//----------------------------------
const tlp::VectorGraph& Observable::getObservableGraph() {
  return Observable::_oGraph;
}
//=================================
Event::Event(const Observable &sender, EventType type):
  _sender(sender.getNode()),
  _type(type) {
  if (_type == TLP_DELETE)
    throw ObservableException("It is forbidden to create a delete events, DeleteEvents are autmotically generated at the observable destruction");
}
//----------------------------------
Event::~Event() {
}
//----------------------------------
Observable* Event::sender() const {
  return static_cast<Observable*>(Observable::getObject(_sender)); /** only Observable can be use to create event */
}
//=================================
// define a class for an empty Iterator of Observable *
class NoObservableIterator : public Iterator<Observable *> {
public:
  Observable* next() {
    return NULL;
  }
  bool hasNext() {
    return false;
  }
};
//=================================
Iterator<Observable *> *Observable::getObservables() const {
  if (isBound())
    return
      new ConversionIterator<node, Observable*,
      Node2Observable>(getOutObjects(), node2Observable);

  return new NoObservableIterator();
}
//=================================
void Observable::treatEvents(const  std::vector<Event> & ) {
  //qDebug() << __PRETTY_FUNCTION__ << " : not implemented";
  /*
  //this code is a compatibility layer for the old observation system.
  if (events[0].type() == Event::TLP_DELETE) {
  observableDestroyed(events[0].sender());
  }
  else {
  std::set<Observable*> observables;

  for(size_t k=0; k < events.size(); ++k) {
    observables.insert(events[k].sender());
  }

  update(observables.begin(), observables.end());
  }
  */
}
//=================================
//void Observable::update(std::set<Observable*>::iterator, std::set<Observable*>::iterator) {
//  qDebug() << __PRETTY_FUNCTION__ << " : not implemented";
//}
//=================================
void Observable::treatEvent(const Event &) {
  qDebug() << __PRETTY_FUNCTION__ << " : not implemented";
}
//=================================
/*void Observable::observableDestroyed(Observable *) {
  qDebug() << __PRETTY_FUNCTION__ << " : not implemented";
}*/
//=================================
Observable::Observable(): deleteMsgSent(false), queuedEvent(false), _n(node()) {
#ifndef NDEBUG
  sent = received = 0;
#endif
  //qDebug() << "[Observable node] created:" << n.id << "::" << this << endl;
}
//----------------------------------
Observable::Observable(const Observable &):deleteMsgSent(false), queuedEvent(false), _n(node()) {
#ifndef NDEBUG
  sent = received = 0;
#endif
  //qDebug() << "[Observable node] created (copy constructor):" << n.id << "::" << this << endl;
}
//----------------------------------
Observable& Observable::operator=(const Observable &) {
#ifndef NDEBUG
  qDebug() << "[Observable Warning]: Observable object should reimplement their operator= else nothing is copied" << endl;
#endif
  return *this;
}
//----------------------------------
Observable::~Observable() {
  if (_n.isValid() == false)
    return;

#ifdef _OPENMP
  #pragma omp critical(ObservableGraphUpdate)
#endif
  {
    if (!deleteMsgSent)
      observableDeleted();

    if (!_oAlive[_n])
      throw ObservableException("Observable object has already been deleted, possible double free!!!");

    //qDebug() << "[Observable node] destructor:" << n.id  << "::" << this << endl;
    _oAlive[_n] = false;

    if (_oNotifying == 0 && _oUnholding == 0 && _oHoldCounter == 0) {
      _oGraph.delNode(_n);
      //qDebug() << "[Observable node] deleted:" << n.id << "::" << this << endl;
    }
    else {
      _oDelayedDelNode.push_back(_n);
      //qDebug() << "[Observable node] delayed delete:" << n.id << "::" << this << endl;
      _oGraph.delEdges(_n);
    }
  }
}

//----------------------------------------
void Observable::holdObservers() {
#ifdef _OPENMP
  #pragma omp critical(ObservableGraphUpdate)
#endif
  ++_oHoldCounter;
}
//----------------------------------------
void Observable::unholdObservers() {
  {
    if (_oHoldCounter == 0)
      throw ObservableException("unhold call without a previous call to hold");

    --_oHoldCounter;
    {
      if (_oHoldCounter > 0 || _oDelayedEvents.empty() ) return;

      ++_oUnholding;
      ++_oHoldCounter; /** rehold the oberver to buffer message sent during unholding */

      set< pair<node, node> > backupEvents;
      backupEvents.swap(_oDelayedEvents);
      set< pair<node, node> >::const_iterator it;

      for( it = backupEvents.begin(); it != backupEvents.end(); ++it) {
        if (Observable::_oAlive[it->first]) {
          Observable *sender = static_cast<Observable *>(Observable::_oPointer[it->first]);
          sender->queuedEvent = false;
        }
      }

      map<node, vector<Event> > preparedEvents;

      for( it = backupEvents.begin(); it != backupEvents.end(); ++it) {
        if (Observable::_oAlive[it->first] && Observable::_oAlive[it->second]) {
          Observable *sender = static_cast<Observable *>(Observable::_oPointer[it->first]);
          preparedEvents[it->second].push_back(Event(*sender, Event::TLP_MODIFICATION));
        }
      }

      {
        map<node, vector<Event> >::const_iterator it;

        for (it = preparedEvents.begin(); it!=preparedEvents.end(); ++it) {
          if (_oAlive[it->first]) {
            Observable *obs  = static_cast<Observable *>(_oPointer[it->first]);
#ifndef NDEBUG
            ++(obs->received);
#endif
            obs->treatEvents(it->second);
          }
        }
      }

      -- _oUnholding;
      unholdObservers(); /** treat queued events during the unhold */
      updateObserverGraph();

      if (_oHoldCounter > 0) {
        throw ObservableException("Observable after unholdd call, bad nested hold/unhold function call in an Observer:: update() function");
      }
    }
  }
}
//----------------------------------------
Iterator<Observable *> *Observable::getOnlookers() const {
  if (isBound()) {
    if (!_oAlive[_n]) {
      throw ObservableException("getObservers called on a deleted Observable");
    }

    return new ConversionIterator<node, Observable*, Node2Onlooker>(getInObjects(), node2Onlooker);
  }

  return new NoObservableIterator();
}
//----------------------------------------
void Observable::addOnlooker(const Observable &obs, OBSERVABLEEDGETYPE type) const {
#ifdef _OPENMP
  #pragma omp critical(ObservableGraphUpdate)
#endif
  {
    if (isBound() && !_oAlive[_n]) {
      throw ObservableException("addObserver called on a deleted Observable");
    }

    // check for an existing link
    edge link;

    if (isBound() && obs.isBound())
      link = _oGraph.existEdge(obs.getNode(), getNode());

    if (!link.isValid()) {
      // add new link
      // at this time both Observables need to be bound
      link = _oGraph.addEdge(((Observable &) obs).getBoundNode(),
      ((Observable *) this)->getBoundNode());
      _oType[link] = type;
    }
    else {
#ifndef NDEBUG

      if (_oType[link] & type) {
        qWarning() << "[Observable Warning]: observer already connected";
      }

#endif
      // add the bit for the given type on the edge
      _oType[link] |= type;
    }
  }
}
//----------------------------------------
void Observable::addObserver(Observable * const obs) const {
  assert(obs != 0);
  addOnlooker(*obs, OBSERVER);
}
//----------------------------------------
void Observable::addListener(Observable * const obs) const {
  assert(obs != 0);
  addOnlooker(*obs, LISTENER);
}
//----------------------------------------
void Observable::observableDeleted() {
  if (deleteMsgSent) {
    throw ObservableException("Delete message has been sent several time.");
  }

  deleteMsgSent = true;

  if (hasOnlookers()) {
    Event msg(*this, Event::TLP_INVALID); //create a modify event to prevent raise exception, (hack) to forbid creation of Delete exception without calling that fucnton
    msg._type = Event::TLP_DELETE;
    sendEvent(msg);
  }
}
//----------------------------------------
void Observable::sendEvent(const Event &message) {
  if (!isBound())
    return;

  //cerr << "send event " << _oPointer[_n] << " " << message.type() << " indeg " << _oGraph.indeg(_n) << " outdeg: " << _oGraph.outdeg(_n) << endl;
  if (!_oGraph.isElement(_n) || !_oAlive[_n]) {
    throw ObservableException("Notify called on a deleted Observable");
  }

  const unsigned int RECCALL = 200;

  if (_oNotifying > RECCALL) {
    std::stringstream str;
    str << "Maximum number of nested calls (" << RECCALL << ") reached, contact tulip team if that limit is too restrictive";
    throw ObservableException(str.str());
    //return;
  }

  node backn = _n; /** to keep trace of the node if the observable is deleted during the notification, in that crazy case, (*this) is dead thus n is not accessible*/
  ++_oNotifying;
  //create two separate list of observer & listeners
  //could be extended if we need recorders
  vector< pair<Observable *, node> > observerTonotify;
  vector< pair<Observable *, node> > listenerTonotify;
  edge e;
  forEach(e, _oGraph.getInEdges(_n)) {
    node src(_oGraph.source(e));

    if (_oAlive[src]) {
      Observable *obs(static_cast<Observable *>(_oPointer[src]));
      assert(obs != NULL);

      if ((message.type() != Event::TLP_INFORMATION) && (_oHoldCounter == 0  || message.type() == Event::TLP_DELETE))
        if (_oType[e] & OBSERVER)
          observerTonotify.push_back(pair<Observable*, node>(obs, src));

      if (_oType[e] & LISTENER)
        listenerTonotify.push_back(pair<Observable*, node>(obs, src));

    }
  }

  if (message.type() == Event::TLP_MODIFICATION && _oHoldCounter!=0) {
    if (!queuedEvent) {
      queuedEvent = true;
      eventQueued = true;
      edge e;
      forEach(e, _oGraph.getInEdges(_n)) {
        if (_oType[e] & OBSERVER) {
          node src(_oGraph.source(e));

          if (_oAlive[src]) {
            _oDelayedEvents.insert(pair<node, node>(_n, src));
          }
        }
      }
    }
  }

  //send message to listeners
  vector< pair<Observable *, node> >::const_iterator itobs;

  for(itobs = listenerTonotify.begin(); itobs != listenerTonotify.end(); ++itobs) {
    if (itobs->second == _n && message.type() == Event::TLP_DELETE) {
      qDebug() << "[Observable info]: An observable onlook itself Event::DELETE msg can't be sent to it." << endl;
      continue;
    }

    if (_oAlive[itobs->second]) { //other listeners/observers could be destroyed during the treat event
#ifndef NDEBUG
      ++(itobs->first->received);
#endif
      itobs->first->treatEvent(message);
    }

    if (!_oAlive[backn]) {
      throw ObservableException("An observable has been deleted during the notifification of its observer (ie. an observer has deleted its caller during an update)");
    }
  }

  //send simple event to observers
  vector<Event> tmp(1, message);

  for(itobs = observerTonotify.begin(); itobs != observerTonotify.end(); ++itobs) {
    if (itobs->second == _n && message.type() == Event::TLP_DELETE) {
      qDebug() << "[Observable info]: An observable onlook itself Event::DELETE msg can't be sent to it." << endl;
      continue;
    }

    if (_oAlive[itobs->second]) { //other listeners/observers could be destroyed during the treat event
#ifndef NDEBUG
      ++(itobs->first->received);
#endif
      itobs->first->treatEvents(tmp);
    }

    if (!_oAlive[backn]) {
      throw ObservableException("An observable has been deleted during the notifification of its observer (ie. an observer has deleted its caller during an update)");
    }
  }

#ifndef NDEBUG
  ++sent;
#endif
  --_oNotifying;
  updateObserverGraph();

}
//----------------------------------------
void Observable::updateObserverGraph() {
  //qDebug() << __PRETTY_FUNCTION__ << endl << flush;
  if (_oNotifying == 0 && _oUnholding == 0 && _oHoldCounter == 0) {
    vector<node>::const_iterator itNodes;

    for( itNodes = _oDelayedDelNode.begin(); itNodes != _oDelayedDelNode.end(); ++itNodes) {
      _oGraph.delNode(*itNodes);
    }

    _oDelayedDelNode.clear();
  }
}
//----------------------------------------
void Observable::removeOnlooker(const Observable &obs, OBSERVABLEEDGETYPE type) const {
  // nothing to do if one of the observables is unbound
  if (!isBound() || !obs.isBound())
    return;

#ifdef _OPENMP
  #pragma omp critical(ObservableGraphUpdate)
#endif
  {
    if (!_oAlive[_n]) {
      throw ObservableException("removeOnlooker called on a deleted Observable");
    }

    edge link(_oGraph.existEdge(obs.getNode(), getNode()));

    if (link.isValid()) {
      _oType[link] = _oType[link] & ~type; //bitwise operation to remove the bit  for the given type on the edge

      if (_oType[link] == 0)
        _oGraph.delEdge(link);
    }
  }
}
//----------------------------------------
void Observable::removeObserver(Observable  * const obs) const {
  assert(obs != NULL);
  removeOnlooker(*obs, OBSERVER);
}
//----------------------------------------
void Observable::removeListener(Observable  * const obs) const {
  assert(obs != NULL);
  removeOnlooker(*obs, LISTENER);
}
//----------------------------------------
void Observable::notifyObservers() {
  if (!isBound())
    return;

  if (!_oAlive[_n]) {
    throw ObservableException("notifyObservers called on a deleted Observable");
  }

  if(hasOnlookers())
    sendEvent(Event(*this, Event::TLP_MODIFICATION));
}
//----------------------------------------
bool Observable::hasOnlookers() const {
  if (!isBound())
    return false;

  if (!_oAlive[_n]) {
    throw ObservableException("hasOnlookers called on a deleted Observable");
  }

  return _oGraph.indeg(_n) > 0;
}
//----------------------------------------

}

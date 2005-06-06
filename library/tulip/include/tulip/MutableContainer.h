#ifndef _TLPMUTABLECONTAINER_
#define _TLPMUTABLECONTAINER_

#include <iostream>
#if (__GNUC__ < 3)
#include <hash_map>
#else
#include <ext/hash_map>
#endif
#include <deque>
#include <exception>
#include <iostream>
#include <tulip/tulipconf.h>
#include <tulip/ReturnType.h>
#include <tulip/Iterator.h>


enum State { VECT=0, HASH=1 };

class ImpossibleOperation : public std::exception {
};

//===================================================================
template <typename TYPE> 
class IteratorVector : public Iterator<unsigned int> {
 public:
  IteratorVector(const TYPE &value, std::deque<TYPE> *vData, unsigned int minIndex):
    _value(value),
    _pos(minIndex),
    vData(vData),
    it(vData->begin()) {
    while (it!=(*vData).end() && (*it)!=_value) {
      ++it;
      ++_pos;
    }
  }
  bool hasNext() {
    return (_pos<UINT_MAX && it!=(*vData).end());
  }
  unsigned int next() {
    unsigned int tmp = _pos;
    do {
      ++it;
      ++_pos;
    } while (it!=(*vData).end() && !((*it)==_value));
    return tmp;
  }
 private:
  const TYPE _value;
  unsigned int _pos;
  std::deque<TYPE> *vData;
  typename std::deque<TYPE>::const_iterator it;
};
//===================================================================
template <typename TYPE> 
class IteratorHash : public Iterator<unsigned int> {
 public:
  IteratorHash(const TYPE &value, stdext::hash_map<unsigned int,TYPE> *hData):
    _value(value),
    hData(hData) {
    it=(*hData).begin();
    while (it!=(*hData).end() && (*it).second!=_value)
      ++it;
  }
  bool hasNext() {
    return (it!=(*hData).end());
  }
  unsigned int next() {
    unsigned int tmp = (*it).first;
    do {
      ++it;
    } while (it!=(*hData).end() && (*it).second!=_value);
    return tmp;
  }
 private:
  const TYPE _value;
  stdext::hash_map<unsigned int,TYPE> *hData;
  typename stdext::hash_map<unsigned int,TYPE>::const_iterator it;
};
//===================================================================
template <typename TYPE> 
class MutableContainer {
  friend class MutableContainerTest;
public:
  MutableContainer();
  ~MutableContainer();
  void setAll(const TYPE &value);
  void set(const unsigned int i,const TYPE &value);  
  /**
   * This function return a reference instead of a copy in order to minimize the
   * the number copy of objects, user must be awer that calling the set function can 
   * devalidate this reference.
   */
  const typename ReturnType<TYPE>::Value get(const unsigned int i) const;
  Iterator<unsigned int>* findAll(const TYPE &value) const throw (ImpossibleOperation) ;
  /**
   * This function is available only for optimisation purpose, one must be sure the 
   * the referenced element is not the default value. Use this function extremely carefully
   */
  TYPE & getReference(const unsigned int i);
private:
  MutableContainer(const MutableContainer<TYPE> &){}
  void operator=(const MutableContainer<TYPE> &){}
  void vecttohash();
  void hashtovect();
  void compress(unsigned int min, unsigned int max, unsigned int nbElements);
private:
  std::deque<TYPE> *vData;
  stdext::hash_map<unsigned int,TYPE> *hData;
  unsigned int minIndex,maxIndex;
  TYPE defaultValue;
  State state;
  unsigned int elementInserted;
  double ratio;
  bool compressing;
};
//===================================================================
//===================================================================
template<typename TYPE> 
MutableContainer<TYPE>::MutableContainer() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  state = VECT;
  vData = new std::deque<TYPE>();
  hData = 0;
  maxIndex = UINT_MAX;
  minIndex = UINT_MAX;
  elementInserted = 0;
  unsigned int phiP = sizeof(void *);
  unsigned int phi = sizeof(TYPE);
  ratio = double(phi) / (3.0*double(phiP)+double(phi));
  compressing = false;
}
//===================================================================
template <typename TYPE> 
MutableContainer<TYPE>::~MutableContainer() {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  switch (state) {
  case VECT: 
    delete vData; vData=0;
    break;
  case HASH:
    delete hData; hData=0;
    break;
  default:
    std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    break;  
  }
}
//===================================================================
template <typename TYPE> 
void MutableContainer<TYPE>::setAll(const TYPE &value) {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  switch (state) {
  case VECT: 
    delete vData;vData=0;
    break;
  case HASH:
    delete hData;hData=0;
    break;
  default:
    std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    break; 
  }
  defaultValue = value;
  state = VECT;
  vData = new std::deque<TYPE>();
  maxIndex = UINT_MAX;
  minIndex = UINT_MAX;
  elementInserted = 0;
}
//===================================================================
template <typename TYPE> 
Iterator<unsigned int>* MutableContainer<TYPE>::findAll(const TYPE &value) const throw (ImpossibleOperation)  {
  if (value == defaultValue) 
    throw ImpossibleOperation();
  else {
    switch (state) {
    case VECT: 
      return new IteratorVector<TYPE>(value, vData, minIndex);
      break;
    case HASH:
      return new IteratorHash<TYPE>(value, hData);
      break;
    default:
      std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
      return 0;
      break; 
    }
  }
}
//===================================================================
template <typename TYPE> 
void MutableContainer<TYPE>::set(const unsigned int i,const TYPE &value) {
  //  cerr << __PRETTY_FUNCTION__ << endl;

  //Test if after insertion we need to resize
  if (!compressing && value != defaultValue) {
    compressing = true;
    compress (i<?minIndex, i>?maxIndex, elementInserted);
    compressing = false;
  }
  
  typename stdext::hash_map<unsigned int,TYPE>::iterator it;
  if (value == defaultValue) {
    switch (state) {
    case VECT : 
      if (i<=maxIndex && i>=minIndex) {
	if ((*vData)[i - minIndex] != defaultValue) {
	  elementInserted--;
	  (*vData)[i - minIndex]= defaultValue;
	}
      }
      break;
    case HASH :
      if ((it=hData->find(i))!=hData->end()) {
	hData->erase(i);
	elementInserted--;
      }
      break;
    default:
      std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
      break; 
    }
  }
  else {
    switch (state) {
    case VECT :
      if (minIndex == UINT_MAX) {
	minIndex = i;
	maxIndex = i;
	(*vData).push_back(value);
	++elementInserted;
      }
      else {
	/*	if ( i > maxIndex ) {
	  (*vData).resize(i+1 - minIndex, defaultValue);
	  maxIndex = i;
	  }*/
	while ( i > maxIndex ) {
	  (*vData).push_back(defaultValue);
	  ++maxIndex;
	}
	while ( i < minIndex ) {
	  (*vData).push_front(defaultValue);
	  minIndex--;
	}
	if ((*vData)[i - minIndex] == defaultValue) ++elementInserted;
	(*vData)[i - minIndex] = value;
      }
      break;
    case HASH :
      if (hData->find(i)==hData->end()) ++elementInserted;
      (*hData)[i]= value;
      break;
    default:
      std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
      break; 
    }
    maxIndex >?= i;
    minIndex <?= i;
  }
}
//===================================================================
//const TYPE &  MutableContainer<TYPE>::get(unsigned int i) const {
template <typename TYPE>   
const typename ReturnType<TYPE>::Value MutableContainer<TYPE>::get(const unsigned int i) const {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  if (maxIndex == UINT_MAX) return defaultValue;
  typename stdext::hash_map<unsigned int,TYPE>::iterator it;
  switch (state) {
  case VECT:
    if (i>maxIndex || i<minIndex) 
      return defaultValue;
    else 
      return ((*vData)[i - minIndex]);
    break;
  case HASH:
    if ((it=hData->find(i))!=hData->end())
      return (*it).second;
    else
      return defaultValue;
    break;
  default:
    std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    return defaultValue;
    break;
  }
}
//===================================================================
//const TYPE &  MutableContainer<TYPE>::get(unsigned int i) const {
template <typename TYPE>   
TYPE & MutableContainer<TYPE>::getReference(const unsigned int i) {
  return (TYPE &) get(i);
}
//===================================================================
template <typename TYPE> 
void MutableContainer<TYPE>::vecttohash() {
  //  std::cerr << __FUNCTION__ << std::endl << std::flush;
  hData=new stdext::hash_map<unsigned int,TYPE>(elementInserted);

  unsigned int newMaxIndex = 0;
  unsigned int newMinIndex = UINT_MAX;
  elementInserted = 0;
  for (unsigned int i=minIndex; i<=maxIndex; ++i) {
    if ((*vData)[i - minIndex] != defaultValue) {
      (*hData)[i] = (*vData)[i - minIndex];
      newMaxIndex >?= i;
      newMinIndex <?= i;
      ++elementInserted;
    }
  }
  maxIndex = newMaxIndex;
  minIndex = newMinIndex;
  delete vData;vData = 0;
  state = HASH;
}
//===================================================================
template <typename TYPE> 
void MutableContainer<TYPE>::hashtovect() {
  //  std::cerr << __FUNCTION__ << std::endl << std::flush;
  vData = new std::deque<TYPE>();
  minIndex = UINT_MAX;
  maxIndex = UINT_MAX;
  elementInserted = 0;
  state=VECT;
  typename stdext::hash_map<unsigned int,TYPE>::const_iterator it;
  for (it=hData->begin(); it!=hData->end(); ++it) {
    if (it->second != defaultValue)
      set(it->first, it->second);
  }
  delete hData; hData = 0;
}
//===================================================================
template <typename TYPE> 
void MutableContainer<TYPE>::compress(unsigned int min, unsigned int max, unsigned int nbElements) {
  //  std::cerr << __PRETTY_FUNCTION__ << std::endl;
  if (max == UINT_MAX || (max - min) < 10) return;
  double limitValue = ratio*(double(max - min + 1.0));

  switch (state) {
  case VECT: 
    if ( double(nbElements) < limitValue) {
      /*
	std::cerr << "****************************************************" << std::endl;
	std::cerr << "min : " << min << " max :" << max << " lim : ";
	std::cerr  << limitValue << " el : " << elementInserted << std::endl;
	std::cerr << "minId : " << minIndex << " maxId :" << maxIndex << " lim : ";
	std::cerr  << limitValue << " el : " << elementInserted << std::endl;
      */
      vecttohash();
      /*
	std::cerr << "minId : " << minIndex << " maxId :" << maxIndex << " lim : ";
	std::cerr  << limitValue << " el : " << elementInserted << std::endl;
	std::cerr << "****************************************************" << std::endl;
	sleep(1);
      */
    }
    break;
  case HASH:
    if ( double(nbElements) > limitValue*1.5) {
      /*
	std::cerr << "****************************************************" << std::endl;
	std::cerr << "min : " << min << " max :" << max << " lim : ";
	std::cerr  << limitValue << " el : " << elementInserted << std::endl;
	std::cerr << "minId : " << minIndex << " maxId :" << maxIndex << " lim : ";
	std::cerr  << limitValue << " el : " << elementInserted << std::endl;
      */
      hashtovect();
      /*
	std::cerr << "minId : " << minIndex << " maxId :" << maxIndex << " lim : ";
	std::cerr  << limitValue << " el : " << elementInserted << std::endl;
	std::cerr << "****************************************************" << std::endl;
	sleep(1);
      */
    }
    break;
  default:
    std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    break; 
  }
}
//===================================================================
#endif

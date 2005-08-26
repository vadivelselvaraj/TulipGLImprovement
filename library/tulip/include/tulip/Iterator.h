//-*-c++-*-
/*
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/
#ifndef TULIP_ITERATOR_H
#define TULIP_ITERATOR_H

///Interface of Iterators
template<class itType> struct Iterator {
  ///
  Iterator(){}
  ///
  virtual ~Iterator(){};
  ///Return the next element
  virtual itType next()=0;
  ///Return true if it exist a next element
  virtual bool hasNext()=0;
};

#endif


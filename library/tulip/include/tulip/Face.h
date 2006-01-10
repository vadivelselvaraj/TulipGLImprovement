#ifndef Tulip_FACE_H
#define Tulip_FACE_H
#include "tulipconf.h"
#if (__GNUC__ < 3)
#include <hash_map>
#else
#include <ext/hash_map>
#endif
#include <climits>

/**
 * \defgroup graphs
 */ 
/*@{*/
/// class face
struct Face { 
  unsigned int id;
  Face():id(UINT_MAX){}
  explicit Face(unsigned int j):id(j){}
  bool operator!=(const Face f) const {return id!=f.id;}
  bool operator==(const Face f) const {return id==f.id;}
  bool isValid() const {return id!=UINT_MAX;}
};
/*@}*/

#ifndef DOXYGEN_NOTFOR_DEVEL

namespace stdext {
  template<> struct hash<Face> {
    size_t operator()(const Face f) const {return f.id;}
  };
}

namespace std {
  template<> struct equal_to<Face> {
    size_t operator()(const Face f,const Face f2) const {return f.id == f2.id;}
  };
  template<> struct less<Face>{
    size_t operator()(const Face f,const Face f2) const {return f.id < f2.id;}
  };
}
#endif // DOXYGEN_NOTFOR_DEVEL

#endif

#ifndef _FRAME_H_
#define _FRAME_H_
#ifndef DOXYGEN_NOTFOR_DEVEL

namespace tlp {

struct Frame{
  virtual void draw(float w_max, float& w) const=0;
  virtual void getBoundingBox(float w_max, float& h, float&w) const=0;
  virtual ~Frame()=0;
};

}

#endif //DOXYGEN_NOTFOR_DEVEL
#endif


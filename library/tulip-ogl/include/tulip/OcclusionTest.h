#ifndef Tulip_OCCLUSIONTEST_H
#define Tulip_OCCLUSIONTEST_H
#include <tulip/Rectangle.h>

struct RectangleInt2D : public tlp::Rectangle<int> {
  RectangleInt2D(int x,int y,int x1,int y1) {
    (*this)[0][0]=x;
    (*this)[0][1]=y;
    (*this)[1][0]=x1;
    (*this)[1][1]=y1;
  }
  RectangleInt2D(const RectangleInt2D &r):tlp::Rectangle<int>(r){}
};

struct OcclusionTest {
  std::vector<RectangleInt2D> data;
  void reset() {
    data.clear();
  }
  bool addRectangle(const RectangleInt2D &rec) {
    if (!testRectangle(rec)) {
      data.push_back(rec);
      return false;
    }
    return true;
  }
  bool testRectangle(const RectangleInt2D &rec) {
    bool intersect=false;
    std::vector<RectangleInt2D>::const_iterator it;
    for (it=data.begin();it!=data.end();++it) {
      if (rec.intersect(*it)) {
	intersect=true;
	break;
      }
    }
    return intersect;
  }

};

#endif

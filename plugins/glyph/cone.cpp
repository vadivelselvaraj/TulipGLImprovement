#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

#include <tulip/StringProxy.h>
#include <tulip/ColorsProxy.h>

#include <tulip/SuperGraph.h>
#include <tulip/GlGraph.h>
#include <tulip/Glyph.h>

using namespace std;

class Cone : public Glyph {
public:
  Cone(GlyphContext *gc=NULL);
  virtual ~Cone();
  virtual string getName() {return string("Cone");}
  virtual void draw(node n);
  virtual void setLOD(int n);
  virtual Coord Cone::getAnchor(const Coord &vector) const;

private:
  GLuint LList;
  bool listOk;
};

GLYPHPLUGIN(Cone, "Cone", "Bertrand Mathieu", "09/07/2002", "Textured cone", "1", "1", 3);
//===================================================================================

Cone::Cone(GlyphContext *gc): Glyph(gc),listOk(false) {
  setLOD(8);
}

Cone::~Cone() {
  if (listOk)
    if (glIsList(LList)) 
      glDeleteLists(LList, 1);
}

void Cone::setLOD(int n) {
  LOD = ((n<0) ? 0 : ((n > 10) ? 10 : n));
  if (listOk) {
    glDeleteLists(LList, 1);
    listOk=false;
  }
}

void Cone::draw(node n) {
  setMaterial(glGraph->elementColor->getNodeValue(n));
  string texFile = glGraph->elementTexture->getNodeValue(n);
  if (texFile != "") {
    if (glGraph->activateTexture(texFile))
	setMaterial(Color(255,255,255,255));
  }

  if (!listOk) {
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricTexture(quadratic, GL_TRUE);  
    LList = glGenLists( 1 );
    glNewList( LList, GL_COMPILE ); 
    glTranslatef(0.0f, 0.0f, -0.5f);
    gluQuadricOrientation(quadratic, GLU_OUTSIDE);
    gluCylinder(quadratic, 0.5f, 0.0f, 1.0f, LOD, LOD);
    gluQuadricOrientation(quadratic, GLU_INSIDE);
    gluDisk(quadratic, 0.0f, 0.5f, LOD, LOD);
    glEndList();
    gluDeleteQuadric(quadratic);
    GLenum error=glGetError();
    if ( error != GL_NO_ERROR)
      cerr << "Open GL Error : " << error << " in " << __PRETTY_FUNCTION__ << endl;
    listOk=true;
  }
  assert(glIsList(LList));
  glCallList(LList);
}

Coord Cone::getAnchor(const Coord &vector) const
{
  Coord anchor = vector;

  float x,y,z,n;
  anchor.get(x,y,z);
  n = sqrt(x*x + y*y);
  float vx0, vy0, vx1, vy1, x0, y0, x1, y1, px, py;
  x0=0; y0=0.5; vx0=0.5; vy0=-1.0;
  x1=0; y1=0; vx1=sqrt(x*x + y*y); vy1=z;
  py=-1.0 * (vy1 * (vx0/vy0*y0+x0-x1) - vx1*y1) / (vx1 - vy1/vy0*vx0);
  px=vx0*(py-y0)/vy0+x0;

  if (fabsf(py) > 0.5) {
    n = anchor.norm();
    py = n * 0.5 / fabsf(z);
    anchor.setX(x * py / n);
    anchor.setY(y * py / n);
    anchor.setZ(z * py / n);
  }
  else {
    anchor.setX(x * px / n);
    anchor.setY(y * px / n);
    anchor.setZ(py);
  }

  return anchor;
}

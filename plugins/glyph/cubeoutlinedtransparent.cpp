#include <GL/gl.h>
#include <cmath>

#include <tulip/StringProxy.h>
#include <tulip/ColorsProxy.h>

#include <tulip/Glyph.h>
#include <tulip/SuperGraph.h>
#include <tulip/GlGraph.h>

using namespace std;

class CubeOutLinedTransparent : public Glyph {
public:
  CubeOutLinedTransparent(GlyphContext *gc=NULL);
  virtual ~CubeOutLinedTransparent();
  virtual string getName() {return string("CubeOutLinedTransparent");}
  virtual void draw(node n);
  virtual Coord getAnchor(const Coord & vector) const;

private:
  GLuint LList;
  bool listOk;
  void drawCubeSimple(GLenum);
};

GLYPHPLUGIN(CubeOutLinedTransparent, "CubeOutLinedTransparent", "David Auber", "09/07/2002", "Textured cubeOutLined", "1", "1", 9);

//===================================================================================
CubeOutLinedTransparent::CubeOutLinedTransparent(GlyphContext *gc): Glyph(gc),listOk(false) {
}

CubeOutLinedTransparent::~CubeOutLinedTransparent() {
  if (listOk) {
    if (glIsList(LList)) glDeleteLists(LList, 2);
  }
}

void CubeOutLinedTransparent::draw(node n) {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  if (!listOk) {
    LList = glGenLists( 2 );
    glNewList( LList, GL_COMPILE );
    //    drawCube(GL_QUADS);
    glEndList();
    glNewList( LList + 1, GL_COMPILE );
    //    setMaterial(Color(0,0,0,255));
    drawCubeSimple(GL_LINE_LOOP);
    glEndList();
    GLenum error = glGetError();
    if ( error != GL_NO_ERROR)
      cerr << "Open GL Error : " << error << " in " << __PRETTY_FUNCTION__ << endl;
    listOk = true;
  }
  assert(glIsList(LList));
  setMaterial(glGraph->elementColor->getNodeValue(n));
  string texFile = glGraph->elementTexture->getNodeValue(n);
  if (texFile != "") {
    if (glGraph->activateTexture(texFile))
      setMaterial(Color(255,255,255,0));
  }
  glCallList(LList);
  ColorsProxy *border = glGraph->getSuperGraph()->getProperty<ColorsProxy>("viewBorderColor");
  glGraph->desactivateTexture();
  Color c = border->getNodeValue(n);
  //  setMaterial(c);
  glLineWidth(2);
  glDisable(GL_LIGHTING);
  glColor3ub(c[0],c[1],c[2]);
  glCallList(LList + 1);  
  glEnable(GL_LIGHTING);
}


Coord CubeOutLinedTransparent::getAnchor(const Coord & vector ) const {
  float x,y,z, fmax;
  vector.get(x,y,z);
  fmax = std::max(std::max(fabsf(x), fabsf(y)), fabsf(z));
  if( fmax > 0.0f )
    return vector * (0.5f/fmax);
  else
    return vector;
}

void CubeOutLinedTransparent::drawCubeSimple(GLenum type) {
  /* front face */
  glBegin(type);
  glVertex3f(-0.5f, -0.5f, 0.5f); 
  glVertex3f(0.5f, -0.5f, 0.5f);
  glVertex3f(0.5f, 0.5f, 0.5f);
  glVertex3f(-0.5f, 0.5f, 0.5f);
  glEnd();
  /* back face */
  glBegin(type);
  glVertex3f(-0.5f, -0.5f, -0.5f); 
  glVertex3f(-0.5f, 0.5f, -0.5f);
  glVertex3f(0.5f, 0.5f, -0.5f);
  glVertex3f(0.5f, -0.5f, -0.5f);
  glEnd();
  /* right face */
  glBegin(type);
  glVertex3f(0.5f, -0.5f, -0.5f); 
  glVertex3f(0.5f, 0.5f, -0.5f);
  glVertex3f(0.5f, 0.5f, 0.5f);
  glVertex3f(0.5f, -0.5f, 0.5f);
  glEnd();
  /* left face */
  glBegin(type);
  glVertex3f(-0.5f, -0.5f, 0.5f); 
  glVertex3f(-0.5f, 0.5f, 0.5f);
  glVertex3f(-0.5f, 0.5f, -0.5f);
  glVertex3f(-0.5f, -0.5f, -0.5f);
  glEnd();
  /* top face */
  glBegin(type);
  glVertex3f(0.5f, 0.5f, 0.5f); 
  glVertex3f(0.5f, 0.5f, -0.5f);
  glVertex3f(-0.5f, 0.5f, -0.5f);
  glVertex3f(-0.5f, 0.5f, 0.5f);
  glEnd();
  /* bottom face */
  glBegin(type);
  glVertex3f(0.5f, -0.5f, -0.5f); 
  glVertex3f(0.5f, -0.5f, 0.5f);
  glVertex3f(-0.5f, -0.5f, 0.5f);
  glVertex3f(-0.5f, -0.5f, -0.5f);
  glEnd();
}

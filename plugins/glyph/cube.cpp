#include <cmath>

#include <string>
#include <GL/gl.h>

#include <tulip/StringProxy.h>
#include <tulip/ColorsProxy.h>
#include <tulip/Size.h>
#include <tulip/Coord.h>
#include <tulip/Glyph.h>

#include <tulip/SuperGraph.h>
#include <tulip/GlGraph.h>

using namespace std;

class Cube : public Glyph {
public:
  Cube(GlyphContext *gc=NULL);
  virtual ~Cube();
  virtual string getName() {return string("Cube");}
  virtual void draw(node n);
  virtual Coord getAnchor(const Coord & vector) const;

protected:
  GLuint LList;
  bool listOk;
  void drawCube();
};

GLYPHPLUGIN(Cube, "Cube", "Bertrand Mathieu", "09/07/2002", "Textured cube", "1", "1" , 0);

//===================================================================================
Cube::Cube(GlyphContext *gc): Glyph(gc),listOk(false) {
}

Cube::~Cube() {
  if (listOk)
    if (glIsList(LList)) glDeleteLists(LList, 1);
}

void
Cube::draw(node n) {
  setMaterial(glGraph->elementColor->getNodeValue(n));
  string texFile = glGraph->elementTexture->getNodeValue(n);
  if (texFile != "") {
    if (glGraph->activateTexture(texFile))
      setMaterial(Color(255,255,255,0));
  }
  if (!listOk) {
    LList = glGenLists( 1 );
    glNewList( LList, GL_COMPILE ); 
    drawCube();
    glEndList();
    listOk=true;
  }
  glCallList(LList);
}


Coord Cube::getAnchor(const Coord & vector ) const {
  float x,y,z, fmax;
  vector.get(x,y,z);
  fmax = std::max(std::max(fabsf(x), fabsf(y)), fabsf(z));
  if( fmax > 0.0f )
    return vector * (0.5f/fmax);
  else
    return vector;
}


void Cube::drawCube() {
  glBegin(GL_QUADS);
  /* front face */
  glNormal3f(0.0f, 0.0f, 1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f, -0.5f, 0.5f); 
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(0.5f, -0.5f, 0.5f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(0.5f, 0.5f, 0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f, 0.5f, 0.5f);
  /* back face */
  glNormal3f(0.0f, 0.0f, -1.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-0.5f, -0.5f, -0.5f); 
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-0.5f, 0.5f, -0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(0.5f, 0.5f, -0.5f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(0.5f, -0.5f, -0.5f);
  /* right face */
  glNormal3f(1.0f, 0.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(0.5f, -0.5f, -0.5f); 
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(0.5f, 0.5f, -0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(0.5f, 0.5f, 0.5f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(0.5f, -0.5f, 0.5f);
  /* left face */
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-0.5f, -0.5f, 0.5f); 
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-0.5f, 0.5f, 0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f, 0.5f, -0.5f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f, -0.5f, -0.5f);
  /* top face */
  glNormal3f(0.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(0.5f, 0.5f, 0.5f); 
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(0.5f, 0.5f, -0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f, 0.5f, -0.5f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f, 0.5f, 0.5f);
  /* bottom face */
  glNormal3f(0.0f, -1.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(0.5f, -0.5f, -0.5f); 
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(0.5f, -0.5f, 0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f, -0.5f, 0.5f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f, -0.5f, -0.5f);
  glEnd();
}

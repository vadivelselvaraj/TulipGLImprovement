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

class Square : public Glyph {
public:
  Square(GlyphContext *gc=NULL);
  virtual ~Square();
  virtual string getName() {return string("Square");}
  virtual void draw(node n);
  virtual Coord getAnchor(const Coord &vector) const;

protected:
  GLuint LList;
  bool listOk;
  void drawSquare();
};

GLYPHPLUGIN(Square, "Square", "David Auber", "09/07/2002", "Textured square", "1", "1", 4);

//===================================================================================
Square::Square(GlyphContext *gc): Glyph(gc),listOk(false) {
}

Square::~Square() {
  if (listOk)
    if (glIsList(LList)) glDeleteLists(LList, 1);
}

void
Square::draw(node n) {
  setMaterial(glGraph->elementColor->getNodeValue(n));
  string texFile = glGraph->elementTexture->getNodeValue(n);
  if (texFile != "") {
    if (glGraph->activateTexture(texFile))
      setMaterial(Color(255,255,255,0));
  }
  if (!listOk) {
    LList = glGenLists( 1 );
    glNewList( LList, GL_COMPILE ); 
    drawSquare();
    glEndList();
    listOk=true;
  }
  glCallList(LList);
}


Coord Square::getAnchor(const Coord &vector) const
{
	Coord v( vector );
	float x,y,z, fmax;
	v.get(x,y,z);
	v.setZ( 0.0f );
	fmax = fabsf(x) >? fabsf(y);
	if( fmax > 0.0f )
		return v * (0.5f/fmax);
	else
		return v;
}


void Square::drawSquare()
{
  glBegin(GL_QUADS);
  /* front face */
  glNormal3f(0.0f, 0.0f, 1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-0.5f, -0.5f); 
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(0.5f, -0.5f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(0.5f, 0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(-0.5f, 0.5f);
  /* back face */
  glNormal3f(0.0f, 0.0f,-1.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(-0.5f, -0.5f); 
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(-0.5f, 0.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0.5f, 0.5f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0.5f, -0.5f);
  glEnd();
}

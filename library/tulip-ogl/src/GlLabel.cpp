#include "tulip/GlLabel.h"

#include <tulip/Coord.h>
#include <tulip/LayoutProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/BooleanProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>

#include "tulip/GlTools.h"
#include "tulip/GlyphManager.h"
#include "tulip/GlDisplayListManager.h"
#include "tulip/OcclusionTest.h"
#include "tulip/TextRenderer.h"
#include "tulip/GlTLPFeedBackBuilder.h"

#include <iostream>

using namespace std;

//====================================================
#ifdef _WIN32
#ifdef DLL_EXPORT
tlp::TextRenderer* tlp::GlLabel::renderer=0;
#endif
#else
tlp::TextRenderer *tlp::GlLabel::renderer=0;
#endif

namespace tlp {

  GlLabel::GlLabel() {
    if(!renderer){
      renderer=new TextRenderer;
      renderer->setContext(TulipBitmapDir + "font.ttf", 20, 0, 0, 255);
      renderer->setMode(TLP_TEXTURE);
    }
  }
  GlLabel::GlLabel(Coord centerPosition,Coord size,Color fontColor,bool leftAlign):centerPosition(centerPosition),size(size),color(fontColor),leftAlign(leftAlign) {
    GlLabel();
  }

  GlLabel::GlLabel(const string &fontPath,Coord centerPosition,Coord size,Color fontColor,bool leftAlign):centerPosition(centerPosition),size(size),color(fontColor),leftAlign(leftAlign){
    GlLabel();
  }

  GlLabel::~GlLabel() {
  }
  //============================================================
  void GlLabel::setText(const string& text) {
    this->text=text;
  }
  //============================================================
  BoundingBox GlLabel::getBoundingBox() {
    if(!leftAlign)
      return BoundingBox(centerPosition-size/2,centerPosition+size/2);
    else
      return BoundingBox(centerPosition-Coord(0,size[1]/2,0),centerPosition+Coord(size[0],size[1]/2,0));
  }
  //============================================================
  Coord GlLabel::getSize() {
    return size;
  }
  //============================================================
  void GlLabel::draw(float lod, Camera *camera) {

    renderer->setString(text, VERBATIM);
    renderer->setColor(color[0], color[1], color[2]);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);

    float w_max = 300;
    float w,h;
    float div_w, div_h;

    renderer->getBoundingBox(w_max, h, w);

    glPushMatrix();

    div_w = size[0]/w;
    div_h = size[1]/h;

    if(!leftAlign) {
      glTranslatef(centerPosition[0],centerPosition[1], centerPosition[2]);
      if(div_h > div_w)
	glScalef(div_w, div_w, 1);
      else
	glScalef(div_h, div_h, 1);
    }else{
      glTranslatef(centerPosition[0]+size[0]/2,centerPosition[1], centerPosition[2]);
      glScalef(div_w, div_h, 1);
    }

    glEnable( GL_TEXTURE_2D);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE_MINUS_SRC_COLOR);
    renderer->draw(w, w, 0);
    glDisable( GL_TEXTURE_2D);
    glPopMatrix();
    glPopAttrib();
  }
  //===========================================================
  void GlLabel::translate(const Coord& mouvement){
    centerPosition+=mouvement;
  }
  //===========================================================
  void GlLabel::getXML(xmlNodePtr rootNode) {
    xmlNodePtr dataNode=NULL;

    xmlNewProp(rootNode,BAD_CAST "type",BAD_CAST "GlLabel");

    GlXMLTools::getDataNode(rootNode,dataNode);

    GlXMLTools::getXML(dataNode,"text",text);
    GlXMLTools::getXML(dataNode,"centerPosition",centerPosition);
    GlXMLTools::getXML(dataNode,"size",size);
    GlXMLTools::getXML(dataNode,"color",color);
    GlXMLTools::getXML(dataNode,"leftAlign",leftAlign);

  }
  //============================================================
  void GlLabel::setWithXML(xmlNodePtr rootNode) {
    xmlNodePtr dataNode=NULL;

    GlXMLTools::getDataNode(rootNode,dataNode);

    // Parse Data
    if(dataNode) {
      GlXMLTools::setWithXML(dataNode,"text",text);
      GlXMLTools::setWithXML(dataNode,"centerPosition",centerPosition);
      GlXMLTools::setWithXML(dataNode, "size", size);
      GlXMLTools::setWithXML(dataNode,"color",color);
      GlXMLTools::setWithXML(dataNode,"leftAlign",leftAlign);
    }
  }

}

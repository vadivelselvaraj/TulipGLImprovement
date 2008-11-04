#include <GL/gl.h>
#include <GL/glu.h>
#include "tulip/GlComplexPolygon.h"
#include "tulip/GlTools.h"
#include "tulip/GlLayer.h"
#include "tulip/GlTextureManager.h"
#include "tulip/Bezier.h"

#include <tulip/TlpTools.h>



using namespace std;

namespace tlp {

  void beginCallback(GLenum which)
  {
    glBegin(which);
  }
  
  void errorCallback(GLenum errorCode)
  {
    const GLubyte *estring;
    
    estring = gluErrorString(errorCode);
    cout << "Tessellation Error: " << estring << endl;
  }

  void endCallback(void)
  {
    glEnd();
  }

  void vertexCallback(GLvoid *vertex)
  {
    const GLdouble *pointer;
    
    pointer = (GLdouble *) vertex;
    Color color=Color(pointer[3],pointer[4],pointer[5],pointer[6]);
    setMaterial(color);
    glColor4ubv((unsigned char *)&color);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(pointer[0], pointer[1]);
    glVertex3dv((GLdouble *)vertex);
  }

  GlComplexPolygon::GlComplexPolygon(vector<Coord> &coords,Color fcolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(false),
    fillColor(fcolor),
    textureName(textureName){
    createPolygon(coords,bezier);
  }
  //=====================================================
  GlComplexPolygon::GlComplexPolygon(vector<Coord> &coords,Color fcolor,Color ocolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(true),
    fillColor(fcolor),
    outlineColor(ocolor),
    textureName(textureName) {
    createPolygon(coords,bezier);
  }
  //=====================================================
  GlComplexPolygon::GlComplexPolygon(vector<vector<Coord> >&coords,Color fcolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(false),
    fillColor(fcolor),
    textureName(textureName){
    for(int i=0;i<coords.size();++i) {
      createPolygon(coords[i],bezier);
      beginNewHole();
    }
  }
  //=====================================================
  GlComplexPolygon::GlComplexPolygon(vector<vector<Coord> >&coords,Color fcolor,Color ocolor,int bezier,const string &textureName):
    currentVector(0),
    outlined(true),
    fillColor(fcolor),
    outlineColor(ocolor),
    textureName(textureName) {
    for(int i=0;i<coords.size();++i) {
      createPolygon(coords[i],bezier);
      beginNewHole();
    }
  }
  //=====================================================
  GlComplexPolygon::~GlComplexPolygon() {
  }
  //=====================================================
  void GlComplexPolygon::createPolygon(vector<Coord> &coords,int bezier) {
    points.push_back(vector<Coord>());
    if(bezier==0) {
      for(vector<Coord>::iterator it=coords.begin();it!=coords.end();++it) {
	addPoint(*it);
      }
    }else{
      double dCoords[coords.size()][3];
      int i=0;
      for(vector<Coord>::iterator it=coords.begin();it!=coords.end();++it) {
	dCoords[i][0]=(*it)[0];dCoords[i][1]=(*it)[1];dCoords[i][2]=(*it)[2];
	i++;
      }
      addPoint(coords[0]);

      double result[3];
      double dec=1./(bezier*coords.size());
      for(double j=dec;j<1;j+=dec) {
	Bezier(result,dCoords,coords.size(),j);
	addPoint(Coord(result[0],result[1],result[2]));
      }
      addPoint(coords[coords.size()-1]);
    }
  }
  //=====================================================
  void GlComplexPolygon::setOutlineMode(const bool outlined) {
    this->outlined = outlined;
  }
  //=====================================================
  void GlComplexPolygon::addPoint(const Coord& point) {
    points[currentVector].push_back(point);
    boundingBox.check(point);
  }
  //=====================================================
  void GlComplexPolygon::beginNewHole() {
    currentVector++;
  }
  //=====================================================
  void GlComplexPolygon::draw(float lod,Camera *camera) {
    glDisable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);

    if(GlTextureManager::getInst().activateTexture(textureName));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    
    GLUtesselator *tobj;
    tobj = gluNewTess();
    
    gluTessCallback(tobj, GLU_TESS_VERTEX, 
		    (void (*)())vertexCallback);
    gluTessCallback(tobj, GLU_TESS_BEGIN, 
		    (void (*)())beginCallback);
    gluTessCallback(tobj, GLU_TESS_END, 
		    (void (*)())endCallback);
    gluTessCallback(tobj, GLU_TESS_ERROR, 
		    (void (*)())errorCallback);
      
    glShadeModel(GL_SMOOTH);  
      
    gluTessBeginPolygon(tobj, NULL);
    for(int v=0;v<points.size();++v) {
      gluTessBeginContour(tobj);
      for(unsigned int i=0; i < points[v].size(); ++i) {
	GLdouble *tmp=new GLdouble[7];
	tmp[0]=points[v][i][0];
	tmp[1]=points[v][i][1];
	tmp[2]=points[v][i][2];
	tmp[3]=fillColor[0];
	tmp[4]=fillColor[1];
	tmp[5]=fillColor[2];
	tmp[6]=fillColor[3];
	gluTessVertex(tobj,tmp,tmp);
      }
      gluTessEndContour(tobj);
    }
    gluTessEndPolygon(tobj);
    gluDeleteTess(tobj);
    GlTextureManager::getInst().desactivateTexture();
    
    if (outlined) {
      for(int v=0;v<points.size();++v) {
	glBegin(GL_LINE_LOOP);
	for(unsigned int i=0; i < points[v].size(); ++i) {
	  setMaterial(outlineColor);
	  glColor4ubv((unsigned char *)&outlineColor);
	  glVertex3fv((float *)&points[v][i]);
	}
	glEnd();
      }
    }
    
    glTest(__PRETTY_FUNCTION__);
  }
  //===========================================================
  void GlComplexPolygon::translate(const Coord& mouvement) {
    boundingBox.first+=mouvement;
    boundingBox.second+=mouvement;
    
    for(vector<vector<Coord> >::iterator it=points.begin();it!=points.end();++it){
      for(vector<Coord>::iterator it2=(*it).begin();it2!=(*it).end();++it2) {
	(*it2)+=mouvement;
      }
    }
  } 
  //===========================================================
  void GlComplexPolygon::getXML(xmlNodePtr rootNode) {

    GlXMLTools::createProperty(rootNode, "type", "GlComplexPolygon");

    getXMLOnlyData(rootNode);
    
  }
  //===========================================================
  void GlComplexPolygon::getXMLOnlyData(xmlNodePtr rootNode) {
    xmlNodePtr dataNode=NULL;
    
    GlXMLTools::getDataNode(rootNode,dataNode);
    
    for(int i=0;i<points.size();++i){
      stringstream str;
      str << i ;
      GlXMLTools::getXML(dataNode,"points"+str.str(),points[i]);
    }
    GlXMLTools::getXML(dataNode,"fillColor",fillColor);
    GlXMLTools::getXML(dataNode,"outlineColor",outlineColor);
    GlXMLTools::getXML(dataNode,"outlined",outlined);
  }
  //============================================================
  void GlComplexPolygon::setWithXML(xmlNodePtr rootNode) {
    xmlNodePtr dataNode=NULL;

    GlXMLTools::getDataNode(rootNode,dataNode);

    // Parse Data
    if(dataNode) {
      /*GlXMLTools::setWithXML(dataNode,"points",points);
      GlXMLTools::setWithXML(dataNode,"fillColors",fillColors);
      GlXMLTools::setWithXML(dataNode,"outlineColors",outlineColors);*/
      GlXMLTools::setWithXML(dataNode,"outlined",outlined);

      for(vector<vector<Coord> >::iterator it= points.begin();it!=points.end();++it) {
	for(vector<Coord>::iterator it2=(*it).begin();it2!=(*it).end();++it2)
	  boundingBox.check(*it2);
      }
    }
  }
}

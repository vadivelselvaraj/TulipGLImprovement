#include "tulip/QtMetaNodeRenderer.h"

#include <QtOpenGL/QGLFramebufferObject>

#include <tulip/GlGraphInputData.h>
#include <tulip/GlMainWidget.h>
#include <tulip/GlTools.h>

using namespace std;

//====================================================
#ifdef _WIN32
#ifdef DLL_EXPORT
unsigned int tlp::QtMetaNodeRenderer::depth=0;
#endif
#else
unsigned int tlp::QtMetaNodeRenderer::depth=0;
#endif

namespace tlp {

  QtMetaNodeRenderer::QtMetaNodeRenderer(QWidget *parent, GlMainWidget *widget,GlGraphInputData *inputData):parent(parent),parentGlMainWidget(widget),inputData(inputData),backgroundColor(Color(255,255,255,0)),stopUpdateMetaNodes(false) {
    glMainWidget=NULL;
  }

  QtMetaNodeRenderer::~QtMetaNodeRenderer(){
    if(glMainWidget){
      delete glMainWidget->getScene()->getGlGraphComposite()->getInputData()->getMetaNodeRenderer();
      delete glMainWidget;
    }

    for(list<string>::iterator it=textureName.begin();it!=textureName.end();++it){
      GlTextureManager::getInst().deleteTexture(*it);
      //delete (*it).second;
    }

    clearObservers();
  }

  void QtMetaNodeRenderer::clearObservers(){
    for(map<Graph *,node>::iterator it=graphToMetaNode.begin();it!=graphToMetaNode.end();++it){
      (*it).first->removeObserver(this);
      (*it).first->removeGraphObserver(this);
    }

    for(map<Graph *,list<Graph *> >::iterator it=parentGraphToGraph.begin();it!=parentGraphToGraph.end();++it){
      (*it).first->removeGraphObserver(this);
    }

    for(multimap<PropertyInterface *,Graph *>::iterator it=propertyToGraph.begin();it!=propertyToGraph.end();++it){
      (*it).first->removeObserver(this);
      (*it).first->removePropertyObserver(this);
    }
  }

  void QtMetaNodeRenderer::destroy(Graph *graph){
    graph->removeObserver(this);
    graph->removeGraphObserver(this);

    map<Graph *,list<Graph *> >::iterator it=parentGraphToGraph.find(graph);
    if(it!=parentGraphToGraph.end()){
      (*it).first->removeGraphObserver(this);
      parentGraphToGraph.erase(it);
    }

    vector<multimap<PropertyInterface *,Graph *>::iterator> toErase;
    for(multimap<PropertyInterface *,Graph *>::iterator it=propertyToGraph.begin();it!=propertyToGraph.end();++it){
      if((*it).second==graph){
        (*it).first->removeObserver(this);
        (*it).first->removePropertyObserver(this);
        toErase.push_back(it);
      }
    }
    for(vector<multimap<PropertyInterface *,Graph *>::iterator>::iterator it=toErase.begin();it!=toErase.end();++it){
      propertyToGraph.erase(*it);
    }
  }

  void QtMetaNodeRenderer::prerender(node n,float lod,Camera *camera){
    if(QtMetaNodeRenderer::depth>=3)
      return;

    if(lod<20.)
      return;

    lod=lod/2.;

    Graph *metaGraph=inputData->getGraph()->getNodeMetaInfo(n);
    if(!metaGraph)
      return;
    if(graphToMetaNode.find(metaGraph)==graphToMetaNode.end()){
      metaGraph->addObserver(this);
      metaGraph->addGraphObserver(this);
      graphToMetaNode[metaGraph]=n;

      Graph *parentGraph=metaGraph;
      while(parentGraph!=parentGraph->getSuperGraph()){
        parentGraph=parentGraph->getSuperGraph();
        if(parentGraphToGraph.find(parentGraph)==parentGraphToGraph.end()){
          parentGraphToGraph[parentGraph]=list<Graph *>();
          parentGraph->addGraphObserver(this);
        }
        (*parentGraphToGraph.find(parentGraph)).second.push_back(metaGraph);
      }

      Iterator<std::string> *it=metaGraph->getProperties();
      while(it->hasNext()){
        string propertyName=it->next();
        PropertyInterface* property=metaGraph->getProperty(propertyName);
        propertyToGraph.insert(pair<PropertyInterface *,Graph*>(property,metaGraph));
        property->addPropertyObserver(this);
        property->addObserver(this);
      }
    }

    stringstream str;
    str << this << "metaNode" << n.id ;

    const Size &nodeSize = inputData->elementSize->getNodeValue(n);

    float diagonal=sqrt(nodeSize[0]*nodeSize[0]+nodeSize[1]*nodeSize[1]);
    int newWidth=(int)(nodeSize[0]*(lod/diagonal));
    int newHeight=(int)(nodeSize[1]*(lod/diagonal));

    if(newWidth<16)
      newWidth=16;
    if(newHeight<16)
      newHeight=16;

    int textureWidth,textureHeight;
    GlMainWidget::getTextureRealSize(newWidth,newHeight, textureWidth, textureHeight);
    if(textureWidth>textureHeight)
      textureHeight=textureWidth;
    else
      textureWidth=textureHeight;

    bool render=true;

    if(metaNodeTextureSize.count(n)>0){
      if(textureWidth==metaNodeTextureSize[n].first && textureHeight==metaNodeTextureSize[n].second){
        render=false;
      }
    }
    if(haveToRenderGraph.find(metaGraph)!=haveToRenderGraph.end() && !stopUpdateMetaNodes){
      if(haveToRenderGraph[metaGraph]){
        render=true;
        haveToRenderGraph[metaGraph]=false;
      }
    }

    if(render){
      if(!glMainWidget){
        glMainWidget=new GlMainWidget(NULL,NULL);
        glMainWidget->setData(metaGraph,DataSet());
        GlGraphRenderingParameters param=parentGlMainWidget->getScene()->getGlGraphComposite()->getRenderingParameters();
        glMainWidget->getScene()->getGlGraphComposite()->getInputData()->setMetaNodeRenderer(new QtMetaNodeRenderer(NULL,glMainWidget,glMainWidget->getScene()->getGlGraphComposite()->getInputData()));
        glMainWidget->getScene()->getGlGraphComposite()->setRenderingParameters(param);
      }
      glMainWidget->getScene()->setBackgroundColor(backgroundColor);

      //clear QGLFramebufferObject
      list<string>::iterator it=find(textureName.begin(),textureName.end(),str.str());
      if(it!=textureName.end()){
        GlTextureManager::getInst().deleteTexture(*it);
        //delete (*it).second;
        textureName.erase(it);
      }

      Vector<int,4> viewport;
      glGetIntegerv(GL_VIEWPORT, (GLint *) &viewport[0]);
      QGLContext *context=(QGLContext *)(QGLContext::currentContext());

      Matrix<float, 4> modelviewMatrix;
      glGetFloatv (GL_MODELVIEW_MATRIX, (GLfloat*)&modelviewMatrix);
      Matrix<float, 4> projectionMatrix;
      glGetFloatv (GL_PROJECTION_MATRIX, (GLfloat*)&projectionMatrix);

      Graph *graph=inputData->getGraph();

      Graph *metaGraph = graph->getNodeMetaInfo(n);

      glMainWidget->setGraph(metaGraph);
      ((QtMetaNodeRenderer*)glMainWidget->getScene()->getGlGraphComposite()->getInputData()->getMetaNodeRenderer())->setBackgroundColor(backgroundColor);

      QtMetaNodeRenderer::depth++;
      glMainWidget->createTexture(str.str(),textureWidth,textureHeight);
      QtMetaNodeRenderer::depth--;
      textureName.push_back(str.str());

      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf((GLfloat*)&projectionMatrix);
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf((GLfloat*)&modelviewMatrix);

      context->makeCurrent();
      glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

      metaNodeTextureSize[n].first=textureWidth;
      metaNodeTextureSize[n].second=textureHeight;
    }
  }

  void QtMetaNodeRenderer::render(node n,float lod, Camera *camera){
    if(QtMetaNodeRenderer::depth>=3)
      return;

    if(lod<20.)
      return;

    lod=lod/2.;

    Graph *metaGraph=inputData->getGraph()->getNodeMetaInfo(n);
    if(!metaGraph)
      return;

    stringstream str;
    str << this << "metaNode" << n.id;

    const Coord &nodePos = inputData->elementLayout->getNodeValue(n);
    const Size &nodeSize = inputData->elementSize->getNodeValue(n);

    BoundingBox includeBoundingBox;
    inputData->glyphs.get(inputData->elementShape->getNodeValue(n))->getIncludeBoundingBox(includeBoundingBox);

    Coord newCenter=nodePos+((includeBoundingBox.second-includeBoundingBox.first)/2.f+includeBoundingBox.first-0.5f)*nodeSize;
    //newCenter[2]+=nodeSize[2]/10.;
    Coord newSize=(includeBoundingBox.second-includeBoundingBox.first)*nodeSize;

    float diagonal=sqrt(nodeSize[0]*nodeSize[0]+nodeSize[1]*nodeSize[1]);
    int newWidth=(int)(nodeSize[0]*(lod/diagonal));
    int newHeight=(int)(nodeSize[1]*(lod/diagonal));
    if(newWidth<16)
      newWidth=16;
    if(newHeight<16)
      newHeight=16;
    if(newWidth>newHeight)
      newHeight=newWidth;
    else
      newWidth=newHeight;

    int textureWidth,textureHeight;
    float xTextureDec;
    float yTextureDec;

    glMainWidget->setGraph(metaGraph);
    GlMainWidget::getTextureRealSize(newWidth, newHeight, textureWidth,textureHeight);
    glMainWidget->getTextureShift(newWidth, newHeight, xTextureDec, yTextureDec);

    setMaterial(Color(255,255,255,255));
    GlTextureManager::getInst().activateTexture(str.str());
    glTranslatef(newCenter[0],newCenter[1],newCenter[2]);
    glRotatef(inputData->elementRotation->getNodeValue(n), 0., 0., 1.);
    glDisable(GL_LIGHTING);
    //glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);
    glDisable(GL_BLEND);
    glBegin(GL_QUADS);
    glTexCoord2f(xTextureDec, yTextureDec);
    glVertex3f(-newSize[0]/2.,-newSize[1]/2.,0);
    glTexCoord2f(1.0-xTextureDec, yTextureDec);
    glVertex3f(newSize[0]/2.,-newSize[1]/2.,0);
    glTexCoord2f(1.0-xTextureDec, 1.0-yTextureDec);
    glVertex3f(newSize[0]/2.,newSize[1]/2.,0);
    glTexCoord2f(xTextureDec, 1.0-yTextureDec);
    glVertex3f(-newSize[0]/2.,newSize[1]/2.,0);
    glEnd();
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);

    GlTextureManager::getInst().desactivateTexture();
    glRotatef(inputData->elementRotation->getNodeValue(n), 0., 0., -1.);
    glTranslatef(-newCenter[0],-newCenter[1],-newCenter[2]);
  }

  void QtMetaNodeRenderer::setBackgroundColor(const Color &color){
    backgroundColor=color;
    for(TLP_HASH_MAP<Graph *,bool>::iterator it=haveToRenderGraph.begin();it!=haveToRenderGraph.end();++it){
      (*it).second=true;
    }
    if(glMainWidget){
      ((QtMetaNodeRenderer*)glMainWidget->getScene()->getGlGraphComposite()->getInputData()->getMetaNodeRenderer())->setBackgroundColor(color);
    }
  }

}

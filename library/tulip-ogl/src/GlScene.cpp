/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <climits>
#include <stdio.h>


#include <tulip/OpenGlConfigManager.h>
#include <tulip/GlScene.h>

#ifdef ENABLE_RENDERING_TIME_DISPLAY
#include <omp.h>
#endif

#include <tulip/GlLODSceneVisitor.h>
#include <tulip/OcclusionTest.h>
#include <tulip/GlCPULODCalculator.h>
#include <tulip/GlBoundingBoxSceneVisitor.h>
#include <tulip/GlSelectSceneVisitor.h>
#include <tulip/Camera.h>
#include <tulip/GlSimpleEntity.h>
#include <tulip/GlComplexeEntity.h>
#include <tulip/GlNode.h>
#include <tulip/GlEdge.h>
#include <tulip/GlFeedBackRecorder.h>
#include <tulip/GlSVGFeedBackBuilder.h>
#include <tulip/GlEPSFeedBackBuilder.h>
#include <tulip/GlVertexArrayManager.h>
#include <tulip/GlVertexArrayVisitor.h>

using namespace std;

namespace tlp {

struct entityWithDistanceCompare {
  static GlGraphInputData *inputData;
  bool operator()(const EntityWithDistance &e1, const EntityWithDistance &e2 ) const {
    if(e1.distance>e2.distance)
      return true;

    if(e1.distance<e2.distance)
      return false;

    BoundingBox &bb1 = e1.entity->boundingBox;
    BoundingBox &bb2 = e2.entity->boundingBox;

    if(bb1[1][0] - bb1[0][0] > bb2[1][0] - bb2[0][0])
      return false;
    else
      return true;

  }
};
//====================================================
class GreatThanNode {
public:
  DoubleProperty *metric;
  bool operator() (pair<node,float> n1,pair<node,float> n2)  {
    return (metric->getNodeValue(n1.first) > metric->getNodeValue(n2.first));
  }
};
//====================================================
class GreatThanEdge {
public:
  DoubleProperty *metric;
  bool operator() (pair<edge,float> e1,pair<edge,float> e2) {
    return (metric->getEdgeValue(e1.first) > metric->getEdgeValue(e2.first));
  }
};
//====================================================
GlGraphInputData *entityWithDistanceCompare::inputData=NULL;

GlScene::GlScene(GlLODCalculator *calculator):viewportZoom(1),xDecViewport(0),yDecViewport(0),backgroundColor(255, 255, 255, 255),viewLabel(true),viewOrtho(true),displayEdgesInLastRendering(true),glGraphComposite(NULL), noClearBackground(false) {

  if(calculator!=NULL)
    lodCalculator=calculator;
  else
    lodCalculator=new GlCPULODCalculator();

  lodCalculator->setScene(*this);
}

GlScene::~GlScene() {
  delete lodCalculator;

  for(vector<pair<string,GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    delete (*it).second;
  }
}

void GlScene::initGlParameters() {
  OpenGlConfigManager::getInst().initGlew();
  OpenGlConfigManager::getInst().checkDrivers();

  glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);

  bool antialiased = true;

  if(glGraphComposite) {
    antialiased=glGraphComposite->getInputData()->parameters->isAntialiased();
  }

  OpenGlConfigManager::getInst().setAntiAliasing(antialiased);

  glDisable(GL_POINT_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(1.0);
  glPointSize(1.0);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glDepthFunc(GL_LEQUAL );
  glPolygonMode(GL_FRONT, GL_FILL);
  glColorMask(1, 1, 1, 1);
  glEnable(GL_BLEND);
  glIndexMask(UINT_MAX);
  glClearStencil(0xFFFF);
  glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

  if (!noClearBackground) {
    glClearColor(backgroundColor.getRGL(), backgroundColor.getGGL(), backgroundColor.getBGL(), backgroundColor.getAGL());
    glClear(GL_COLOR_BUFFER_BIT);
  }

  glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glDisable(GL_TEXTURE_2D);

  GLenum error = glGetError();

  if ( error != GL_NO_ERROR)
    cerr << "[OpenGL Error] => " << gluErrorString(error) << endl << "\tin : " << __PRETTY_FUNCTION__ << endl;
}

void drawLabelsForComplexEntities(bool drawSelected,GlGraphComposite *glGraphComposite,
                                  OcclusionTest *occlusionTest,LayerLODUnit &layerLODUnit) {
  Graph *graph=glGraphComposite->getInputData()->getGraph();
  BooleanProperty *selectionProperty=glGraphComposite->getInputData()->getElementSelected();
  bool viewOutScreenLabel=glGraphComposite->getRenderingParameters().isViewOutScreenLabel();
  DoubleProperty *metric=NULL;

  if(graph->existProperty("viewMetric"))
    metric=graph->getProperty<DoubleProperty>("viewMetric");

  vector<pair<node,float> > nodesMetricOrdered;
  vector<pair<edge,float> > edgesMetricOrdered;
  GlNode glNode(0);
  GlEdge glEdge(0);
  GlMetaNode glMetaNode(0);

  Iterator<node> *nonDefaultLabelNodes = glGraphComposite->getInputData()->getElementLabel()->getNonDefaultValuatedNodes();
  Iterator<edge> *nonDefaultLabelEdges = glGraphComposite->getInputData()->getElementLabel()->getNonDefaultValuatedEdges();

  bool nodeLabelEmpty=(!nonDefaultLabelNodes->hasNext())
                      && glGraphComposite->getInputData()->getElementLabel()->getNodeDefaultStringValue()=="";
  bool edgeLabelEmpty=(!nonDefaultLabelEdges->hasNext())
                      && glGraphComposite->getInputData()->getElementLabel()->getEdgeDefaultStringValue()=="";

  delete nonDefaultLabelNodes;
  delete nonDefaultLabelEdges;

  bool viewNodeLabel=glGraphComposite->getInputData()->parameters->isViewNodeLabel();
  bool viewMetaLabel=glGraphComposite->getInputData()->parameters->isViewMetaLabel();

  // Draw Labels for Nodes
  if((viewNodeLabel || viewMetaLabel) && !nodeLabelEmpty) {
    node n;

    for(vector<ComplexEntityLODUnit>::iterator it=layerLODUnit.nodesLODVector.begin(); it!=layerLODUnit.nodesLODVector.end(); ++it) {
      if((*it).lod<0 && !viewOutScreenLabel)
        continue;

      float lod=(*it).lod;

      if(viewOutScreenLabel && lod<0)
        lod=-lod;

      n.id=(*it).id;

      if(selectionProperty->getNodeValue(n)==drawSelected) {
        if(!glGraphComposite->getInputData()->parameters->isElementOrdered() || !metric) {
          // Not metric ordered
          if(!graph->isMetaNode(n) && viewNodeLabel) {
            glNode.id=n.id;
            glNode.drawLabel(occlusionTest,glGraphComposite->getInputData(),lod,(Camera *)(layerLODUnit.camera));
          }
          else if(graph->isMetaNode(n)) {
            glMetaNode.id=n.id;
            glMetaNode.drawLabel(occlusionTest,glGraphComposite->getInputData(),lod,(Camera *)(layerLODUnit.camera));
          }
        }
        else {
          // Metric ordered
          if((!graph->isMetaNode(n) && viewNodeLabel) || graph->isMetaNode(n)) {
            nodesMetricOrdered.push_back(pair<node,float>(n,lod));
          }
        }
      }
    }

    // If not Metric ordered : a this point selected nodes are draw
    if(glGraphComposite->getInputData()->parameters->isElementOrdered()) {
      // Draw selected nodes label with metric ordering
      if(glGraphComposite->getInputData()->parameters->getElementOrderingProperty()) {
        metric = glGraphComposite->getInputData()->parameters->getElementOrderingProperty();
      }

      GreatThanNode ltn;
      ltn.metric=metric;
      sort(nodesMetricOrdered.begin(),nodesMetricOrdered.end(),ltn);

      for(vector<pair<node,float> >::iterator it=nodesMetricOrdered.begin(); it!=nodesMetricOrdered.end(); ++it) {
        if(!graph->isMetaNode((*it).first)) {
          glNode.id=(*it).first.id;
          glNode.drawLabel(occlusionTest,glGraphComposite->getInputData(),(*it).second,(Camera *)(layerLODUnit.camera));
        }
        else {
          glMetaNode.id=(*it).first.id;
          glMetaNode.drawLabel(occlusionTest,glGraphComposite->getInputData(),(*it).second,(Camera *)(layerLODUnit.camera));
        }
      }
    }
  }

  // Draw Labels for Edges
  if(glGraphComposite->getInputData()->parameters->isViewEdgeLabel() && !edgeLabelEmpty) {
    edge e;

    for(vector<ComplexEntityLODUnit>::iterator it=layerLODUnit.edgesLODVector.begin(); it!=layerLODUnit.edgesLODVector.end(); ++it) {
      if((*it).lod<0 && !viewOutScreenLabel)
        continue;

      e.id=(*it).id;

      if(selectionProperty->getEdgeValue(e) == drawSelected) {
        if(!glGraphComposite->getInputData()->parameters->isElementOrdered() || !metric) {
          // Not metric ordered
          glEdge.id=e.id;
          glEdge.drawLabel(occlusionTest,glGraphComposite->getInputData(),(*it).lod,(Camera *)(layerLODUnit.camera));
        }
        else {
          // Metric ordered
          edgesMetricOrdered.push_back(pair<edge,float>(e,(*it).lod));
        }
      }
    }

    // If not Metric ordered : a this point selected edges are draw
    if(glGraphComposite->getInputData()->parameters->isElementOrdered()) {
      // Draw selected edges label with metric ordering
      if(glGraphComposite->getInputData()->parameters->getElementOrderingProperty()) {
        metric = glGraphComposite->getInputData()->parameters->getElementOrderingProperty();
      }

      GreatThanEdge lte;
      lte.metric=metric;
      sort(edgesMetricOrdered.begin(),edgesMetricOrdered.end(),lte);

      for(vector<pair<edge,float> >::iterator it=edgesMetricOrdered.begin(); it!=edgesMetricOrdered.end(); ++it) {
        glEdge.id=(*it).first.id;
        glEdge.drawLabel(occlusionTest,glGraphComposite->getInputData(),(*it).second,(Camera *)(layerLODUnit.camera));
      }
    }
  }
}


void GlScene::draw() {
  initGlParameters();

  /**********************************************************************
  LOD Compute
      **********************************************************************/
  lodCalculator->clear();
  lodCalculator->setRenderingEntitiesFlag(RenderingAll);

#ifdef ENABLE_RENDERING_TIME_DISPLAY
  double lastTime=omp_get_wtime();
#endif

  /**
  * If LOD Calculator need entities to compute LOD, we use visitor system
  */
  if(lodCalculator->needEntities()) {
    GlLODSceneVisitor *lodVisitor;

    if(glGraphComposite)
      lodVisitor = new GlLODSceneVisitor(lodCalculator, glGraphComposite->getInputData());
    else
      lodVisitor = new GlLODSceneVisitor(lodCalculator, NULL);

    for(vector<pair<string,GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
      (*it).second->acceptVisitor(lodVisitor);
    }

    delete lodVisitor;
  }

#ifdef ENABLE_RENDERING_TIME_DISPLAY
  cout << "scene visitor time             : " << (int)((omp_get_wtime()-lastTime)*1000) << " ms" << endl;
  lastTime=omp_get_wtime();
#endif

  lodCalculator->compute(viewport, viewport);
  LayersLODVector &layersLODVector = lodCalculator->getResult();
  BoundingBox sceneBoundingBox = lodCalculator->getSceneBoundingBox();

#ifdef ENABLE_RENDERING_TIME_DISPLAY
  cout << "lod time                       : " << (int)((omp_get_wtime()-lastTime)*1000) << " ms" << endl;
  lastTime=omp_get_wtime();
#endif

  /**
   *  VertexArray compute
   */
  if(glGraphComposite) {
    GlVertexArrayManager *vertexArrayManager = glGraphComposite->getInputData()->getGlVertexArrayManager();
    bool lastDisplayEdge = glGraphComposite->isDisplayEdges();

    if (!displayEdgesInLastRendering && lastDisplayEdge != displayEdgesInLastRendering) {
      vertexArrayManager->setHaveToComputeAll(true);
    }

    if(vertexArrayManager->haveToCompute()) {
      GlVertexArrayVisitor vertexArrayVisitor(glGraphComposite->getInputData());
      glGraphComposite->acceptVisitor(&vertexArrayVisitor);
      vertexArrayManager->setHaveToComputeAll(false);
    }

    displayEdgesInLastRendering = lastDisplayEdge;
  }

#ifdef ENABLE_RENDERING_TIME_DISPLAY
  cout << "vertex array construction time : " << (int)((omp_get_wtime()-lastTime)*1000) << " ms" << endl;
  lastTime=omp_get_wtime();
#endif

  OcclusionTest occlusionTest;

  Camera *camera;
  Graph  *graph = NULL;

  if(glGraphComposite) {
    graph = glGraphComposite->getInputData()->graph;
  }

  GlNode     glNode(0);
  GlMetaNode glMetaNode(0);
  GlEdge     glEdge(0);

  // Iterate on Camera
  Camera *oldCamera = NULL;

  vector<LayerLODUnit>::iterator itLayer;

  for(itLayer = layersLODVector.begin(); itLayer!=layersLODVector.end(); ++itLayer) {
    camera = itLayer->camera;
    camera->setSceneRadius(camera->getSceneRadius(), sceneBoundingBox);

    if(camera != oldCamera) {
      camera->initGl();
      oldCamera = camera;
    }

    GlEdge::clearEdgeWidthLodSystem(viewOrtho);

    bool zOrdering = false;

    if(glGraphComposite)
      zOrdering = glGraphComposite->getRenderingParameters().isElementZOrdered();

    BooleanProperty *filteringProperty = NULL;
    bool displayNodes     = true;
    bool displayMetaNodes = true;
    bool displayEdges     = true;

    if(glGraphComposite) {
      filteringProperty= glGraphComposite->getRenderingParameters().getDisplayFilteringProperty();
      displayNodes     = glGraphComposite->getRenderingParameters().isDisplayNodes();
      displayMetaNodes = glGraphComposite->getRenderingParameters().isDisplayMetaNodes();
      displayEdges     = glGraphComposite->getRenderingParameters().isDisplayEdges();
    }

    if(!zOrdering) {
      // If elements are not zOrdered

      if(glGraphComposite)
        glGraphComposite->getInputData()->getGlVertexArrayManager()->beginRendering();

      // Draw simple entities
      vector<SimpleEntityLODUnit>::iterator it;

      for( it = (*itLayer).simpleEntitiesLODVector.begin(); it!=(*itLayer).simpleEntitiesLODVector.end(); ++it) {
        if((*it).lod<0)
          continue;

        glStencilFunc(GL_LEQUAL,(((*it).entity))->getStencil(), 0xFFFF);
        ((*it).entity)->draw((*it).lod,camera);
      }

      // Draw complex entities
      if(glGraphComposite) {
        vector<ComplexEntityLODUnit>::iterator it;

        //draw nodes and metanodes
        for( it =(*itLayer).nodesLODVector.begin(); it!=(*itLayer).nodesLODVector.end(); ++it) {
          if((*it).lod<=0)
            continue;

          if(filteringProperty) {
            if(filteringProperty->getNodeValue(node((*it).id)))
              continue;
          }

          if(!graph->isMetaNode(node((*it).id))) {
            if(!displayNodes)
              continue;

            glNode.id=(*it).id;
            glNode.draw((*it).lod,glGraphComposite->getInputData(),camera);

          }
          else {
            if(!displayMetaNodes)
              continue;

            glMetaNode.id = (*it).id;
            glMetaNode.draw((*it).lod, glGraphComposite->getInputData(), camera);
          }
        }

        //draw edges
        for(it = (*itLayer).edgesLODVector.begin(); it!=(*itLayer).edgesLODVector.end(); ++it) {
          if((*it).lod<=0)
            continue;

          if(filteringProperty) {
            if(filteringProperty->getEdgeValue(edge((*it).id)))
              continue;
          }

          if(!displayEdges)
            continue;

          glEdge.id=(*it).id;
          glEdge.draw((*it).lod,glGraphComposite->getInputData(),camera);
        }
      }

      if(glGraphComposite)
        glGraphComposite->getInputData()->getGlVertexArrayManager()->endRendering();

    }
    else {
      // If elements are zOrdered

      entityWithDistanceCompare::inputData=glGraphComposite->getInputData();
      multiset<EntityWithDistance,entityWithDistanceCompare> entitiesSet;
      Coord camPos=camera->getEyes();
      Coord camCenter=camera->getCenter();
      BoundingBox bb;
      double dist;

      // Colect simple entities
      for(vector<SimpleEntityLODUnit>::iterator it=(*itLayer).simpleEntitiesLODVector.begin(); it!=(*itLayer).simpleEntitiesLODVector.end(); ++it) {
        if((*it).lod<0)
          continue;

        bb = (*it).boundingBox;
        Coord middle((bb[1] + bb[0])/2.f);
        dist=(((double)middle[0])-((double)camPos[0]))*(((double)middle[0])-((double)camPos[0]));
        dist+=(((double)middle[1])-((double)camPos[1]))*(((double)middle[1])-((double)camPos[1]));
        dist+=(((double)middle[2])-((double)camPos[2]))*(((double)middle[2])-((double)camPos[2]));
        entitiesSet.insert(EntityWithDistance(dist,&(*it)));
      }

      // Colect complex entities
      if(glGraphComposite) {
        for(vector<ComplexEntityLODUnit>::iterator it=(*itLayer).nodesLODVector.begin(); it!=(*itLayer).nodesLODVector.end(); ++it) {
          if((*it).lod<0)
            continue;

          if(filteringProperty) {
            if(filteringProperty->getNodeValue(node((*it).id)))
              continue;
          }

          bb=(*it).boundingBox;
          Coord middle((bb[1]+bb[0])/2.f);
          dist=(((double)middle[0])-((double)camPos[0]))*(((double)middle[0])-((double)camPos[0]));
          dist+=(((double)middle[1])-((double)camPos[1]))*(((double)middle[1])-((double)camPos[1]));
          dist+=(((double)middle[2])-((double)camPos[2]))*(((double)middle[2])-((double)camPos[2]));
          entitiesSet.insert(EntityWithDistance(dist,&(*it),true));
        }

        for(vector<ComplexEntityLODUnit>::iterator it=(*itLayer).edgesLODVector.begin(); it!=(*itLayer).edgesLODVector.end(); ++it) {
          if((*it).lod<0)
            continue;

          if(filteringProperty) {
            if(filteringProperty->getEdgeValue(edge((*it).id)))
              continue;
          }

          bb = (*it).boundingBox;
          Coord middle((bb[0] + bb[1])/2.f);
          dist=(((double)middle[0])-((double)camPos[0]))*(((double)middle[0])-((double)camPos[0]));
          dist+=(((double)middle[1])-((double)camPos[1]))*(((double)middle[1])-((double)camPos[1]));
          dist+=(((double)middle[2])-((double)camPos[2]))*(((double)middle[2])-((double)camPos[2]));
          entitiesSet.insert(EntityWithDistance(dist,&(*it),false));
        }
      }

      // Draw
      for(multiset<EntityWithDistance,entityWithDistanceCompare>::iterator it=entitiesSet.begin(); it!=entitiesSet.end(); ++it) {
        if(!(*it).isComplexEntity) {
          // Simple entities
          GlSimpleEntity *entity = (((SimpleEntityLODUnit*)((*it).entity))->entity);
          glStencilFunc(GL_LEQUAL,entity->getStencil(),0xFFFF);
          entity->draw((*it).entity->lod,camera);
        }
        else {
          // Complex entities
          ComplexEntityLODUnit *entity=(ComplexEntityLODUnit*)((*it).entity);

          if((*it).isNode) {
            if(!graph->isMetaNode(node(entity->id))) {
              if(!displayNodes)
                continue;

              glNode.id=entity->id;
              glNode.draw(entity->lod,glGraphComposite->getInputData(),camera);
            }
            else {
              if(!displayMetaNodes)
                continue;

              glMetaNode.id=entity->id;
              glMetaNode.draw(entity->lod,glGraphComposite->getInputData(),camera);
            }
          }
          else {
            if(!displayEdges)
              continue;

            glEdge.id=entity->id;
            glEdge.draw(entity->lod,glGraphComposite->getInputData(),camera);
          }
        }
      }
    }


    /*
      Label draw
    */

    bool labelDensityAtZero=true;

    if(glGraphComposite) {
      if(glGraphComposite->getInputData()->parameters->getLabelsDensity()!=-100)
        labelDensityAtZero=false;
    }

    if(viewLabel && glGraphComposite && !labelDensityAtZero) {
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glDisable(GL_LIGHTING);
      glDepthFunc(GL_ALWAYS );
      glDisable(GL_CULL_FACE);
      glDisable(GL_COLOR_MATERIAL);

      // Draw Labels for selected entities
      drawLabelsForComplexEntities(true,glGraphComposite,&occlusionTest,*itLayer);

      // Draw Labels for unselected entities
      drawLabelsForComplexEntities(false,glGraphComposite,&occlusionTest,*itLayer);

      glPopAttrib();
    }
  }

#ifdef ENABLE_RENDERING_TIME_DISPLAY
  cout << "draw time                      : " << (int)((omp_get_wtime()-lastTime)*1000) << " ms" << endl;
#endif

}

void GlScene::addLayer(GlLayer *layer) {
  layersList.push_back(std::pair<std::string,GlLayer*>(layer->getName(),layer));
  layer->setScene(this);

  if (hasOnlookers())
    sendEvent(GlSceneEvent(*this,GlSceneEvent::TLP_ADDLAYER,layer->getName(),layer));
}

bool GlScene::insertLayerBefore(GlLayer *layer,const string &name) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).first==name) {
      layersList.insert(it,pair<string,GlLayer*>(layer->getName(),layer));
      layer->setScene(this);

      if (hasOnlookers())
        sendEvent(GlSceneEvent(*this,GlSceneEvent::TLP_ADDLAYER,layer->getName(),layer));

      return true;
    }
  }

  return false;
}

bool GlScene::insertLayerAfter(GlLayer *layer,const string &name) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).first==name) {
      ++it;
      layersList.insert(it,pair<string,GlLayer*>(layer->getName(),layer));
      layer->setScene(this);

      if (hasOnlookers())
        sendEvent(GlSceneEvent(*this,GlSceneEvent::TLP_ADDLAYER,layer->getName(),layer));

      return true;
    }
  }

  return false;
}

void GlScene::removeLayer(const std::string& name,bool deleteLayer) {
  for(vector<pair<string,GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).first==name) {
      GlLayer *layer=(*it).second;
      layersList.erase(it);

      if (hasOnlookers())
        sendEvent(GlSceneEvent(*this,GlSceneEvent::TLP_DELLAYER,layer->getName(),layer));

      if(deleteLayer)
        delete layer;

      return;
    }
  }
}

void GlScene::removeLayer(GlLayer *layer,bool deleteLayer) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).second==layer) {
      GlLayer *layer=(*it).second;
      layersList.erase(it);

      if (hasOnlookers())
        sendEvent(GlSceneEvent(*this,GlSceneEvent::TLP_DELLAYER,layer->getName(),layer));

      if(deleteLayer)
        delete layer;

      return;
    }
  }
}

void GlScene::notifyModifyLayer(const std::string &name,GlLayer *layer) {
  if (hasOnlookers())
    sendEvent(GlSceneEvent(*this,GlSceneEvent::TLP_MODIFYLAYER,name,layer));
}

void GlScene::notifyModifyEntity(GlSimpleEntity *entity) {
  if (hasOnlookers())
    sendEvent(GlSceneEvent(*this,entity));
}

void GlScene::centerScene() {
  ajustSceneToSize(viewport[2], viewport[3]);
}

void GlScene::computeAjustSceneToSize(int width, int height, Coord *center, Coord *eye, float *sceneRadius, float *xWhiteFactor, float *yWhiteFactor,BoundingBox *sceneBoundingBox,float *zoomFactor) {
  if(xWhiteFactor)
    *xWhiteFactor=0.;

  if(yWhiteFactor)
    *yWhiteFactor=0.;

  GlBoundingBoxSceneVisitor *visitor;

  if(glGraphComposite)
    visitor=new GlBoundingBoxSceneVisitor(glGraphComposite->getInputData());
  else
    visitor=new GlBoundingBoxSceneVisitor(NULL);

  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).second->getCamera().is3D() && (!(*it).second->useSharedCamera()))
      (*it).second->acceptVisitor(visitor);
  }

  BoundingBox boundingBox(visitor->getBoundingBox());
  delete visitor;


  if(!boundingBox.isValid()) {
    if(center)
      *center=Coord(0,0,0);

    if(sceneRadius)
      *sceneRadius=static_cast<float>(sqrt(300.0));

    if(eye && center && sceneRadius) {
      *eye=Coord(0, 0, *sceneRadius);
      *eye=*eye + *center;
    }

    if(zoomFactor) {
      *zoomFactor=1.;
    }


    return;
  }

  Coord maxC(boundingBox[1]);
  Coord minC(boundingBox[0]);

  int zoomTmp=1;

  for(int i=0; i<viewportZoom-1; ++i) {
    zoomTmp=zoomTmp*2;
  }


  double dx = maxC[0] - minC[0];
  double dy = maxC[1] - minC[1];
  double dz = maxC[2] - minC[2];

  double dxZoomed=(maxC[0] - minC[0])/zoomTmp;
  double dyZoomed=(maxC[1] - minC[1])/zoomTmp;

  int newXDecViewport;

  if(xDecViewport<zoomTmp/2)
    newXDecViewport=xDecViewport-zoomTmp/2;
  else
    newXDecViewport=xDecViewport-zoomTmp/2+1;

  int newYDecViewport;

  if(yDecViewport<zoomTmp/2)
    newYDecViewport=yDecViewport-zoomTmp/2;
  else
    newYDecViewport=yDecViewport-zoomTmp/2+1;



  Coord centerTmp=(maxC + minC)/2;

  if(zoomTmp!=1) {
    double dec;

    if(dxZoomed>=dyZoomed)
      dec=dxZoomed;
    else
      dec=dyZoomed;

    if(newXDecViewport<0) {
      if(newXDecViewport==-1)
        centerTmp[0]-=static_cast<float>(dec/2);
      else
        centerTmp[0]-=static_cast<float>(dec/2-dec*(newXDecViewport+1));
    }
    else {
      if(newXDecViewport==1)
        centerTmp[0]+=static_cast<float>(dec/2);
      else
        centerTmp[0]+=static_cast<float>(dec/2+dec*(newXDecViewport-1));
    }

    if(newYDecViewport<0) {
      if(newYDecViewport==-1)
        centerTmp[1]-=static_cast<float>(dec/2);
      else
        centerTmp[1]-=static_cast<float>(dec/2-dec*(newYDecViewport+1));
    }
    else {
      if(newYDecViewport==1)
        centerTmp[1]+=static_cast<float>(dec/2);
      else
        centerTmp[1]+=static_cast<float>(dec/2+dec*(newYDecViewport-1));
    }
  }


  if(center) {
    *center=centerTmp;
  }

  if ((dx==0) && (dy==0) && (dz==0))
    dx = dy = dz = 10.0;

  double wdx=width/dxZoomed;
  double hdy=height/dyZoomed;

  float sceneRadiusTmp;

  if(dx<dy) {
    if(wdx<hdy) {
      sceneRadiusTmp=static_cast<float>(dx);

      if(yWhiteFactor)
        *yWhiteFactor=static_cast<float>((1.-(dy/(sceneRadiusTmp*(height/width))))/2.);
    }
    else {
      if (width < height)
        sceneRadiusTmp=static_cast<float>(dx*wdx/hdy);
      else
        sceneRadiusTmp=static_cast<float>(dy);

      if(xWhiteFactor) {
        *xWhiteFactor=static_cast<float>((1.-(dx/sceneRadiusTmp))/2.);
      }
    }
  }
  else {
    if(wdx>hdy) {
      sceneRadiusTmp=static_cast<float>(dy);

      if(xWhiteFactor)
        *xWhiteFactor=static_cast<float>((1.-(dx/(sceneRadiusTmp*(width/height))))/2.);
    }
    else {
      if (height < width)
        sceneRadiusTmp=static_cast<float>(dy*hdy/wdx);
      else
        sceneRadiusTmp=static_cast<float>(dx);

      if(yWhiteFactor)
        *yWhiteFactor=static_cast<float>((1.-(dy/sceneRadiusTmp))/2.);
    }
  }

  if(sceneRadius) {
    *sceneRadius=sceneRadiusTmp;
  }

  if(eye) {
    *eye=Coord(0, 0, sceneRadiusTmp);
    *eye=*eye + centerTmp;
  }

  if(sceneBoundingBox) {
    *sceneBoundingBox=boundingBox;
  }

  if(zoomFactor) {
    *zoomFactor=static_cast<float>(zoomTmp);
  }

}

void GlScene::ajustSceneToSize(int width, int height) {

  Coord center;
  Coord eye;
  float sceneRadius;
  float zoomFactor;
  BoundingBox sceneBoundingBox;

  computeAjustSceneToSize(width,height, &center, &eye, &sceneRadius,NULL,NULL,&sceneBoundingBox,&zoomFactor);

  for(vector<pair<string,GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    Camera &camera=(*it).second->getCamera();
    camera.setCenter(center);

    camera.setSceneRadius(sceneRadius,sceneBoundingBox);

    camera.setEyes(eye);
    camera.setUp(Coord(0, 1., 0));
    camera.setZoomFactor(zoomFactor);
  }
}

void GlScene::zoomXY(int step, const int x, const int y) {

  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).second->getCamera().is3D() && (!(*it).second->useSharedCamera()))
      (*it).second->getCamera().setZoomFactor((*it).second->getCamera().getZoomFactor() * pow(1.1,step));
  }

  if (step < 0) step *= -1;

  int factX = (int)(step*(double(viewport[2])/2.0-x)/ 7.0);
  int factY = (int)(step*(double(viewport[3])/2.0-y)/ 7.0);
  translateCamera(factX,-factY,0);
}

void GlScene::zoom(float,const Coord& dest) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).second->getCamera().is3D() && (!(*it).second->useSharedCamera())) {
      (*it).second->getCamera().setEyes(dest + ((*it).second->getCamera().getEyes() - (*it).second->getCamera().getCenter()));
      (*it).second->getCamera().setCenter(dest);
    }
  }
}

void GlScene::translateCamera(const int x, const int y, const int z) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).second->getCamera().is3D() && (!(*it).second->useSharedCamera())) {
      Coord v1(0, 0, 0);
      Coord v2(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
      v1 = (*it).second->getCamera().screenTo3DWorld(v1);
      v2 = (*it).second->getCamera().screenTo3DWorld(v2);
      Coord move = v2 - v1;
      (*it).second->getCamera().setEyes((*it).second->getCamera().getEyes() + move);
      (*it).second->getCamera().setCenter((*it).second->getCamera().getCenter() + move);
    }
  }
}

void GlScene::zoom(int step) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).second->getCamera().is3D() && (!(*it).second->useSharedCamera())) {
      (*it).second->getCamera().setZoomFactor((*it).second->getCamera().getZoomFactor() * pow(1.1, step));
    }
  }
}

void GlScene::rotateScene(const int x, const int y, const int z) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).second->getCamera().is3D() && (!(*it).second->useSharedCamera())) {
      (*it).second->getCamera().rotate(static_cast<float>(x/360.0 * M_PI), 1.0f, 0, 0);
      (*it).second->getCamera().rotate(static_cast<float>(y/360.0 * M_PI), 0, 1.0f, 0);
      (*it).second->getCamera().rotate(static_cast<float>(z/360.0 * M_PI), 0, 0, 1.0f);
    }
  }
}
//========================================================================================================
void GlScene::glGraphCompositeAdded(GlLayer* layer,GlGraphComposite *glGraphComposite) {
  this->graphLayer=layer;
  this->glGraphComposite=glGraphComposite;

  if(glGraphComposite)
    lodCalculator->setInputData(glGraphComposite->getInputData());
}
//========================================================================================================
void GlScene::glGraphCompositeRemoved(GlLayer* layer,GlGraphComposite *glGraphComposite) {
  if(this->glGraphComposite==glGraphComposite) {
    assert(graphLayer==layer);
    graphLayer=NULL;
    this->glGraphComposite=NULL;
  }
}
//========================================================================================================
bool GlScene::selectEntities(RenderingEntitiesFlag type,int x, int y, int w, int h, GlLayer* layer,
                             vector<unsigned long>& selectedEntities) {
  if(w==0)
    w=1;

  if(h==0)
    h=1;

  GlLODCalculator *selectLODCalculator;

  //check if the layer is in scene
  bool layerInScene=true;

  if(layer) {
    layerInScene=false;

    for(vector<pair<string,GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end() && layerInScene; ++it) {
      if((*it).second==layer)
        layerInScene=true;
    }
  }

  if(layerInScene) {
    selectLODCalculator=lodCalculator;
  }
  else {
    selectLODCalculator=lodCalculator->clone();
  }

  selectLODCalculator->setRenderingEntitiesFlag((RenderingEntitiesFlag)(RenderingAll | RenderingWithoutRemove));

  selectLODCalculator->clear();

  //Collect entities if need
  GlLODSceneVisitor *lodVisitor;

  if(glGraphComposite)
    lodVisitor=new GlLODSceneVisitor(selectLODCalculator,glGraphComposite->getInputData());
  else
    lodVisitor=new GlLODSceneVisitor(selectLODCalculator,NULL);

  if(layerInScene) {
    if(selectLODCalculator->needEntities()) {
      for(vector<pair<string,GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
        (*it).second->acceptVisitor(lodVisitor);
      }
    }
  }
  else {
    layer->acceptVisitor(lodVisitor);
  }

  delete lodVisitor;

  Vector<int,4> selectionViewport;
  selectionViewport[0]=x;
  selectionViewport[1]=y;
  selectionViewport[2]=w;
  selectionViewport[3]=h;

  glViewport(selectionViewport[0],selectionViewport[1],selectionViewport[2],selectionViewport[3]);

  selectLODCalculator->compute(viewport,selectionViewport);

  LayersLODVector &layersLODVector=selectLODCalculator->getResult();

  for(vector<LayerLODUnit>::iterator itLayer=layersLODVector.begin(); itLayer!=layersLODVector.end(); ++itLayer) {
    Camera *camera=(Camera*)((*itLayer).camera);

    Vector<int, 4> viewport = camera->getViewport();

    unsigned int size;
    size=(*itLayer).simpleEntitiesLODVector.size()+(*itLayer).nodesLODVector.size()+(*itLayer).edgesLODVector.size();

    if(size==0)
      continue;

    glPushAttrib(GL_ALL_ATTRIB_BITS); //save previous attributes
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS); //save previous attributes

    //Allocate memory to store the result oh the selection
    GLuint (*selectBuf)[4] = new GLuint[size][4];
    glSelectBuffer(size*4 , (GLuint *)selectBuf);
    //Activate Open Gl Selection mode
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); //save previous projection matrix

    //initialize picking matrix
    glLoadIdentity();
    int newX = x + w/2;
    int newY = viewport[3] - (y + h/2);
    gluPickMatrix(newX, newY, w, h, (GLint *)&viewport);


    camera->initProjection(false);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); //save previous model view matrix

    camera->initModelView();

    glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);

    map<unsigned int, unsigned long> idToEntity;
    unsigned int id=1;

    if((type & RenderingSimpleEntities)!=0) {
      vector<SimpleEntityLODUnit>::iterator it;

      for(it = (*itLayer).simpleEntitiesLODVector.begin(); it!=(*itLayer).simpleEntitiesLODVector.end(); ++it) {
        if((*it).lod<0)
          continue;

        idToEntity[id] = (unsigned long)((*it).entity);
        glLoadName(id);
        id++;
        (((*it).entity))->draw(20.,camera);
      }
    }

    // Draw complex entities
    if(glGraphComposite) {
      vector<ComplexEntityLODUnit>::iterator it;

      if((type & RenderingNodes)!=0) {
        GlNode glNode(0);

        for(it = (*itLayer).nodesLODVector.begin(); it!=(*itLayer).nodesLODVector.end(); ++it) {
          if((*it).lod<0)
            continue;

          idToEntity[id]=(*it).id;
          glLoadName(id);
          id++;

          glNode.id=(*it).id;
          glNode.draw(20.,glGraphComposite->getInputData(),camera);
        }
      }

      if((type & RenderingEdges)!=0) {
        GlEdge glEdge(0);

        for(it = (*itLayer).edgesLODVector.begin(); it!=(*itLayer).edgesLODVector.end(); ++it) {
          if((*it).lod<0)
            continue;

          idToEntity[id]=(*it).id;
          glLoadName(id);
          id++;

          glEdge.id=(*it).id;
          glEdge.draw(20.,glGraphComposite->getInputData(),camera);
        }
      }
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    //glLoadIdentity();

    glFlush();
    GLint hits = glRenderMode(GL_RENDER);

    while(hits>0) {
      selectedEntities.push_back(idToEntity[selectBuf[hits-1][3]]);
      hits--;
    }

    glPopClientAttrib();
    glPopAttrib();

    delete[] selectBuf;
  }

  selectLODCalculator->clear();

  if(selectLODCalculator!=lodCalculator)
    delete selectLODCalculator;

  glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
  return (selectedEntities.size()!=0);
}
//====================================================
void GlScene::outputSVG(unsigned int size,const string& filename) {
  if(!glGraphComposite)
    return;

  GLint returned;
  GLfloat clearColor[4];
  GLfloat lineWidth;
  GLfloat pointSize;
  GLfloat* buffer = (GLfloat *)calloc(size, sizeof(GLfloat));
  glFeedbackBuffer(size, GL_3D_COLOR, buffer);
  glRenderMode(GL_FEEDBACK);
  glGraphComposite->getInputData()->parameters->setFeedbackRender(true);
  draw();
  glGraphComposite->getInputData()->parameters->setFeedbackRender(false);
  glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
  glGetFloatv(GL_LINE_WIDTH, &lineWidth);
  glGetFloatv(GL_POINT_SIZE, &pointSize);

  glFlush();
  glFinish();
  returned = glRenderMode(GL_RENDER);
  GlSVGFeedBackBuilder builder;
  GlFeedBackRecorder recorder(&builder);
  builder.begin(viewport,clearColor,pointSize,lineWidth);
  recorder.record(false,returned,buffer,layersList[0].second->getCamera().getViewport());
  string str;
  builder.getResult(&str);

  if(!filename.empty()) {
    /* subgraphs drawing disabled
       initMapsSVG(_renderingParameters.getGraph(), &ge); */
    FILE* file ;
#ifndef _MSC_VER
    file = fopen(filename.c_str(), "w");
#else
    fopen_s(&file, filename.c_str(), "w");
#endif

    if (file) {
      fprintf(file, "%s",str.c_str());
      fclose(file);
    }
    else {
      perror(filename.c_str());
    }
  }
}
//====================================================
void GlScene::outputEPS(unsigned int size,const string& filename) {
  if(!glGraphComposite)
    return;

  GLint returned;
  GLfloat clearColor[4];
  GLfloat lineWidth;
  GLfloat pointSize;
  GLfloat* buffer = (GLfloat *)calloc(size, sizeof(GLfloat));
  glFeedbackBuffer(size, GL_3D_COLOR, buffer);
  glRenderMode(GL_FEEDBACK);
  glGraphComposite->getInputData()->parameters->setFeedbackRender(true);
  draw();
  glGraphComposite->getInputData()->parameters->setFeedbackRender(false);

  glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
  glGetFloatv(GL_LINE_WIDTH, &lineWidth);
  glGetFloatv(GL_POINT_SIZE, &pointSize);

  glFlush();
  glFinish();
  returned = glRenderMode(GL_RENDER);
  GlEPSFeedBackBuilder builder;
  GlFeedBackRecorder recorder(&builder);
  builder.begin(viewport,clearColor,pointSize,lineWidth);
  recorder.record(false,returned,buffer,layersList[0].second->getCamera().getViewport());
  string str;
  builder.getResult(&str);

  if(!filename.empty()) {
    FILE* file;
#ifndef _MSC_VER
    file = fopen(filename.c_str(), "w");
#else
    fopen_s(&file, filename.c_str(), "w");
#endif


    if (file) {
      fprintf(file, "%s", str.c_str());
      fclose(file);
    }
    else {
      perror(filename.c_str());
    }
  }
}
//====================================================
unsigned char * GlScene::getImage() {
  unsigned char *image = (unsigned char *)malloc(viewport[2]*viewport[3]*3*sizeof(unsigned char));
  draw();
  glFlush();
  glFinish();
  glPixelStorei(GL_PACK_ALIGNMENT,1);
  glReadPixels(viewport[0],viewport[1],viewport[2],viewport[3],GL_RGB,GL_UNSIGNED_BYTE,image);
  return image;
}
//====================================================
GlLayer* GlScene::getLayer(const std::string& name) {
  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {
    if((*it).first==name)
      return (*it).second;
  }

  return NULL;
}
//====================================================
void GlScene::getXML(string &out) {

  out.append("<scene>");

  GlXMLTools::beginDataNode(out);

  GlXMLTools::getXML(out,"viewport",viewport);
  GlXMLTools::getXML(out,"background",backgroundColor);

  GlXMLTools::endDataNode(out);

  GlXMLTools::beginChildNode(out);

  for(vector<pair<string, GlLayer *> >::iterator it=layersList.begin(); it!=layersList.end(); ++it) {

    // Don't save working layers
    if((*it).second->isAWorkingLayer())
      continue;

    GlXMLTools::beginChildNode(out,"GlLayer");

    GlXMLTools::createProperty(out, "name", (*it).first);

    (*it).second->getXML(out);

    GlXMLTools::endChildNode(out,"GlLayer");
  }

  GlXMLTools::endChildNode(out);

  out.append("</scene>");
  cout << out << endl;
}
//====================================================
void GlScene::setWithXML(string &in, Graph *graph) {

  glGraphComposite=new GlGraphComposite(graph);

  assert(in.substr(0,7)=="<scene>");
  unsigned int currentPosition=7;
  GlXMLTools::enterDataNode(in,currentPosition);
  GlXMLTools::setWithXML(in,currentPosition,"viewport",viewport);
  GlXMLTools::setWithXML(in,currentPosition,"background",backgroundColor);
  GlXMLTools::leaveDataNode(in,currentPosition);

  GlXMLTools::enterChildNode(in,currentPosition);
  string childName=GlXMLTools::enterChildNode(in,currentPosition);

  while(childName!="") {
    assert(childName=="GlLayer");

    map<string,string> properties=GlXMLTools::getProperties(in,currentPosition);

    assert(properties.count("name")!=0);

    GlLayer *newLayer=new GlLayer(properties["name"]);
    addLayer(newLayer);
    newLayer->setWithXML(in,currentPosition);

    GlXMLTools::leaveChildNode(in,currentPosition,"GlLayer");

    childName=GlXMLTools::enterChildNode(in,currentPosition);
  }

  getLayer("Main")->addGlEntity(glGraphComposite,"graph");
}

void GlScene::setViewportZoom(int zoom,int xDec, int yDec) {
  viewportZoom=zoom;
  xDecViewport=xDec;
  yDecViewport=yDec;
}

void GlScene::getViewportZoom(int &zoom,int &xDec, int &yDec) {
  zoom=viewportZoom;
  xDec=xDecViewport;
  yDec=yDecViewport;
}

BoundingBox GlScene::getBoundingBox() {
  return lodCalculator->getSceneBoundingBox();
}

}

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
#include <tulip/GlMetaNodeRenderer.h>

#include <tulip/OpenGlConfigManager.h>

#include <tulip/DrawingTools.h>
#include <tulip/BoundingBox.h>
#include <tulip/Camera.h>
#include <tulip/GlGraphInputData.h>
#include <tulip/GlCPULODCalculator.h>
#include <tulip/GlNode.h>
#include <tulip/GlGraphComposite.h>
#include <tulip/Glyph.h>

using namespace std;

namespace tlp {

GlMetaNodeRenderer::GlMetaNodeRenderer(GlGraphInputData *inputData) : _inputData(inputData) {}

GlMetaNodeRenderer::~GlMetaNodeRenderer() {
    clearScenes();
}

void GlMetaNodeRenderer::setInputData(GlGraphInputData *inputData) {
    _inputData = inputData;
}

GlGraphInputData *GlMetaNodeRenderer::getInputData() {
    return _inputData;
}

void GlMetaNodeRenderer::render(node n,float,Camera* camera) {

    GLint renderMode;
    glGetIntegerv(GL_RENDER_MODE,&renderMode);

    if(renderMode==GL_SELECT)
        return;

    Graph *metaGraph = _inputData->getGraph()->getNodeMetaInfo(n);

    GlScene *scene = NULL;
    if(_metaGraphToSceneMap.count(metaGraph)!=0) {
        scene=_metaGraphToSceneMap[metaGraph];
    }
    else {
        scene = createScene(metaGraph);
        _metaGraphToSceneMap[metaGraph]=scene;
        metaGraph->addListener(this);
    }

    bool viewMetaLabels = _inputData->renderingParameters()->isViewMetaLabel();//Checks if user want to see metanode labels
    scene->getGlGraphComposite()->getRenderingParametersPointer()->setViewEdgeLabel(viewMetaLabels);
    scene->getGlGraphComposite()->getRenderingParametersPointer()->setViewNodeLabel(viewMetaLabels);

    GlNode glNode(n.id);

    BoundingBox bb=glNode.getBoundingBox(_inputData);
    BoundingBox bbTmp;
    BoundingBox includeBB;
    _inputData->glyphs.get(_inputData->getElementShape()->getNodeValue(n))->getIncludeBoundingBox(includeBB,n);
    bbTmp[0]=bb.center()-Coord((bb.width()/2.f)*(includeBB[0][0]*-2.f),(bb.height()/2.f)*(includeBB[0][1]*-2.f),(bb.depth()/2.f)*(includeBB[0][2]*-2.f));
    bbTmp[1]=bb.center()+Coord((bb.width()/2.f)*(includeBB[1][0]*2.f),(bb.height()/2.f)*(includeBB[1][1]*2.f),(bb.depth()/2.f)*(includeBB[1][2]*2.f));
    bb=bbTmp;

    Coord eyeDirection=camera->getEyes()-camera->getCenter();
    eyeDirection = eyeDirection/eyeDirection.norm();

    Camera newCamera2=*camera;
    newCamera2.setEyes(newCamera2.getCenter()+Coord(0,0,1)*(newCamera2.getEyes()-newCamera2.getCenter()).norm());
    newCamera2.setUp(Coord(0,1,0));

    Coord first=newCamera2.worldTo2DScreen((Coord)(bb[0]));
    Coord second=newCamera2.worldTo2DScreen((Coord)(bb[1]));

    Coord center=camera->worldTo2DScreen((Coord)(bb[0]+bb[1])/2.f);
    Coord size=second-first;

    Vector<int,4> viewport;
    viewport[0]=center[0]-size[0]/2;
    viewport[1]=center[1]-size[1]/2;
    viewport[2]=size[0];
    viewport[3]=size[1];

    viewport[0]=camera->getViewport()[0]+viewport[0]-viewport[2]/2;
    viewport[1]=camera->getViewport()[1]+viewport[1]-viewport[3]/2;
    viewport[2]*=2;
    viewport[3]*=2;

    if(viewport[2]==0 || viewport[3]==0)
        return;

    scene->setViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
    scene->setClearBufferAtDraw(false);
    scene->centerScene();

    float baseNorm=(scene->getGraphLayer()->getCamera().getEyes()-scene->getGraphLayer()->getCamera().getCenter()).norm();
    Camera newCamera=scene->getGraphLayer()->getCamera();
    Camera *oldCamera=new Camera(scene,true);
    newCamera.setScene(scene);
    *oldCamera=newCamera;
    newCamera.setScene(scene);
    newCamera.setUp(camera->getUp());
    newCamera.setEyes(newCamera.getCenter()+(eyeDirection*baseNorm));
    newCamera.setZoomFactor(newCamera.getZoomFactor()*0.5);
    scene->getGraphLayer()->setSharedCamera(&newCamera);

    scene->draw();

    scene->getGraphLayer()->setCamera(oldCamera);

    camera->getScene()->setClearBufferAtDraw(false);
    camera->getScene()->initGlParameters();
    camera->getScene()->setClearBufferAtDraw(true);
    camera->initGl();
}

GlScene *GlMetaNodeRenderer::createScene(Graph* metaGraph) const {
    GlScene *scene = new GlScene (new GlCPULODCalculator());
    GlLayer* layer=new GlLayer("Main");
    scene->addExistingLayer(layer);
    GlGraphComposite* graphComposite=new GlGraphComposite(metaGraph, scene);
    layer->addGlEntity(graphComposite,"graph");
    graphComposite->getRenderingParametersPointer()->setViewNodeLabel(true);
    graphComposite->getRenderingParametersPointer()->setEdgeColorInterpolate(false);
    graphComposite->getRenderingParametersPointer()->setNodesStencil(0x0002);
    graphComposite->getRenderingParametersPointer()->setNodesLabelStencil(0x0001);
    return scene;
}

void GlMetaNodeRenderer::treatEvent(const Event &e) {
    if(e.type() == Event::TLP_DELETE) {
        delete _metaGraphToSceneMap[reinterpret_cast<Graph*>(e.sender())];
        _metaGraphToSceneMap.erase(reinterpret_cast<Graph*>(e.sender()));
    }
}

void GlMetaNodeRenderer::clearScenes() {
    for(map<Graph *,GlScene *>::iterator it=_metaGraphToSceneMap.begin(); it!=_metaGraphToSceneMap.end(); ++it) {
        delete (*it).second;
    }

    _metaGraphToSceneMap.clear();
}

}




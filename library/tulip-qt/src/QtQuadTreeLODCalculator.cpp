#include "tulip/QtQuadTreeLODCalculator.h"

#include <tulip/Matrix.h>

#include <tulip/Camera.h>
#include <tulip/GlEntity.h>
#include <tulip/GlTools.h>
#include <tulip/GlScene.h>

#include <iostream>

#include "tulip/QuadTree.h"

using namespace std;

namespace tlp {

  BBox computeNewBoundingBox(const BBox &box,const Coord &centerScene,float aX,float aZ) {
    Coord size=(box.second-box.first)/2.;
    Coord center=box.first+size;
    size=Coord(size.norm(),size.norm(),size.norm());
    center[0]=centerScene[0]+(cos(aZ)*(center[0]-centerScene[0]));
    center[1]=centerScene[1]+(cos(aX)*(center[1]-centerScene[1]));
    return pair<Coord,Coord>(center-size,center+size);
  }

  QtQuadTreeLODCalculator::QtQuadTreeLODCalculator() : scene(NULL),nodesQuadTree(NULL),edgesQuadTree(NULL),nodesSelectedQuadTree(NULL),edgesSelectedQuadTree(NULL),entitiesQuadTree(NULL),
                                                       haveToCompute(true),inputData(NULL),currentGraph(NULL),layoutProperty(NULL),sizeProperty(NULL),selectionProperty(NULL) {
  }

  QtQuadTreeLODCalculator::~QtQuadTreeLODCalculator() {
    removeObservers();
  }

  void QtQuadTreeLODCalculator::setScene(GlScene *scene){
    if(this->scene)
      removeObservers();
    this->scene=scene;
    haveToCompute=true;
  }

  bool QtQuadTreeLODCalculator::needEntities(){
    if(haveToCompute)
      return true;

    for(vector<pair<Camera *,Camera> >::iterator it=cameras.begin();it!=cameras.end();++it){
      if(((*it).first)->is3D()){
        Camera *camera=(*it).first;
        Camera *oldCamera=&(*it).second;
        Coord unitCamera=camera->getEyes()-camera->getCenter();
        unitCamera=unitCamera/unitCamera.norm();
        Coord unitOldCamera=oldCamera->getEyes()-oldCamera->getCenter();
        unitOldCamera=unitOldCamera/unitOldCamera.norm();
        if(unitCamera!=unitOldCamera){
          haveToCompute=true;
          return true;
        }
      }
    }

    return false;
  }

  void QtQuadTreeLODCalculator::setNeedEntities(bool need){
    haveToCompute=true;
    removeObservers();
  }

  void QtQuadTreeLODCalculator::addSimpleEntityBoundingBox(unsigned long entity,const BoundingBox& bb) {
    QtCPULODCalculator::addSimpleEntityBoundingBox(entity,bb);
    entitiesGlobalBoundingBox.check(bb.first);
    entitiesGlobalBoundingBox.check(bb.second);
  }
  void QtQuadTreeLODCalculator::addNodeBoundingBox(unsigned int id,const BoundingBox& bb) {
    QtCPULODCalculator::addNodeBoundingBox(id,bb);
    nodesGlobalBoundingBox.check(bb.first);
    nodesGlobalBoundingBox.check(bb.second);
  }
  void QtQuadTreeLODCalculator::addEdgeBoundingBox(unsigned int id,const BoundingBox& bb) {
    QtCPULODCalculator::addEdgeBoundingBox(id,bb);
    edgesGlobalBoundingBox.check(bb.first);
    edgesGlobalBoundingBox.check(bb.second);
  }

  void QtQuadTreeLODCalculator::compute(const Vector<int,4>& globalViewport,const Vector<int,4>& currentViewport,RenderingEntitiesFlag type) {
    this->type=type;
    if(haveToCompute){
      addObservers();

      cameras.clear();
      simpleEntities.clear();

      for(VectorOfCamera::iterator it=cameraVector.begin();it!=cameraVector.end();++it){
        cameras.push_back(pair<Camera*,Camera>((Camera*)(*it),*((Camera *)(*it))));
      }

      int i=0;
      for(VectorOfCamera::iterator it=cameraVector.begin();it!=cameraVector.end();++it){
        Camera *camera=(Camera*)(*it);

        simpleResultVector.push_back(std::vector<LODResultSimpleEntity>());
        nodesResultVector.push_back(std::vector<LODResultComplexEntity>());
        edgesResultVector.push_back(std::vector<LODResultComplexEntity>());

        Matrix<float,4> transformMatrix;
        camera->getTransformMatrix(globalViewport,transformMatrix);

        Coord eye;
        if(camera->is3D()) {
          currentCamera=camera;
          eye=camera->getEyes() + ( camera->getEyes() -camera->getCenter() ) / camera->getZoomFactor();
          computeFor3DCamera(&simpleBoundingBoxVector[i],&nodesBoundingBoxVector[i],&edgesBoundingBoxVector[i],
              &(simpleResultVector.back()),&(nodesResultVector.back()),&(edgesResultVector.back()),
              eye,transformMatrix,globalViewport,currentViewport);
        }else{
          simpleEntities.push_back(simpleBoundingBoxVector[i]);
          computeFor2DCamera(&simpleBoundingBoxVector[i],&nodesBoundingBoxVector[i],&edgesBoundingBoxVector[i],
              &(simpleResultVector.back()),&(nodesResultVector.back()),&(edgesResultVector.back()),
              globalViewport,currentViewport);
        }

        glMatrixMode(GL_MODELVIEW);
        ++i;
      }

    }else{
      int i=0;
      for(vector<pair<Camera*,Camera> >::iterator it=cameras.begin();it!=cameras.end();++it){
        cameraVector.push_back((unsigned long)((*it).first));
        simpleResultVector.push_back(std::vector<LODResultSimpleEntity>());
        nodesResultVector.push_back(std::vector<LODResultComplexEntity>());
        edgesResultVector.push_back(std::vector<LODResultComplexEntity>());

        Camera *camera=(Camera*)((*it).first);
        Matrix<float,4> transformMatrix;
        camera->getTransformMatrix(globalViewport,transformMatrix);

        Coord eye;
        if(camera->is3D()) {
          currentCamera=camera;
          eye=camera->getEyes() + ( camera->getEyes() -camera->getCenter() ) / camera->getZoomFactor();
          computeFor3DCamera(NULL,NULL,NULL,
              &(simpleResultVector.back()),&(nodesResultVector.back()),&(edgesResultVector.back()),
              eye,transformMatrix,globalViewport,currentViewport);
        }else{
          computeFor2DCamera(&simpleEntities[i],new ComplexBoundingBoxVector(),new ComplexBoundingBoxVector(),
              &(simpleResultVector.back()),&(nodesResultVector.back()),&(edgesResultVector.back()),
              globalViewport,currentViewport);
          ++i;
        }
      }

    }
  }

  void QtQuadTreeLODCalculator::computeFor3DCamera(SimpleBoundingBoxVector *inputSimple,ComplexBoundingBoxVector *inputNodes,ComplexBoundingBoxVector *inputEdges,
      SimpleLODResultVector *outputSimple, ComplexLODResultVector *outputNodes, ComplexLODResultVector *outputEdges,
      const Coord &eye,
      const Matrix<float, 4> transformMatrix,
      const Vector<int,4>& globalViewport,
      const Vector<int,4>& currentViewport) {

    BooleanProperty *selectedProperty=NULL;
    if(inputData)
      selectedProperty=inputData->getGraph()->getProperty<BooleanProperty>(inputData->getElementSelectedPropName());

    //backup old rotation
    Coord oldEyes=currentCamera->getEyes();
    Coord oldCenter=currentCamera->getCenter();
    Coord oldUp=currentCamera->getUp();


    //remove rotation of the camera
    Coord newUp=projectPoint(oldEyes+oldUp,transformMatrix,globalViewport)-projectPoint(oldEyes,transformMatrix,globalViewport);
    currentCamera->setEyes(oldCenter+Coord(0,0,(oldEyes-oldCenter).norm()));
    currentCamera->setUp(Coord(newUp[0]/newUp.norm(),newUp[1]/newUp.norm(),0));

    Matrix<float,4> newTransformMatrix;
    currentCamera->getTransformMatrix(globalViewport,newTransformMatrix);
    MatrixGL tmp(newTransformMatrix);
    tmp.inverse();

    BoundingBox cameraBoundingBox;
    Coord pScr = projectPoint(Coord(0,0,0), newTransformMatrix, globalViewport);
    pScr[0] = currentViewport[0];
    pScr[1] = globalViewport[3]-(currentViewport[1]+currentViewport[3]);
    cameraBoundingBox.check(unprojectPoint(pScr, tmp, globalViewport));
    pScr[0] = currentViewport[0]+currentViewport[2];
    cameraBoundingBox.check(unprojectPoint(pScr, tmp, globalViewport));
    pScr[1] = globalViewport[3]-currentViewport[1];
    cameraBoundingBox.check(unprojectPoint(pScr, tmp, globalViewport));
    pScr[0] = currentViewport[0];
    cameraBoundingBox.check(unprojectPoint(pScr, tmp, globalViewport));

    //restore rotation od the camera
    currentCamera->setEyes(oldEyes);
    currentCamera->setUp(oldUp);

    if(haveToCompute){
      if(entitiesQuadTree)
        delete entitiesQuadTree;
      entitiesQuadTree = new QuadTreeNode(entitiesGlobalBoundingBox);
      if(nodesQuadTree)
        delete nodesQuadTree;
      nodesQuadTree = new QuadTreeNode(nodesGlobalBoundingBox);
      if(edgesQuadTree)
        delete edgesQuadTree;
      edgesQuadTree = new QuadTreeNode(edgesGlobalBoundingBox);
      if(nodesSelectedQuadTree)
        delete nodesSelectedQuadTree;
      nodesSelectedQuadTree = new QuadTreeNode(nodesGlobalBoundingBox);
      if(edgesSelectedQuadTree)
        delete edgesSelectedQuadTree;
      edgesSelectedQuadTree = new QuadTreeNode(edgesGlobalBoundingBox);


      Coord eyeCenter=currentCamera->getCenter()-currentCamera->getEyes();
      float aX=atan(eyeCenter[1]/eyeCenter[2]);
      float aZ=atan(eyeCenter[0]/eyeCenter[2]);

      for(SimpleBoundingBoxVector::iterator it=inputSimple->begin();it!=inputSimple->end();++it){
        if(aX==0 && aZ==0){
          entitiesQuadTree->insert((*it).second,(*it).first);
        }else{
          entitiesQuadTree->insert(computeNewBoundingBox((*it).second,currentCamera->getCenter(),aX,aZ),(*it).first);
        }
      }
      for(ComplexBoundingBoxVector::iterator it=inputNodes->begin();it!=inputNodes->end();++it){
        assert(selectedProperty);
        QuadTreeNode *quadTree;
        if(selectedProperty->getNodeValue(node((*it).first)))
          quadTree=nodesSelectedQuadTree;
        else
          quadTree=nodesQuadTree;

        if(aX==0 && aZ==0){
          quadTree->insert((*it).second,(*it).first);
        }else{
          quadTree->insert(computeNewBoundingBox((*it).second,currentCamera->getCenter(),aX,aZ),(*it).first);
        }
      }
      for(ComplexBoundingBoxVector::iterator it=inputEdges->begin();it!=inputEdges->end();++it){
        assert(selectedProperty);
        QuadTreeNode *quadTree;
        if(selectedProperty->getEdgeValue(edge((*it).first)))
          quadTree=edgesSelectedQuadTree;
        else
          quadTree=edgesQuadTree;

        if(aX==0 && aZ==0){
          quadTree->insert((*it).second,(*it).first);
        }else{
          quadTree->insert(computeNewBoundingBox((*it).second,currentCamera->getCenter(),aX,aZ),(*it).first);
        }
      }

      haveToCompute = false;
    }

    SimpleBoundingBoxVector simples;
    ComplexBoundingBoxVector nodes;
    ComplexBoundingBoxVector edges;

    vector<unsigned int> resNodes;
    vector<unsigned int> resEdges;
    vector<unsigned long> resEntities;

    int ratio;
    if(currentViewport[2]>currentViewport[3])
      ratio=currentViewport[2];
    else
      ratio=currentViewport[3];


    if((type & RenderingNodes)!=0){
      if((type & RenderingWithoutRemove)==0){
        nodesQuadTree->getElementsWithRatio(cameraBoundingBox,&resNodes,NULL,ratio);
        nodesSelectedQuadTree->getElementsWithRatio(cameraBoundingBox,&resNodes,NULL,ratio);
      }else{
        nodesQuadTree->getElements(cameraBoundingBox,&resNodes,NULL);
        nodesSelectedQuadTree->getElements(cameraBoundingBox,&resNodes,NULL);
      }

      for(unsigned int i=0;i<resNodes.size();++i){
        nodes.push_back(pair<unsigned int,BoundingBox>(resNodes[i],GlNode(resNodes[i]).getBoundingBox(inputData)));
      }
    }
    if((type & RenderingEdges)!=0){
      if((type & RenderingWithoutRemove)==0){
        edgesQuadTree->getElementsWithRatio(cameraBoundingBox,&resEdges,NULL,ratio);
        edgesSelectedQuadTree->getElementsWithRatio(cameraBoundingBox,&resEdges,NULL,ratio);
      }else{
        edgesQuadTree->getElements(cameraBoundingBox,&resEdges,NULL);
        edgesSelectedQuadTree->getElements(cameraBoundingBox,&resEdges,NULL);
      }

      for(unsigned int i=0;i<resEdges.size();++i){
        edges.push_back(pair<unsigned int,BoundingBox>(resEdges[i],GlEdge(resEdges[i]).getBoundingBox(inputData)));
      }
    }
    if((type & RenderingSimpleEntities)!=0){
      if((type & RenderingWithoutRemove)==0)
        entitiesQuadTree->getElementsWithRatio(cameraBoundingBox,NULL,&resEntities,ratio);
      else
        entitiesQuadTree->getElements(cameraBoundingBox,NULL,&resEntities);
      for(unsigned int i=0;i<resEntities.size();++i){
        simples.push_back(pair<unsigned long,BoundingBox>(resEntities[i],((GlSimpleEntity*)(resEntities[i]))->getBoundingBox()));
      }
    }

    QtCPULODCalculator::computeFor3DCamera(&simples,&nodes,&edges,
          outputSimple, outputNodes, outputEdges,
          eye,transformMatrix,globalViewport,currentViewport);
  }

  void QtQuadTreeLODCalculator::removeObservers() {
    if(inputData){
      if(currentGraph)
        currentGraph->removeGraphObserver(this);
      if(layoutProperty)
        layoutProperty->removeObserver(this);
      if(sizeProperty)
        sizeProperty->removeObserver(this);
      if(selectionProperty)
        selectionProperty->removeObserver(this);
    }
    scene->removeObserver(this);
  }

  void QtQuadTreeLODCalculator::addObservers() {
    if(inputData){
      currentGraph=inputData->getGraph();
      currentGraph->addGraphObserver(this);
      layoutProperty=currentGraph->getProperty(inputData->getElementLayoutPropName());
      layoutProperty->addObserver(this);
      sizeProperty=currentGraph->getProperty(inputData->getElementSizePropName());
      sizeProperty->addObserver(this);
      selectionProperty=currentGraph->getProperty(inputData->getElementSelectedPropName());
      selectionProperty->addObserver(this);
    }
    scene->addObserver(this);
  }

  void QtQuadTreeLODCalculator::setInputData(GlGraphInputData *newInputData){
    if(inputData)
      removeObservers();

    inputData=newInputData;
    haveToCompute=true;
  }

  void QtQuadTreeLODCalculator::update(std::set<Observable *>::iterator begin ,std::set<Observable *>::iterator end){
    haveToCompute=true;
    removeObservers();
  }

  void QtQuadTreeLODCalculator::addLocalProperty(Graph*, const std::string &name){
    if(name==inputData->getElementLayoutPropName() || name==inputData->getElementSizePropName()){
      haveToCompute=true;
      removeObservers();
      addObservers();
    }
  }
  void QtQuadTreeLODCalculator::delLocalProperty(Graph*, const std::string &name){
    if(name==inputData->getElementLayoutPropName() || name==inputData->getElementSizePropName()){
      haveToCompute=true;
      removeObservers();
      addObservers();
    }
  }

  void QtQuadTreeLODCalculator::clear() {
      simpleBoundingBoxVector.clear();
      nodesBoundingBoxVector.clear();
      edgesBoundingBoxVector.clear();
      simpleResultVector.clear();
      nodesResultVector.clear();
      edgesResultVector.clear();
      cameraVector.clear();
    }

}


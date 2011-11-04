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
#ifndef Tulip_GLCPULODCALCULATOR_H
#define Tulip_GLCPULODCALCULATOR_H

#include <map>
#include <vector>

#include <tulip/BoundingBox.h>
#include <tulip/Coord.h>
#include <tulip/GlLODCalculator.h>

namespace tlp {
class GlSimpleEntity;
class Camera;

/**
 * \brief Class used to compute LOD of GlEntities with OpenMP parallelization
 *
 * This class perform LOD computation of GlEntities based on screen projection of entities bounding boxes
 * \warning By default this class don't compute LOD for edges (for optimisation) and return a lod of 10. to these edges, if you want to compute edges' LOD call setComputeEdgesLOD(true)
 */
class TLP_GL_SCOPE GlCPULODCalculator : public GlLODCalculator {

public:

  GlCPULODCalculator();
  virtual ~GlCPULODCalculator();
  virtual GlLODCalculator *clone() {
    GlCPULODCalculator *calculator=new GlCPULODCalculator();
    calculator->setComputeOutScreenLOD(computeOutScreenLOD);
    return calculator;
  }

  /**
   * Begin a new camera (use to render next entities)
   */
  virtual void beginNewCamera(Camera* camera);
  /**
   * This function is call by GlLODSceneVisitor when a simple entitie is found
   */
  virtual void addSimpleEntityBoundingBox(GlSimpleEntity * entity, const BoundingBox& bb);
  /**
   * This function is call by GlLODSceneVisitor when a node is found
   */
  virtual void addNodeBoundingBox(unsigned int id,const BoundingBox& bb);
  /**
   * This function is call by GlLODSceneVisitor when an edge is found
   */
  virtual void addEdgeBoundingBox(unsigned int id,const BoundingBox& bb);

  /**
   * Reserve memory to store nodes LOD, this function is an optimisation function
   */
  virtual void reserveMemoryForNodes(unsigned int numberOfNodes);

  /**
   * Reserve memory to store edges LOD, this function is an optimisation function
   */
  virtual void reserveMemoryForEdges(unsigned int numberOfEdges);

  /**
   * Compute all bounding boxes
   * If you want to compute LOD for a simple scene, you just have to call this function with same value on globalViewport and currentViewport
   * But if you want to perform a sub screen part selection you have to call this function with : globalViewport the viewport of the visualisation and currentViewport the viewport of the selection
   * \param globalViewport is used to compute LOD
   * \param currentViewport : return -1 for all entities outside this viewport
   */
  virtual void compute(const Vector<int,4>& globalViewport,const Vector<int,4>& currentViewport);

  /**
   * This function return the scene bounding box
   */
  virtual BoundingBox getSceneBoundingBox() {
    return sceneBoundingBox;
  }

  /**
   * Set if the edge LOD must be calculated
   * \Warning at default the edge LOD is not calculated and return 10.
   */
  void setComputeEdgesLOD(bool state) {
    computeEdgesLOD=state;
  }

  /**
   * Set if the LOD is computed for out screen entities
   */
  void setComputeOutScreenLOD(bool state) {
    computeOutScreenLOD=state;
  }


protected:

  virtual void computeFor3DCamera(LayerLODUnit *layerLODUnit, const Coord &eye, const Matrix<float, 4> transformMatrix, const Vector<int,4>& globalViewport,const Vector<int,4>& currentViewport);

  virtual void computeFor2DCamera(LayerLODUnit *layerLODUnit,const Vector<int,4>& globalViewport,const Vector<int,4>& currentViewport);

  bool computeEdgesLOD;
  bool computeOutScreenLOD;

  BoundingBox sceneBoundingBox;

  LayerLODUnit *currentLayerLODUnit;

};

}

#endif // Tulip_GLCPULODCALCULATOR_H

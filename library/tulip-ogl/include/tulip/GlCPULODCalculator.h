//-*-c++-*-
/**
 Author: Morgan Mathiaut
 Email : mathiaut@labri.fr
 Last modification : 25/10/2007
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/
#ifndef Tulip_GLCPULODCALCULATOR_H
#define Tulip_GLCPULODCALCULATOR_H
#ifndef DOXYGEN_NOTFOR_DEVEL

#include <map>
#include <vector>

#include <tulip/BoundingBox.h>

#include "tulip/GlLODCalculator.h"

namespace tlp {
  
  class Camera;

  /**
   * Class use to compute bounding boxs of a vector of GlEntity
   */
  class TLP_GL_SCOPE GlCPULODCalculator : public GlLODCalculator {
  
  protected:

    typedef std::pair<unsigned long, BoundingBox> BoundingBoxUnit;
    typedef std::vector<BoundingBoxUnit> BoundingBoxVector;
    typedef std::pair<unsigned long, std::pair<BoundingBoxVector*,BoundingBoxVector*> > CameraAndBoundingBoxUnit;
    typedef std::vector<CameraAndBoundingBoxUnit> CameraAndBoundingBoxVector;

  public:

    virtual ~GlCPULODCalculator();

    /**
     * Begin a new camera (use to render next entities)
     */
    virtual void beginNewCamera(Camera* camera);
    /**
     * Add a bounding box for a simple entity
     */
    virtual void addSimpleEntityBoundingBox(unsigned long entity,const BoundingBox& bb);
    /**
     * Add a bounding box for a complexe entity
     */
    virtual void addComplexeEntityBoundingBox(unsigned long entity,const BoundingBox& bb);

    /**
     * Compute all bounding boxs with the given viewport
     */
    virtual void compute(const Vector<int,4>& globalViewport,const Vector<int,4>& currentViewport);

    virtual void computeFor3DCamera(const std::pair<BoundingBoxVector*,BoundingBoxVector*> &entities,
				    const LODResultVector::iterator &itSEOutput, 
				    const LODResultVector::iterator &itCEOutput,
				    const Coord &eye,
				    const Matrix<float, 4> transformMatrix,
				    const Vector<int,4>& globalViewport,
				    const Vector<int,4>& currentViewport);

    void computeFor2DCamera(const std::pair<BoundingBoxVector*,BoundingBoxVector*> &entities,
			    const LODResultVector::iterator &itSEOutput, 
			    const LODResultVector::iterator &itCEOutput,
			    const Vector<int,4>& globalViewport,
			    const Vector<int,4>& currentViewport);
    
    /**
     * Return the result vector for simpleEntities
     */
    virtual LODResultVector* getResultForSimpleEntities();
    /**
     * Return the result vector for complexeEntities
     */
    virtual LODResultVector* getResultForComplexeEntities();

    /**
     * Clear the class data
     */
    virtual void clear();

  protected:
    
    BoundingBoxVector* actualSEBoundingBoxVector;
    BoundingBoxVector* actualCEBoundingBoxVector;
    CameraAndBoundingBoxVector boundingBoxVector;

    LODResultVector seResultVector;
    LODResultVector ceResultVector;

  };
 
}

#endif // DOXYGEN_NOTFOR_DEVEL

#endif // Tulip_GLCPULODCALCULATOR_H

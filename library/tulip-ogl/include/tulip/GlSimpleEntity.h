//-*-c++-*-
/**
 Author: Morgan Mathiaut
 Email : mathiaut@labri.fr
 Last modification : 23/10/2007
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
*/
#ifndef Tulip_GLSIMPLEENTITY_H
#define Tulip_GLSIMPLEENTITY_H

#include <vector>

#include "tulip/GlEntity.h"
#include "tulip/GlSceneVisitor.h"
#include "tulip/Camera.h"
#include "tulip/GlXMLTools.h"
#include <tulip/BoundingBox.h>

namespace tlp {

  class GlComposite;

  /**
   * Base class for all simple entity (entity who not need GraphInputData
   */
  class TLP_GL_SCOPE GlSimpleEntity : public GlEntity {

  public:

    GlSimpleEntity():visible(true),stencil(0xFFFF),checkByBoundingBoxVisitor(true) {}

    /**
     * Draw function
     */
    virtual void draw(float lod,Camera* camera) = 0;

    /**
     * Accept visitor function
     */
    virtual void acceptVisitor(GlSceneVisitor *visitor) {
      visitor->visit(this);
    }

    /**
     * Set if entity is visible
     */
    void setVisible(bool visible);
    /**
     * Return if entity is visible
     */
    bool isVisible() {return visible;}
    /**
     * Set stencil number of the entity
     */
    virtual void setStencil(int stencil) {this->stencil=stencil;}
    /**
     * Return stencil number of entity
     */
    int getStencil() {return stencil;}
    /**
     * Set if the entity is check by boundingbox visitor
     */
    void setCheckByBoundingBoxVisitor(bool check) {checkByBoundingBoxVisitor=check;}
    /**
     * Return if entity is check by boudingbox visitor
     */
    bool isCheckByBoundingBoxVisitor() {return checkByBoundingBoxVisitor;}

    /**
     * Return the entity boundingbox
     */
    virtual BoundingBox getBoundingBox() {return boundingBox;}

    /**
     * Add a layer parent to this entity
     */
    virtual void addLayerParent(GlLayer *layer) {}

    /**
     * remove a layer parent to this entity
     */
    virtual void removeLayerParent(GlLayer *layer) {}

    /**
     * Add a parent to this entity
     */
    void addParent(GlComposite *composite) {parents.push_back(composite);}

    /**
     * remove a parent to this entity
     */
    void removeParent(GlComposite *composite) {
      for(std::vector<GlComposite*>::iterator it=parents.begin();it!=parents.end();++it){
        if((*it)==composite){
          parents.erase(it);
          return;
        }
      }
    }

    /**
     * virtual fucntion : Translate entity
     */
    virtual void translate(const Coord &mouvement){};

    /**
     * Save the entity in Xml
     */
    virtual void getXML(xmlNodePtr rootNode) =0;

    /**
     * Load entity with Xml
     */
    virtual void setWithXML(xmlNodePtr rootNode) =0;

  protected:

    bool visible;
    int stencil;
    bool checkByBoundingBoxVisitor;

    BoundingBox boundingBox;

    std::vector<GlComposite*> parents;

  };

}

#endif // Tulip_GLSIMPLEENTITY_H

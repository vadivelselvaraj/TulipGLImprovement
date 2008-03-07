//-*-c++-*-
/**
 Author: Morgan Mathiaut
 Email : mathiaut@labri.fr
 Last modification : 23/01/2008
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/
#ifndef Tulip_GLXMLTOOLS_H
#define Tulip_GLXMLTOOLS_H

#include <tulip/tulipconf.h>

#include <sstream>
#include <vector>

#include <tulip/Array.h>
#include <tulip/Color.h>
#include <tulip/Coord.h>

typedef struct _xmlNode xmlNode;
typedef xmlNode * xmlNodePtr;

namespace tlp {

  /**
   * static tools class use to store/load xml data 
   */
  class GlSimpleEntity;

  class TLP_GL_SCOPE GlXMLTools {

  public :

    /**
     * Create a data and a child node in rootNode
     */
    static void createDataAndChildrenNodes(xmlNodePtr rootNode,xmlNodePtr &dataNode, xmlNodePtr &childrenNode);
    /**
     * Create a data node in rootNode
     */
    static void createDataNode(xmlNodePtr rootNode,xmlNodePtr &dataNode);
    /**
     * Create a child node with a given name in rootNode
     */
    static void createChild(xmlNodePtr rootNode, const std::string &name, xmlNodePtr &childNode);
    /**
     * Create a property with name and value in rootNode
     */
    static void createProperty(xmlNodePtr rootNode, const std::string &name, const std::string &value);
    /**
     * Add a text content in rootNode
     */
    static void addContent(xmlNodePtr rootNode,const std::string &content);
    /**
     * Get the data and child node of rootNode
     */
    static void getDataAndChildrenNodes(xmlNodePtr rootNode,xmlNodePtr &dataNode, xmlNodePtr &childrenNode);
    /**
     * Get the data node of rootNode
     */
    static void getDataNode(xmlNodePtr rootNode,xmlNodePtr &dataNode);
    /**
     * Get the data node in the same level of rootNode
     */
    static void getDataNodeDirectly(xmlNodePtr rootNode,xmlNodePtr &dataNode);
    /**
     * Get the data with name of dataNode
     */
    static void getData(const std::string &name, xmlNodePtr dataNode, xmlNodePtr &outNode);
    /**
     * Get the property with name of node
     */
    static std::string getProperty(const std::string &name, xmlNodePtr node);
    /**
     * Get the text content of the rootNode
     */
    static void getContent(xmlNodePtr rootNode,std::string &content);
    /**
     * Create a GlEntity with the given name
     */
    static GlSimpleEntity *createEntity(const std::string &name);

    /**
     * Get the XML output for a vector of Object
     */
    template <typename Obj>
      static void getXML(xmlNodePtr rootNode,const std::string &name,const typename std::vector<Obj> &vect)
    {
      xmlNodePtr node;
      createChild(rootNode,name,node);
    
      std::stringstream str;
      str << "(" ;
      typename std::vector<Obj>::const_iterator it=vect.begin();
      str << *it ;
      for(;it!=vect.end();++it) {
	str << "," << *it ;
      }
      str << ")" ;
      addContent(node,str.str());
    }
    
    /**
     * Set vector of Object with the given XML
     */
    template <typename Obj>
      static void setWithXML(xmlNodePtr rootNode,const std::string &name,std::vector<Obj> &vect)
    {
      xmlNodePtr node;
      GlXMLTools::getData(name, rootNode, node);
      std::string tmp;
      getContent(node,tmp);
      std::istringstream is(tmp);
      Obj data;
      char c=is.get();
      while(c!=')'){
	is >> data ;
	vect.push_back(data);
	c=is.get();
      }
    }
    
    /**
     * Get the XML output for an Object
     */
    template <typename Obj>
      static void getXML(xmlNodePtr rootNode, const std::string &name, const Obj &value)
    {
      xmlNodePtr node;
      createChild(rootNode,name,node);
      std::stringstream str;
      str << value ;
      addContent(node,str.str());
    }
    
    /**
     * Set an Object with the given XML
     */
    template <typename Obj>
      static void setWithXML(xmlNodePtr rootNode, const std::string &name, Obj &value) {
      xmlNodePtr node;
      GlXMLTools::getData(name, rootNode, node);
      std::string tmp;
      getContent(node,tmp);
      std::stringstream str(tmp);
      str >> value;
    }

  };

}

#endif

//-*-c++-*-
#ifndef TULIP_COLORS_H
#define TULIP_COLORS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tulip/AbstractProperty.h"
#include "tulip/TemplateFactory.h"
#include "tulip/ColorAlgorithm.h"

namespace tlp {

class PropertyContext;
/** \addtogroup properties */ 
/*@{*/
class TLP_SCOPE ColorProperty:public AbstractProperty<ColorType,ColorType, ColorAlgorithm> { 
  friend class ColorAlgorithm;
public :
  ColorProperty (Graph *);
  PropertyInterface* clonePrototype(Graph *, std::string );
  void copy( const node, const node, PropertyInterface * );
  void copy( const edge, const edge, PropertyInterface * );
};
/*@}*/

}
#endif


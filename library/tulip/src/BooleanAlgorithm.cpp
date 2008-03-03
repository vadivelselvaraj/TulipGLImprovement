//-*-c++-*-
#include "tulip/BooleanAlgorithm.h"
#include "tulip/Graph.h"
#include "tulip/BooleanProperty.h"

tlp::BooleanAlgorithm::BooleanAlgorithm (const tlp::PropertyContext & context):tlp::PropertyAlgorithm(context) {
  booleanResult = (tlp::BooleanProperty *)context.propertyProxy;
}

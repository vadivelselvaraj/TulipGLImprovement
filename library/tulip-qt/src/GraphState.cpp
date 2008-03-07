#include <tulip/Morphing.h>
#include <tulip/GlMainWidget.h>
#include <tulip/Graph.h>
#include <tulip/LayoutProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/ColorProperty.h>

using namespace tlp;
using namespace std;

//====================================================
static bool HaveSameValues( const vector<vector <Coord> > &v1,
			    const vector<vector <Coord> > &v2 ) {
  if (v1.size() != v2.size()) return false;
  for (unsigned int i = 0; i < v1.size(); ++i) {
    if (v1[i].size() != v2[i].size()) return false;
    for (unsigned int j = 0; j < v1.size(); ++j) {
      if ((v1[i][j] - v2[i][j]).norm() > 1e-6)
	return false;
    }//end for
  }//end for
  return true;
}//end HaveSameValues

//====================================================
static bool HaveSameValues( Graph * inG, 
			    PropertyInterface * inP0, 
			    PropertyInterface * inP1 ) {
  assert( inP0 );
  assert( inP1 );
  assert( inG );
  // Nodes interpolation
  Iterator<node> * nodeIt = inG->getNodes();
  while( nodeIt->hasNext() ) {
    node n = nodeIt->next();
    if( inP0->getNodeStringValue(n) != inP1->getNodeStringValue(n) ) {
      delete nodeIt;
      return false;
    }
  } delete nodeIt;
  // Edges interpolation
  Iterator<edge> * edgeIt = inG->getEdges();
  while( edgeIt->hasNext() ) {
    edge e = edgeIt->next();
    if( inP0->getEdgeStringValue(e) != inP1->getEdgeStringValue(e) ) {
      delete edgeIt;
      return false;
    }
  } delete edgeIt;
  return true;
}
//====================================================
GraphState::GraphState( GlMainWidget * glgw ) {
  assert( glgw );
  g = glgw->getScene()->getGlGraphComposite()->getInputData()->getGraph();
  layout = new LayoutProperty( g );
  *layout = *( g->getProperty<LayoutProperty>("viewLayout") );
  size = new SizeProperty( g );
  *size = *( g->getProperty<SizeProperty>("viewSize") );
  color = new ColorProperty( g );
  *color = *( g->getProperty<ColorProperty>("viewColor") );
  camera = glgw->getScene()->getCamera();
}
//====================================================
GraphState::~GraphState() {
  if( layout ) delete layout;
  if( size ) delete size;
  if( color ) delete color;
}
//====================================================
bool GraphState::setupDiff( Graph * inG, 
			    GraphState * inGS0, 
			    GraphState * inGS1 ) {
  int remain = 4;
  if( HaveSameValues(inG,inGS0->layout,inGS1->layout) ) {
    delete inGS0->layout;
    delete inGS1->layout;
    inGS0->layout = inGS1->layout = 0;
    remain--;
  }
  if( HaveSameValues(inG,inGS0->size,inGS1->size) ) {
    delete inGS0->size;
    delete inGS1->size;
    inGS0->size = inGS1->size = 0;
    remain--;
  }
  if( HaveSameValues(inG,inGS0->color,inGS1->color) ) {
    delete inGS0->color;
    delete inGS1->color;
    inGS0->color = inGS1->color = 0;
    remain--;
  }
  if ( HaveSameValues(inGS0->augPoints, inGS1->augPoints) ) {
    inGS0->augPoints.clear();
    inGS1->augPoints.clear();
    remain--;
  }
  return ( remain > 0 );
}
//====================================================
void 
GraphState::EdgeEnds( Coord & outC0, 
		      Coord & outC1, 
		      edge e ) {
  node n0 = g->source(e);
  node n1 = g->target(e);
  outC0   = layout->getNodeValue( n0 );
  outC1   = layout->getNodeValue( n1 );
}
//====================================================

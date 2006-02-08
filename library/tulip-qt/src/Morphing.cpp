#include <tulip/GlGraphWidget.h>
#include <tulip/SuperGraph.h>
#include <tulip/LayoutProxy.h>
#include <tulip/SizesProxy.h>
#include <tulip/ColorsProxy.h>
#include <tulip/Morphing.h>

#define	MORPHING_MIN_DURATION	1.0f		// s
#define	MORPHING_MAX_DURATION	3.0f		// s
#define	MORPHING_MIN_FRAMES	8		// #


static
void InterpolateColors( Color & outc, const Color & c0, const Color & c1, float inT ) {
  for( int i = 0 ; i < 4 ; i++ ) {
    float f0 = float( c0.array[i] );
    float f1 = float( c1.array[i] );
    outc.array[i] = (unsigned int)( f0 + (f1-f0) * inT );
  }
}


bool AssociateEdges( GraphState  * g0, LayoutProxy * e0, GraphState  * g1,
		     LayoutProxy * e1, edge e ) {
  if( e0->getEdgeStringValue(e) == e1->getEdgeStringValue(e) )
    return false;
  
  std::vector<Coord> ec0, ec1;
  ec0 = e0->getEdgeValue( e );
  ec1 = e1->getEdgeValue( e );
  
  if( ec0.size() == ec1.size() ) return true;
  else if( ec0.size() > ec1.size() ) {
    std::swap( g0, g1 );
    std::swap( e0, e1 );
    std::swap( ec0, ec1 );
  }
  
  Coord c0, c1;
  g0->EdgeEnds(c0, c1, e);
  int n  = ec1.size() - ec0.size();
  int n0 = n >> 1;
  int n1 = n - n0;
  ec0.insert( ec0.begin(), n0, c0 );
  ec0.insert( ec0.end(), n1, c1 );
  e0->setEdgeValue( e, ec0 );
  return true;
}


Morphing::Morphing(){
  g0 = g1 = 0;
  e0 = e1 = 0; 
  tid = -1;
  t = 0.0f;
  glWidget = 0;
}


bool 
Morphing::start( GlGraphWidget * outGlgw, GraphState * inG0, GraphState * inG1) {
  assert( outGlgw );
  assert( inG0 );
  assert( inG1 );
  glWidget = outGlgw;
  SuperGraph * g = outGlgw->getSuperGraph();
  assert( g );
  stop();
  
  bool hasdiff = GraphState::setupDiff( g, inG0, inG1 );
  if( !hasdiff )
    return false;
  
  g0  = inG0;
  g1  = inG1;
  e0  = 0;
  e1  = 0;
  qt0.start();
  tid = startTimer(1000/MORPHING_MAX_FPS);
  frameCpt = 0;
  
  // Edges association
  if( g0->layout && g1->layout ) {
    e0 = new LayoutProxy( g0->g );
    e1 = new LayoutProxy( g1->g );
    *e0 = *(g0->layout);
    *e1 = *(g1->layout);
    e0->setAllNodeValue(Coord(0,0,0));
    e1->setAllNodeValue(Coord(0,0,0));
    
    bool haveSameValues = true;
    Iterator<edge> * edgeIt = g->getEdges();
    while( edgeIt->hasNext() ) {
      edge e = edgeIt->next();
      bool isDiff = AssociateEdges( g0, e0, g1, e1, e );
      if( isDiff )
	haveSameValues = false;
    }
    delete edgeIt;
    
    if( haveSameValues ) {
      delete e0;
      delete e1;
      e0 = e1 = 0;
    }
  }
  
  interpolate( outGlgw, 0.0f );
  return true;
}

void
Morphing::stop( ) {
  if( g0 )	delete g0;
  if( g1 )	delete g1;
  if( e0 )	delete e0;
  if( e1 )	delete e1;
  g0 = g1 = 0;
  e0 = e1 = 0;
  tid = -1;
}

float
Morphing::fps() {
  float dt = float(qt0.elapsed()) / 1000.0f;
  return float(frameCpt) / dt;
}
 
void 
Morphing::interpolate( GlGraphWidget * outGlgw, float inT) {
  frameCpt++;
  
  assert( outGlgw );
  SuperGraph * g = outGlgw->getSuperGraph();
  assert( g );
  //assert( glg );
  
  if (inT < 0.0f) inT = 0.0f; //inT = inT >? 0.0f;
  if (inT > 1.0f) inT = 1.0f; //inT = inT <? 1.0f;
  t = inT;
  
  LayoutProxy * outLayout = g->getProperty<LayoutProxy>( "viewLayout" );
  SizesProxy  * outSize   = g->getProperty<SizesProxy>( "viewSize" );
  ColorsProxy * outColor  = g->getProperty<ColorsProxy>( "viewColor" );
  
  // Nodes interpolation
  Iterator<node> * nodeIt = g->getNodes();
  while( nodeIt->hasNext() ) {
    node n = nodeIt->next();
    
    // Layout
    if( g0->layout && g1->layout ) {
      Coord c0, c1;
      c0 = g0->layout->getNodeValue( n );
      c1 = g1->layout->getNodeValue( n );
      c0 += (c1-c0) * inT;
      outLayout->setNodeValue( n, c0 );
    }
    
    // Size
    if( g0->size && g1->size ) {
      Size s0, s1;
      s0 = g0->size->getNodeValue( n );
      s1 = g1->size->getNodeValue( n );
      s0 += (s1-s0) * inT;
      outSize->setNodeValue( n, s0 );
    }
    
    // Color
    if( g0->color && g1->color ) {
      Color c0, c1;
      c0 = g0->color->getNodeValue( n );
      c1 = g1->color->getNodeValue( n );
      InterpolateColors( c0, c0, c1, inT );
      outColor->setNodeValue( n, c0 );
    }
  }
  delete nodeIt;
  
  // Edges interpolation
  std::vector<Coord> edgeControls, edgeControls0, edgeControls1;
  Iterator<edge> * edgeIt = g->getEdges();
  while( edgeIt->hasNext() ) {
    edge e = edgeIt->next();
    
    // Layout
    if( g0->layout && g1->layout ) {
      if( inT <= 0.0f ) {
	edgeControls = g0->layout->getEdgeValue( e );
	outLayout->setEdgeValue( e, edgeControls );
      } else if( inT >= 1.0f ) {
	edgeControls = g1->layout->getEdgeValue( e );
	outLayout->setEdgeValue( e, edgeControls );
      } else if( e0 && e1 ) {
	edgeControls0 = e0->getEdgeValue( e );
	edgeControls1 = e1->getEdgeValue( e );
	int n = edgeControls0.size();
	edgeControls.resize( n );
	for( int ec = 0 ; ec < n ; ec++ )
	  edgeControls[ec] = edgeControls0[ec] + (edgeControls1[ec]-edgeControls0[ec]) * inT;
	outLayout->setEdgeValue( e, edgeControls );
      }
    }
    
    // Size
    if( g0->size && g1->size ) {
      Size s0, s1;
      s0 = g0->size->getEdgeValue( e );
      s1 = g1->size->getEdgeValue( e );
      s0 += (s1-s0) * inT;
      outSize->setEdgeValue( e, s0 );
    }
    
    // Color
    if( g0->color && g1->color ) {
      Color c0, c1;
      c0 = g0->color->getEdgeValue( e );
      c1 = g1->color->getEdgeValue( e );
      InterpolateColors( c0, c0, c1, inT );
      outColor->setEdgeValue( e, c0 );
    }
  }
  delete edgeIt;
  
  // Camera
  Coord cam_center = g0->camera.center + (g1->camera.center - g0->camera.center) * inT;
  Coord cam_eyes   = g0->camera.eyes + (g1->camera.eyes - g0->camera.eyes) * inT;
  Coord cam_up     = g0->camera.up + (g1->camera.up - g0->camera.up) * inT;
  float zoomf      = g0->camera.zoomFactor + (g1->camera.zoomFactor - g0->camera.zoomFactor) * inT;
  float radius     = g0->camera.sceneRadius + (g1->camera.sceneRadius - g0->camera.sceneRadius) * inT;
  Camera c;
  c = g0->camera;
  outGlgw->setCamera( Camera(cam_center,cam_eyes,cam_up,zoomf,radius) );
  // Scene
  Coord scn_t      = g0->sceneT + (g1->sceneT - g0->sceneT) * inT;
  Coord scn_r      = g0->sceneR + (g1->sceneR - g0->sceneR) * inT;
  outGlgw->setSceneTranslation( scn_t );
  outGlgw->setSceneRotation( scn_r );
}

  void Morphing::timerEvent( QTimerEvent * te ) {
    if( te->timerId() == this->tid ) {
      float fps = this->fps();
      float min_frames = MORPHING_MIN_DURATION * fps;
      float max_frames = MORPHING_MAX_DURATION * fps;
      float it;
      if( max_frames < MORPHING_MIN_FRAMES )
	it = 1.0f / MORPHING_MIN_FRAMES;
      else if( min_frames < MORPHING_MIN_FRAMES )
	it = 1.0f / max_frames;
      else
	it = 1.0f / min_frames;
      float t  = this->t + it;
      
      //	  cout << "FPS = " << fps << ", it=" << it << endl;
      Observable::holdObservers();
      if( glWidget )
	interpolate(glWidget, t);
      Observable::unholdObservers();
      if(glWidget)
	glWidget->updateGL();
      if( t >= 1.0f ) {
	killTimer( te->timerId() );
	stop();
      }
    }
  }

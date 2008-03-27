#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>

#include <stdio.h>
#include <getopt.h>
  /* from man 3 basename, #include <libgen.h>. However we want to use GNU's basename.
   * see: info libc, libgen.h and string.h for more information. libgen.h is required for dirname,
   * but we are not using it. If dirname comes to be used include libgen.h, #undef basename,
   * include string.h, in this order. It would be ugly but it would work the way we want.
   */

#if defined(__APPLE__)
#include <libgen.h>
#endif

#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/osmesa.h>

#include <tulip/TlpTools.h>
#include <tulip/PluginLoader.h>
#include <tulip/LayoutProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/ExportModule.h>
#include <tulip/ImportModule.h>
#include <tulip/Glyph.h>
#include <tulip/GlScene.h>
#include <tulip/GlDisplayListManager.h>
#include <tulip/GlTextureManager.h>

#include "ImageWriter.h"
#include "Shape.h"

using namespace std;
using namespace tlp;
//EXIT CODES
enum EXIT_CODES {LAYOUT_NOTFOUND = 2,
                 LAYOUT_ERROR,
                 IMAGE_ERROR,
                 HTML_ERROR,
                 OPENGL_ERROR
};
//
static const string DEFAULT_FILENAME("tlprender"); //default filename for output

static string programName;
static string graphFile;
static string layoutName;

static bool layoutSpecified(false);
static bool outputMap(true);
static bool outputHtmlBody(true);
static bool saveTLP(false);
static string saveTLPFile;
static string importPluginName("tlp");
static string filename(DEFAULT_FILENAME);
static string imageFormat(tlprender::SUPPORTED_IMAGE_FORMATS[0]); //default image format
static tlprender::ImageWriter *imageWriter(NULL);
static int width(640), height(480);

static const int BUFFERSIZE(64 * 1024 * 1024);
static GLfloat *buffer;

static void exitManager(void);
static void parseCommandLine(int, char **);
static void help() __attribute__ ((noreturn));

//==============================================================================
class GLOffscreen {
private:
  int width,height;
  OSMesaContext osContext;
  GLubyte *buffer;
public:
  GlScene scene;
  //GlGraph *glgraph;
  GLOffscreen(const int width=640, const int height=480):
    width(width), height(height)
  {
    Vector<int, 4> viewport;
    viewport[0] = 0;
    viewport[1] = 0;
    viewport[2] = width;
    viewport[3] = height;
    scene.setViewport(viewport);

    buffer = new GLubyte [width * height * 4];
    osContext = OSMesaCreateContext(OSMESA_RGBA, NULL);
    if (!osContext) {
      cerr << programName << ": OpenGL: CreateContext Failed!" << endl;
      exit(OPENGL_ERROR);
    }
    
    if (!OSMesaMakeCurrent(osContext, buffer, GL_UNSIGNED_BYTE, width, height)) {
      cerr << programName << ": OpenGL: MakeCurrent Failed!" << endl;
      exit(OPENGL_ERROR);
    }
  }
  
  virtual ~GLOffscreen(){
    OSMesaDestroyContext(osContext);
    delete [] buffer;
  }
  void makeCurrent() {
    //    cerr << __PRETTY_FUNCTION__ << endl;
    OSMesaMakeCurrent(osContext, buffer, GL_UNSIGNED_BYTE, width, height);
  }
  
  void updateGL() {
    //    cerr << __PRETTY_FUNCTION__ << endl;
    glDrawBuffer(GL_FRONT_LEFT);
    glFlush();
  }

  void setupOpenGlContext() {
    //  cerr << __PRETTY_FUNCTION__ << " (" << (int)this << ")" << endl;
    makeCurrent();
  }

  void setDoubleBuffering(bool b) {}

  bool timerIsActive() {return false;}
  int timerStart(int msec, bool sshot=false) {return 0;}
  void timerStop() {}

  void mPaint() {}
  void outputSetText(const string &s) {}
  const string outputGetText() {return string("");}

  const GLubyte *getImageBuffer() const {return buffer;}

};
//==============================================================================
//a pluginLoader with less output than PluginLoaderTxt
struct MyPluginLoader:public PluginLoader {
public:
  virtual void start(const std::string &path,const std::string &type) {
    cout << "Loading " << type << " plugins: ";
  }
  virtual void loading(const std::string &filename) {}
  virtual void loaded(const std::string &name,
		      const std::string &author,
		      const std::string &date, 
		      const std::string &info,
		      const std::string &release,
		      const std::string &version,
		      const std::list <Dependency> &deps)
  {
    cout << "[" << name << "]";
  }
  virtual void aborted(const std::string &filename,const  std::string &erreurmsg) {
    //    cout << "Error loading " << filename << ": " << erreurmsg << endl;
  }
  virtual void finished(bool state,const std::string &msg) {
    cout << endl << endl;
    // else cout << "Loading error " << msg << endl;
  }
};

//load software side plugins
static void loadGlyphPlugins(PluginLoader *plug)
{
  string getEnvVar=tlp::TulipLibDir + "/tlp/";
  
  tlp::loadPluginsFromDir(getEnvVar + "glyphs", "Glyph", plug);
}

//==============================================================================
void importGraph(const string &filename, const string &importPluginName, GlScene *glScene)
{
  DataSet dataSet;

  StructDef parameter=
    ImportModuleFactory::factory->getPluginParameters(importPluginName);
  Iterator<pair<string,string> > *itP=parameter.getField();
  
  for (;itP->hasNext();) {
    pair<string,string> itp=itP->next();
    if (itp.first=="filename"){
      dataSet.set("filename", filename);
      continue;
    }
  }delete itP;
  
  //Graph *newGraph=tlp::importGraph(importPluginName, dataSet, NULL);
  Graph *newGraph=tlp::loadGraph(filename);

  if (newGraph!=0) {
    GlGraphComposite* glGraphComposite = new GlGraphComposite(newGraph);
    GlGraphRenderingParameters param =glGraphComposite->getRenderingParameters();
    GlLayer *layer = new GlLayer("Main");
    glScene->addLayer(layer);
    glScene->addGlGraphCompositeInfo(glScene->getLayer("Main"),glGraphComposite);
    glScene->getLayer("Main")->addGlEntity(glGraphComposite,"graph");

    LayoutProperty *layout =
      newGraph->getProperty<LayoutProperty>("viewLayout");
    layout->resetBoundingBox();
    layout->center();
    layout->notifyObservers();
    glScene->centerScene();
    DataSet glGraphData;
    if (dataSet.get<DataSet>("displaying", glGraphData))
    {
      param.setParameters(glGraphData);
      glGraphComposite->setRenderingParameters(param);
    }
  }
}
//==============================================================================
/*
 *
 */
void outputPolygons(const int size, GlScene &glScene)
{
  priority_queue<tlprender::Shape *> shapeQueue;
  ofstream of((filename+".html").c_str(), ios::out | ios::trunc);
  string filenameBase(basename(filename.c_str()));
  
  bool drawingNodes=false, inNode=false;
  GLint token, count = size, n, x, y;
  GLfloat z;
  GLfloat val=0.0;
  tlprender::Shape *poly=NULL;

  cerr << __PRETTY_FUNCTION__ << ": size=" << size << endl;
  while (count) {
    token = (GLint) buffer[size-count]; count--;
    switch(token) {
    case GL_PASS_THROUGH_TOKEN:
      /*
       * val < 0: then it is a marker: -1 => entering/leaving node rendering
       * val > 0: node number rendered in node mode. This marker occurs each time a new node has been rendered;
       *          NODE 1 ... POLY ... POLY ... POLY ... NODE 2 ... POLY ...
       *	  To clip all polygons of node 1 in one Shape clip them from node 1's marker until you
       *          seek the node 2's marker.
       */
      val = buffer[size-count]; count--;
      if (val == -1.0) drawingNodes = !drawingNodes;
      else if (val >= 0.0) {
 	inNode = !inNode;
 	if (!inNode) {
 	}
 	else {
 	  if (poly!=NULL) {
            if (poly->isValid()) shapeQueue.push(poly);
            else delete poly;
          }
 	  poly = new tlprender::Shape();
	}
      }
#ifndef NDEBUG
      if (val <= 0.0)
        cerr << "GL_PASS_THROUGH_TOKEN: val=" << dec << val << ", drawingNodes=" << drawingNodes << endl;
#endif
      break;
    case GL_POINT_TOKEN: count--; break;
    case GL_LINE_TOKEN: count -= 6; break;
    case GL_LINE_RESET_TOKEN: count -=6; break;
    case GL_POLYGON_TOKEN: {
      n = (GLint) buffer[size-count]; count--;
      if (drawingNodes) {
	tlprender::Shape *tmp = new tlprender::Shape();
        tmp->begin();
	for (int i=0; i<n; ++i) {
	  x = (GLint) buffer[size-count]; count--;
	  y = height - (GLint) buffer[size-count]; count--;
	  z = buffer[size-count]; count--;
	  tmp->add(x,y,z);
	}
	if (tmp->end()) {
	  if (!poly->clip(*tmp)) {
            poly->nodeNum=(unsigned int) val;
            if (poly->isValid()) shapeQueue.push(poly);
            else delete poly;
            poly=tmp;
          }
          else delete tmp;
        }
      } //endif (drawingNodes)
      else {
        count -= (n * 3);        
      }
      break;
    }
    case GL_BITMAP_TOKEN: count -=3; break;
    case GL_DRAW_PIXEL_TOKEN: count -=3; break;
    case GL_COPY_PIXEL_TOKEN: count -=3; break;
    default:
#ifndef NDEBUG
      cerr << __PRETTY_FUNCTION__ << ": count=" << count << ", unexpected token: " << hex << token << dec << endl;
#endif
      break;
    } //end switch(token)
  } //end while(count)

  /* write HTML AREA MAP */
  StringProperty *hrefp = glScene.getGlGraphComposite()->getInputData()->getGraph()->getProperty<StringProperty>("href");
  StringProperty *altp = glScene.getGlGraphComposite()->getInputData()->getGraph()->getProperty<StringProperty>("alt");

  if (outputHtmlBody) {
    of << "<html><body>" << endl;
  }
  
  of << "<img src=\"" << filenameBase << "." << imageFormat << "\" usemap=\"#usemap\" border=\"0\" ";
  of << "width=\"" << width << "\" height=\"" << height << "\">" << endl;
  of << "<map name=\"usemap\">" << endl;

  while (!shapeQueue.empty()) {
    const tlprender::Shape *s = shapeQueue.top();
    string href = hrefp->getNodeValue(node(s->nodeNum));
#ifndef NDEBUG
    if (href=="") {
      stringstream ss;
      ss << "javascript:alert(\'node " << s->nodeNum << "\')";
      href = ss.str();
    }
#endif
    string alt = altp->getNodeValue(node(s->nodeNum));
#ifdef NDEBUG
    if (alt=="") {
      stringstream ss;
      ss << "node " << s->nodeNum;
      alt = ss.str();
    }
#endif
    of << "    <area href=\"" << href << "\" alt=\"" << alt << "\"" << endl;
    of << "          shape=\"poly\" coords=\"" << *s << "\">" << endl;
    delete s;
    shapeQueue.pop();
  }
  
  of << "</map>" << endl;
  
  if (outputHtmlBody) {
    of << "</body></html>" << endl;
  }
  
  of.close();
}

/***************************************************************************************************/
int main (int argc, char **argv) {

  atexit(exitManager);

  parseCommandLine(argc, argv);
  
  MyPluginLoader plug;
  tlp::initTulipLib();
  tlp::loadPlugins(&plug);   // library side plugins
  loadGlyphPlugins(&plug);   // software side plugins, i.e. glyphs

  GLOffscreen glOffscreen(width, height);

  importGraph(graphFile, importPluginName, &glOffscreen.scene);
  
  Graph *graph=glOffscreen.scene.getGlGraphComposite()->getInputData()->getGraph();

  if (layoutSpecified) {
    bool resultBool=false;
    string errorMsg;
    if (LayoutProperty::factory->pluginExists(layoutName)) {
      LayoutProperty *myLayout = graph->getProperty<LayoutProperty>("viewLayout");
      resultBool = graph->computeProperty(layoutName, myLayout, errorMsg);
      if (!resultBool) {
        cerr << programName << ": layout error, reason: " << errorMsg << endl;
        exit(LAYOUT_ERROR);
      }
      else {
        myLayout->resetBoundingBox();
        myLayout->center();
        myLayout->notifyObservers();
        glOffscreen.scene.centerScene();
      }
    }
    else {
      cerr << programName << ": cannot find layout \"" << layoutName << "\"" << endl;
      exit(LAYOUT_NOTFOUND);
    }
  }
  
  glOffscreen.setupOpenGlContext();
  glOffscreen.updateGL();

  GlDisplayListManager::getInst().changeContext(0);
  GlTextureManager::getInst().changeContext(0);

  GlGraphRenderingParameters params = glOffscreen.scene.getGlGraphComposite()->getRenderingParameters();
  params.setIncrementalRendering(false);
  glOffscreen.scene.getGlGraphComposite()->setRenderingParameters(params);
  glOffscreen.scene.draw();

  glOffscreen.setupOpenGlContext();
  glOffscreen.updateGL();

  //write image
  imageWriter = tlprender::ImageWriter::getImageWriter(imageFormat, glOffscreen.getImageBuffer(), width, height);
  if (imageWriter != NULL) {
    imageWriter->setSoftware(programName);
    imageWriter->setSource(graphFile);
    if (layoutSpecified) imageWriter->setLayout(layoutName);
    ofstream of((filename + "." + imageFormat).c_str(), ios::out|ios::trunc|ios::binary);
    imageWriter->writeImage(of);
    of.flush();
    of.close();
    delete imageWriter;
  }
  else {
    cerr << programName << ": No image written to disk: couldn't get a writer for format " << imageFormat << endl;
    exit(IMAGE_ERROR);
  }
  
  //create HTML map
  /*if (outputMap) {
    buffer = new GLfloat[BUFFERSIZE];
    glFeedbackBuffer(BUFFERSIZE, GL_3D, buffer);
    glRenderMode(GL_FEEDBACK);
    glOffscreen.draw();

    int size = glRenderMode(GL_RENDER);
    if (size < 0) {
      cerr << programName << ": Problem during Feedback mode. Cannot compute HTML page" << endl;
      exit(HTML_ERROR);
    }
    else  outputPolygons(size, glOffscreen);
    
    delete[] buffer;
  }

  if (saveTLP) {
    DataSet dataSet;
    ostream *os = new ofstream(saveTLPFile.c_str());
    StructDef parameter =
      ExportModuleFactory::factory->getPluginParameters("tlp");

    dataSet.set("displaying", glOffscreen.getRenderingParameters().getParameters());

    if (!tlp::exportGraph(glOffscreen.getGraph(), *os, "tlp", dataSet, NULL)) {
      cerr << programName << ": saving graph to \"" << saveTLPFile << "\" failed. Exiting" << endl;
      return EXIT_FAILURE;
    }
    }*/
  
  return EXIT_SUCCESS;
}

/*
 *
 */
void exitManager()
{
}

/*
 * command line parser and help() functions below this line, nothing else should be added after
 */
void parseCommandLine(int argc, char **argv)
{
  static struct option long_options[] = {
    {"format", 1, 0, 'f'},
    {"help", 0, 0, 'h'},	//shows help
    {"height", 1, 0, 0},	//height, in pixel
    {"layout", 1, 0, 'l'},	//a layout to apply
    {"no-htmlbody", 0 , 0, 0},          //do not ouput <html><body>, only <map>...</map>
    {"no-map", 0, 0, 0},        //produce only an image, no html map
    {"output", 1, 0, 'o'},	//output File
    {"save", 1, 0, 's'},        //save graph in tlp FILE; required argument
    {"type", 1, 0, 't'},	//format of imported file (tlp, GML, etc...)
    {"width", 1, 0, 0},		//width, in pixels
    {0,0,0,0}
  };

  //variables init
  programName = string(basename(argv[0]));

  if (argc < 2) help();
  
  char *endptr;
  char c;
  int option_index=0;
  
  while ((c=getopt_long(argc, argv, "f:hl:o:s:t:", long_options, &option_index)) != -1) {
    switch(c) {
    case 0: {
      string option = long_options[option_index].name;
      if (option == "width") {
	width = strtol(optarg, &endptr, 10);
	if ((*endptr != '\0') || (width < 1)) {
	  cerr << programName << ": invalid width!" << endl;
	  help();
	}
      }
      else if (option == "height") {
	height = strtol(optarg, &endptr, 10);
	if ((*endptr != '\0') || (height < 1)) {
	  cerr << programName << ": invalid height!" << endl;
	  help();
	}
      }
      else if (option == "no-htmlbody") outputHtmlBody = false;
      else if (option == "no-map") outputMap = false;
      break;
    }
    case 'f':
      imageFormat = optarg;
#ifdef HAVE_LIBPNG
      if (imageFormat == "png");
      else
#endif
#ifdef HAVE_LIBJPEG
      if (imageFormat == "jpeg");
      else
#endif
	{
	  cerr << "Error: unsupported image format: " << imageFormat << endl;
	  help();
	}
      break;
      
    case 'h': 
    case ':': help(); break;
    case 'l': layoutSpecified = true; layoutName = optarg; break;
    case 'o': filename = optarg; break;
    case 's': saveTLP = true; saveTLPFile = optarg; break;
    case 't': importPluginName = optarg; break;
    case '?':
    default: cerr << programName << ": unknown option: " << c << endl; break;
    }
  }

  if (optind < argc) {
    graphFile = argv[optind];
  }
  else {
    cerr << programName << ": no file specified!" << endl;
    help();
  }
  
#ifndef NDEBUG
  cerr << __PRETTY_FUNCTION__ << ": graphFile = " << graphFile << endl;
#endif
}

void help()
{
  cout << "Usage: " << programName << "[OPTIONS] ... FILE " << endl;
  cout << "Generate an image and a HTML file with a corresponding image MAP" << endl;
  cout << "from FILE, where FILE is a graph description" << endl;
  cout << endl;
  cout << "Options:" << endl;
  cout << "  -h, --help:        show this help" << endl;
  cout << "  -t, --type=TYPE:   use TYPE as import plugin [default=tlp]" << endl;
  cout << endl;
  cout << "Image options:" << endl;
  cout << "  -f, --format=F     set image format to F" << endl;
  cout << "                     supported: [" << tlprender::SUPPORTED_IMAGE_FORMATS[0] << "] ";
  for (int i=1; tlprender::SUPPORTED_IMAGE_FORMATS[i] != ""; ++i)
    cout << tlprender::SUPPORTED_IMAGE_FORMATS[i] << " ";
  cout << endl;
  cout << "      --height=H:    set image height to H" << endl;
  cout << "      --width=W:     set image width to W" << endl;
  cout << endl;
  cout << "Output options:" << endl;
  cout << "  -l, --layout=ALGO  apply layout named ALGO before rendering" << endl;
  cout << "      --no-htmlbody  output only \"<img ...></img><map>...</map>\" HMTL code" << endl;
  cout << "                     (to embed in a complete HTML page)" << endl;
  cout << "      --no-map       do not output an HTML map" << endl;
  cout << "  -o, --output=FILE: generate FILE.png and FILE.html [default=" << DEFAULT_FILENAME << "]" << endl;
  cout << "  -s, --save=[FILE]: saves graph to FILE, in Tulip format" << endl;
  exit(EXIT_FAILURE);
}

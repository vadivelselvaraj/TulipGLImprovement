# MACRO(BUNDLE_DEFINE_COMPONENT component display_name description group)

BUNDLE_DEFINE_COMPONENT(gzstream "gzstream" "Provides the functionality of the zlib C-library in a C++ iostream" "thirdparty")
BUNDLE_DEFINE_COMPONENT(ftgl "ftgl" "A font rendering library for OpenGL" "thirdparty")
BUNDLE_DEFINE_COMPONENT(ogdf "OGDF" "OGDF is a self-contained C++ class library for the automatic layout of diagrams" "thirdparty")

BUNDLE_DEFINE_COMPONENT(quazip "quazip" "Qt/C++ wrapper for ZIP/UNZIP package" "thirdparty")
BUNDLE_DEFINE_COMPONENT(yajl "yajl" "YAJL is a small event-driven (SAX-style) JSON parser written in ANSI C, and a small validating JSON generator" "thirdparty")
BUNDLE_DEFINE_COMPONENT(model_tests "Qt Model tester" "Validator library for Qt model framework" "thirdparty")
BUNDLE_DEFINE_COMPONENT(qxt "qxt" "LibQxt is an extension library for Qt providing a suite of cross-platform utility classes to add functionality not readily available in the Qt toolkit by Trolltech, a Nokia company." "thirdparty")
BUNDLE_DEFINE_COMPONENT(dbus "DBus" "D-Bus is a message bus system, a simple way for applications to talk to one another." "thirdparty")
BUNDLE_DEFINE_COMPONENT(runtime "Runtime" "Runtime components for the MinGW framework" "thirdparty")
BUNDLE_DEFINE_COMPONENT(iconv "iconv" "The GNU iconv library" "thirdparty")
BUNDLE_DEFINE_COMPONENT(zlib "zlib" "A Massively Spiffy Yet Delicately Unobtrusive Compression Library" "thirdparty")
BUNDLE_DEFINE_COMPONENT(freetype "freetype" "A Free, High-Quality, and Portable Font Engine" "thirdparty")
BUNDLE_DEFINE_COMPONENT(qt "Qt" "Intuitive APIs for C++ and CSS/JavaScript-like programming with Qt Quick for rapid UI creation" "thirdparty")
BUNDLE_DEFINE_COMPONENT(glew "glew" "The OpenGL Extension Wrangler Library" "thirdparty")
BUNDLE_DEFINE_COMPONENT(jpeg "jpeg" "libjpeg is a library written entirely in C which contains a widely-used implementation of a JPEG decoder, JPEG encoder and other JPEG utilities." "thirdparty")
BUNDLE_DEFINE_COMPONENT(png "png" "libpng is the official PNG reference library." "thirdparty")
BUNDLE_DEFINE_COMPONENT(python "Python" "Python Programming Language" "thirdparty")
BUNDLE_DEFINE_COMPONENT(sip "SIP" "Python/C++ bindings library" "thirdparty")
BUNDLE_DEFINE_COMPONENT(intl "INTL" "Libintl is a library that provides native language support to programs. It is part of Gettext" "thirdparty")

BUNDLE_DEFINE_COMPONENT(tulip_core_dev "tulip-core (dev)" "Headers and developer files for the tulip-core library" "Developement files")
BUNDLE_DEFINE_COMPONENT(tulip_core "tulip-core" "tulip-core library: A C++ framework for graph manipulation" "Libraries")
BUNDLE_DEFINE_COMPONENT(tulip_ogl_dev "tulip-ogl (dev)" "Headers and developer files for the tulip-ogl library" "Developement files")
BUNDLE_DEFINE_COMPONENT(tulip_ogl "tulip-ogl" "tulip-ogl library: An openGL based framework to provide 3D visualization on tulip-core objects." "Libraries")
BUNDLE_DEFINE_COMPONENT(tulip_gui_dev "tulip-gui (dev)" "Headers and developer files for the tulip-gui library" "Developement files")
BUNDLE_DEFINE_COMPONENT(tulip_gui "tulip-gui" "tulip-gui library: A Qt based library to provide GUI oriented features using the tulip-ogl and tulip-core libraries" "Libraries")
BUNDLE_DEFINE_COMPONENT(tulip_python "Tulip python" "Tulip python bindings" "Libraries")

BUNDLE_DEFINE_COMPONENT(tulip_ogdf_dev "tulip-ogdf (dev)" "Headers and developer files for the tulip-ogdf library" "Developement files")
BUNDLE_DEFINE_COMPONENT(tulip_ogdf "tulip-ogdf" "Bridge library between tulip-core and ogdf." "Libraries")
BUNDLE_DEFINE_COMPONENT(tulip_app "tulip-app" "Main Tulip application" "Software")

BUNDLE_DEFINE_COMPONENT(TULIP_TEXTURES "Textures pack" " " "Textures")

BUNDLE_DEFINE_COMPONENT(strengthclustering "Strength clustering" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(HIERARCHICALCLUSTERING "Hierarchical clustering" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CONVOLUTIONCLUSTERING "Convolution clustering" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(EQUALVALUECLUSTERING "Equal value clustering" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(QUOTIENTCLUSTERING "Quotient clustering" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(COLORMAPPING "Color mapping" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(PLANARGRAPH "Planar graph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(DOTIMPORT "Dot import" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(GRID "Grid" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(GMLIMPORT "GML" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(RANDOMGRAPH "Random graph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(RANDOMSIMPLEGRAPH "Random simple graph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(ADJACENCYMATRIXIMPORT "Adjacency matrix" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(COMPLETEGRAPH "Complete graph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(RANDOMTREE "Random tree" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(RANDOMTREEGENERAL "Random tree general" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(COMPLETETREE "Complete tree" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(SMALLWORLDGRAPH "Small world graph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(FILESYSTEM "File system" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(WEBIMPORT "Web import" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(IMPORTPAJEK "Import Pajek" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(IMPORTUCINET "Import UCINET" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(MIXEDMODEL "Mixed model" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CONNECTEDCOMPONENTPACKING "Connected component packing" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(RANDOM "Random" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(GEMLAYOUT "GEM layout" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TREEREINGOLDANDTILFOREXTENDED "TREEREINGOLDANDTILFOREXTENDED" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CONETREEEXTENDED "Cone tree extended" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TREERADIAL "Tree radial" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TREELEAF "Tree leaf" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(BUBBLETREE "Bubble tree" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CIRCULAR "Circular" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(HIERARCHICALGRAPH "Hierarchical graph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TUTTE "Tutte" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(DENDROGRAM "Dendogram" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(IMPROVEDWALKER "Improved walker" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(SQUARIFIEDTREEMAP "Squarified treemap" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TULIP_OGDF_PLUGINS "Tulip OGDF plugins" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(ECCENTRICITY "Eccentricity" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(DEGREEMETRIC "Degree" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CLUSTERMETRIC "Cluster metric" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(STRENGTHMETRIC "Strength" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(BICONNECTEDCOMPONENT "Biconnected component" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CONNECTEDCOMPONENT "Connected component" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(STRONGCOMPONENT "Strong component" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(DAGLEVELMETRIC "DAG level" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(IDMETRIC "ID" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(LEAFMETRIC "Leaf" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(NODEMETRIC "Node" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(DEPTHMETRIC "Depth" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(PATHLENGTHMETRIC "Path length" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(STRAHLERMETRIC "Strahler" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(RANDOMMETRIC "Random" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(BETWEENNESSCENTRALITY "Betweenes centrality" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(SPANNINGDAGSELECTION "Spanning DAG" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(REACHABLESUBGRAPHSELECTION "Reachable subgraph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(SPANNINGTREESELECTION "Spanning tree" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INDUCEDSUBGRAPHSELECTION "Induced subgraph" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(LOOPSELECTION "Loop" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(MULTIPLESELECTION "Multiple selection" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(KRUSKAL "Kruskal" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(METRICSIZEMAPPING "Metric size mapping" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(AUTOSIZE "Auto size" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(FITTOLABELS "Fit to labels" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(GMLEXPORT "GML" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(CIRCLE "Circle" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(HEXAGONE "Hexagone" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TRIANGLE "Triangle" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(PENTAGONE "Pentagone" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(SQUARE "Square" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(RING "Ring" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(BILLBOARD "Billboard" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CUBE "Cube" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CUBEOUTLINEDTRANSPARENT "Cube outlined transparent" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(SPHERE "Sphere" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CONE "Cone" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CYLINDER "Cylinder" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(HALFCYLINDER "Half cylinder" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CHRISTMASTREE "Christmas tree" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(WINDOW "Window" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(ROUNDEDBOX "Rounded box" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(DIAMOND "Diamond" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(CROSS "Cross" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(SPREADSHEET "Spreadsheet" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TULIP_GUI_PLUGINS "tulip-gui plugins" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(TULIP_PLUGINS_PYTHONSCRIPTVIEW "Python script view" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(INTERACTORNAVIGATION "Navigation" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTORGETINFORMATION "Get information" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTORSELECTION "Plugins" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTORSELECTIONMODIFIER "Selection modifier" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTORRECTANGLEZOOM "Rectangle zoom" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTORDELETEELEMENT "Delete element" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTORADDNODE "Add node" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTORADDEDGE "Add edge" " " "Plugins")
BUNDLE_DEFINE_COMPONENT(INTERACTOREDITEDGEBENDS "Edit edge bends" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(TOLABELS "To labels" " " "Plugins")

BUNDLE_DEFINE_COMPONENT(GRAPHPERSPECTIVE "Graph hierarchy analysis" " " "Plugins")


#include <qfiledialog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tulip/TulipPlugin.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

using namespace std;
using namespace tlp;

/** \addtogroup import */
/*@{*/
/// Import a tree representation of a file system directory.
/** This plugin enables to capture in a tree the full hierarchy of
 *  of a given file system directory
 */
class FileSystem:public ImportModule {
public:
  FileSystem(AlgorithmContext context):ImportModule(context) {}
  ~FileSystem(){}

  DoubleProperty *size,*gid,*uid,*lastaccess,*lastmodif,*lastchange;
  IntegerProperty *type;
  StringProperty *label;
  LayoutProperty *layout;
  int progress;


  ProgressState readDir(node n, string directory, unsigned int &x, unsigned int y) {
    if (pluginProgress->progress(progress,100)!=TLP_CONTINUE) { 
      return pluginProgress->state();
    }
    progress++;
    progress = progress%100;
    #ifndef _WIN32
    DIR *dir=opendir(directory.c_str());
    if (dir == 0) {
      pluginProgress->stop();
      return pluginProgress->state();
    }
    dirent *entry;
    #else
    HANDLE hFind; 
  	WIN32_FIND_DATA FindData; 
  	SetCurrentDirectory (directory.c_str()); 
  	hFind=FindFirstFile ("*.*", &FindData); 
  	bool endOfDirectory = (hFind == NULL) || (hFind == INVALID_HANDLE_VALUE);
  	if(endOfDirectory) {
    pluginProgress->stop();
    return pluginProgress->state();
  	}
    #endif
    struct stat infoEntry;
    #ifdef _WIN32
     while (!endOfDirectory) {
   	  if (!strcmp("..",FindData.cFileName) || !strcmp(".",FindData.cFileName)) {
   	  	endOfDirectory = !(FindNextFile (hFind, &FindData));
   	  	continue;
   	  }
      string entryName=FindData.cFileName;
      string pathEntry=directory+entryName;
	  stat(pathEntry.c_str(),&infoEntry);
     #else
    while ((entry = readdir(dir))!=0) {
      if (!strcmp("..",entry->d_name) || !strcmp(".",entry->d_name)) continue;
      string entryName=entry->d_name;
      string pathEntry=directory+entryName;
      lstat(pathEntry.c_str(),&infoEntry);
    #endif
      if (infoEntry.st_dev==true) continue;
      node newNode=graph->addNode();
      graph->addEdge(n,newNode);
      label->setNodeValue(newNode,entryName);
      if (infoEntry.st_size<1)
	size->setNodeValue(newNode,1);
      else
	size->setNodeValue(newNode,infoEntry.st_size);
      uid->setNodeValue(newNode,infoEntry.st_uid);
      gid->setNodeValue(newNode,infoEntry.st_gid);
      lastaccess->setNodeValue(newNode,infoEntry.st_atime);
      lastmodif->setNodeValue(newNode,infoEntry.st_mtime);
      lastchange->setNodeValue(newNode,infoEntry.st_ctime);
      
      if ((infoEntry.st_mode & S_IFMT) == S_IFDIR) {
	x += 2;
	if (readDir(newNode,pathEntry+"/", x , x ) == TLP_CANCEL)
	  graph->delNode(newNode);
	else {
	  double newSize=0;
	  Coord tmp(0,0,0);
	  Iterator<node> *itN=graph->getOutNodes(newNode);
	  for (;itN->hasNext();) {
	    node itn=itN->next();
	    newSize+=size->getNodeValue(itn);
	    tmp += layout->getNodeValue(itn);
	  }
	  delete itN;
	  size->setNodeValue(newNode,newSize/1024.0);
	  if (graph->outdeg(newNode) == 0) {
	    layout->setNodeValue(newNode, Coord(x, y, 0));
	    x += 2;
	  } else {
	    tmp[0] /= graph->outdeg(newNode);
	    tmp[1] = y;
	    layout->setNodeValue(newNode, tmp);
	  }
	}
      } else {
	layout->setNodeValue(newNode, Coord(x, y, 0));
	x += 2;
      }
       #ifdef _WIN32
      endOfDirectory = !(FindNextFile (hFind, &FindData));
      #endif 
    } 
    #ifdef _WIN32
    FindClose (hFind); 
    #else
    closedir(dir);
    #endif
    return TLP_CONTINUE;
  }

  bool import(const string &name) {
    size=graph->getProperty<DoubleProperty>("size");
    uid=graph->getProperty<DoubleProperty>("uid");
    gid=graph->getProperty<DoubleProperty>("gid");
    lastaccess=graph->getProperty<DoubleProperty>("lastaccess");
    lastmodif=graph->getProperty<DoubleProperty>("lastmodif");
    lastchange=graph->getProperty<DoubleProperty>("lastchange");
    type=graph->getProperty<IntegerProperty>("viewShape");
    layout=graph->getProperty<LayoutProperty>("viewLayout");
    label=graph->getProperty<StringProperty>("name");
    type->setAllNodeValue(0);
    layout->setAllNodeValue(Coord(0,0,0));
    node newNode=graph->addNode();
    QString dirName=QFileDialog::getExistingDirectory ();
    if (dirName.isNull()) return false;

    struct stat infoEntry;
    #ifdef _WIN32
    stat(dirName.toAscii().data(),&infoEntry);
    #else
    lstat(dirName.toAscii().data(),&infoEntry);
    #endif
    if (infoEntry.st_dev!=true)  {
      label->setNodeValue(newNode,dirName.toAscii().data());
      if (infoEntry.st_size<1)
	size->setNodeValue(newNode,1);
      else
	size->setNodeValue(newNode,infoEntry.st_size);
      uid->setNodeValue(newNode,infoEntry.st_uid);
      gid->setNodeValue(newNode,infoEntry.st_gid);
      lastaccess->setNodeValue(newNode,infoEntry.st_atime);
      lastmodif->setNodeValue(newNode,infoEntry.st_mtime);
      lastchange->setNodeValue(newNode,infoEntry.st_ctime);
    }

    if (pluginProgress)
      pluginProgress->showPreview(false);

    unsigned int x = 0, y = 2;
    readDir(newNode,string(dirName.toAscii().data())+"/", x , y);
    double newSize=0;
    Coord tmp(0,0,0);
    if (pluginProgress->state()!=TLP_CANCEL) {
      Iterator<node> *itN=graph->getOutNodes(newNode);
      while (itN->hasNext()) {
	node itn=itN->next();
	newSize+=size->getNodeValue(itn);
	tmp += layout->getNodeValue(itn);
      } delete itN;
      size->setNodeValue(newNode,newSize);
      tmp /= graph->outdeg(newNode);
      tmp[1] = 0;
      layout->setNodeValue(newNode, tmp);
      node itn;
      forEach(itn, graph->getNodes()) {
	tmp = layout->getNodeValue(itn);
	tmp[1] = -tmp[1];
	layout->setNodeValue(itn, tmp);
      }
    }
    return pluginProgress->state()!=TLP_CANCEL;
  }
};
/*@}*/
IMPORTPLUGINOFGROUP(FileSystem,"File System Directory", "Auber", "16/12/2002", "", "1.0", "Misc")

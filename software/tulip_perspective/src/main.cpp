#include "TulipAgentCommunicator.h"

#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

#include <tulip/PluginLibraryLoader.h>
#include <tulip/PluginLister.h>
#include <tulip/TlpTools.h>
#include <tulip/TulipProject.h>
#include <tulip/SimplePluginProgressWidget.h>
#include <tulip/PluginLister.h>
#include <tulip/Perspective.h>
#include <tulip/InteractorManager.h>

#include <iostream>
using namespace std;
using namespace tlp;

void usage(const QString &error) {
  int returnCode = 0;
  if (!error.isEmpty()) {
    cerr << "Error: " << error.toStdString() << endl;
    returnCode = 1;
  }
  cout << "Usage: tulip_perspective [OPTION] [FILE]" << endl
       << "Run a Tulip Perspective plugin into its dedicated process." << endl
       << "If Tulip main process is already running, embedded perspective will run into managed mode." << endl << endl
       << "FILE: a Tulip project file. If a file is specified, the --perspective flag will be ignored and tulip_perspective will look into the project's meta-informations to find the correct perspective to launch." << endl
       << "List of OPTIONS:" << endl
       << "  --perspective=<perspective_name>\tWill use the perspective specified by perspective_name. Perspective will be run with no project file. If a project file has been specified using the FILE option, this flag will be ignored." << endl
       << "  --help\tDisplays this help message and ignores other options." << endl;
  exit(returnCode);
}

int main(int argc,char **argv) {
  QApplication tulip_perspective(argc, argv);

  SimplePluginProgressWidget *progress = new SimplePluginProgressWidget;
  progress->setWindowTitle(QObject::trUtf8("Opening perspective"));
  progress->setComment(QObject::trUtf8("Starting process"));
  progress->progress(0,100);
  progress->show();

  tulip_perspective.setApplicationName(QObject::trUtf8("Tulip"));
  QLocale::setDefault(QLocale(QLocale::English));

  progress->setComment(QObject::trUtf8("Initializing D-Bus"));

#if defined(__APPLE__)
  // allows to load qt imageformats plugin
  QApplication::addLibraryPath(QApplication::applicationDirPath() + "/..");
  // Switch the current directory to ensure that libdbus is loaded
  QString currentPath = QDir::currentPath();
  QDir::setCurrent(QApplication::applicationDirPath() + "/../Frameworks");
#endif

  TulipAgentCommunicator *communicator = new TulipAgentCommunicator("org.labri.Tulip","/",QDBusConnection::sessionBus(),0);

  if (!(QDBusConnection::sessionBus().interface()->isServiceRegistered("org.labri.Tulip").value()))
    qWarning("No org.labri.Tulip service on session bus. Falling back to headless mode.");

#if defined(__APPLE__) // revert current directory
  QDir::setCurrent(currentPath);
#endif

  progress->progress(25,100);

  progress->setComment(QObject::trUtf8("Loading plugins"));

  // Init Tulip and load plugins
  tlp::initTulipLib(QApplication::applicationDirPath().toStdString().c_str());
  tlp::PluginLibraryLoader::loadPlugins();
  tlp::PluginListerInterface::checkLoadedPluginsDependencies(0);
  tlp::InteractorManager::getInst().loadInteractorPlugins();

  progress->progress(60,100);
  progress->setComment(QObject::trUtf8("Cheking arguments"));

  // Check arguments
  QString perspectiveName,projectFilePath;
  QRegExp perspectiveRegexp("^\\-\\-perspective=(.*)");
  QString a;
  int i=0;
  foreach(a,QApplication::arguments()) {
    if (i++ == 0)
      continue;
    if (perspectiveRegexp.exactMatch(a))
      perspectiveName = perspectiveRegexp.cap(1);
    else if (a == "--help")
      usage("");
    else if (projectFilePath.isNull())
      projectFilePath = a;
  }

  if (perspectiveName.isNull() && projectFilePath.isNull()) // no argument given (program was previously stopped if --help was specified)
    usage("Invalid arguments");

  // Initialize context
  PerspectiveContext context;
  TulipProject *project = NULL;

  if (!projectFilePath.isNull()) { // some file was specified. We check if it can be opened as a TulipProject. If not, we consider it as an external file.
    project = TulipProject::openProject(projectFilePath,progress);

    if (!project->isValid()) {
      if (perspectiveName.isEmpty()) // If the specified file is not a tulip project, we need a perspective name to open it
        usage("Invalid project file and could not retrieve a perspective name: " + project->lastError());
      else {
        context.externalFile = projectFilePath;
        delete project;
        project = NULL;
      }
    }

    else // We have a valid tulip project
      perspectiveName = project->perspective();
  }

  if (!project) { // no file path was specified or it was not a tulip project, we create an empty project
    project = TulipProject::newProject();
    if (!project->isValid())
      usage("Failed to initialize project files:" + project->lastError());
    project->setPerspective(perspectiveName);
  }
  context.project = project;

  progress->progress(80,100);
  progress->setComment("Setting up GUI");

  // Initialize main window.
  QMainWindow *mainWindow = new QMainWindow();
  mainWindow->setVisible(false);
  QString title("Tulip [" + perspectiveName + "]");
  if (project) {
    title += " - ";
    if (!project->name().isNull())
      title += project->name();
    else
      title += projectFilePath;
  }
  mainWindow->setWindowTitle(title);
  context.mainWindow = mainWindow;

  // Create perspective object
  Perspective *perspective = StaticPluginLister<Perspective,PerspectiveContext>::getPluginObject(perspectiveName.toStdString(), context);
  if (!perspective)
    usage("Failed to create perspective: " + perspectiveName);

  // Connect perspective and communicator
  QObject::connect(perspective,SIGNAL(showTulipWelcomeScreen()),communicator,SLOT(ShowWelcomeScreen()));
  QObject::connect(perspective,SIGNAL(showTulipPluginsCenter()),communicator,SLOT(ShowPluginsCenter()));
  QObject::connect(perspective,SIGNAL(showTulipAboutPage()),communicator,SLOT(ShowAboutPage()));
  QObject::connect(perspective,SIGNAL(showOpenProjectWindow()),communicator,SLOT(ShowOpenProjectWindow()));
  QObject::connect(perspective,SIGNAL(openProject(QString)),communicator,SLOT(OpenProject(QString)));
  QObject::connect(perspective,SIGNAL(openProjectWith(QString,QString)),communicator,SLOT(OpenProjectWith(QString,QString)));
  QObject::connect(perspective,SIGNAL(createPerspective(QString)),communicator,SLOT(CreatePerspective(QString)));
  QObject::connect(perspective,SIGNAL(addPluginRepository(QString)),communicator,SLOT(AddPluginRepository(QString)));
  QObject::connect(perspective,SIGNAL(removePluginRepository(QString)),communicator,SLOT(RemovePluginRepository(QString)));
  QObject::connect(communicator,SIGNAL(Terminate()),perspective,SLOT(terminated()));

  perspective->construct();

  delete progress;

  return tulip_perspective.exec();
}

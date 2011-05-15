#ifndef TULIPPROJECT_H
#define TULIPPROJECT_H

#include <fstream>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDate>
#include <QtCore/QObject>
#include <QtCore/QIODevice>

#include <tulip/tulipconf.h>
#include <tulip/PluginProgress.h>

#define PROJECT_OK 0

namespace tlp {

/**
  @brief The TulipProject object handles the content of a Tulip project archive.

  All tulip projects contain a set of defined static meta-informations:
  @list
  @li name (QString): the name of the project
  @li description (QString): Comments about the project
  @li author (QString): the author of the project
  @li perspective (QString): the name of the perspective plugin associated to the project
  @li date (QDate): the date of project's last modification
  @li version (QString): the version of the Tulip project format
  @endlist

  Alongside those informations, one can store any kind of file into a Tulip project. Since a project is meant to be associated to a specific perspective, the responisbility of those file
  is left to the perspective.

  A TulipProject DOES NOT automatically save to disk. One will have to call the save() method to serialize data.
  Precise implementation or actual location of uncompressed the files when a TulipProject is opened should not be known or taken into account by the user.

  When opening/saving a TulipProject, errors might be raised due to invalid filesystem operations. Error codes are integer values defined in constants:
  @list
  @li PROJECT_OK (0): no error
  @endlist

  After opening and before saving a project, user will be able to list/delete files and directories available in the project and open them using std filestreams or Qt's QIODevice.
  Files can be opened using the stdFileStram and fileStram methods. They will always be opened in Read/Write mode.

  Files in a tulip project are identified by their path. Those path ar similar to the path of a standard file system and use the "/" character as a separator.
  The root directory is identified by "/".

  @example A file called graph.tlp located at the top-level directory will be identified by the "/graph.tlp" path while the file "graph.tlp" located in the "data" directory will be identified by "/data/graph.tlp".
  */
class TLP_QT_SCOPE TulipProject: public QObject {
  Q_OBJECT

  TulipProject();
public:

  /**
    @brief Opens a compressed tulip project file
    This method will unpack a tulip project file into some directory and allow the user to manipulate the files.
    @see TulipProject::save()
    @arg file The file to open.
    @arg errorCode If the opening process fails, this method will return a NULL pointer and input an error code into the errorCode parameter.
    @see TulipProject for a complete list of error codes.
    @arg progress A progress handler.
    @return a pointer to a TulipProject object.
    */
  static TulipProject *open(const QString &file,int *errorCode, tlp::PluginProgress *progress=0);

  /**
    @brief Saves files in the TulipProject into a packed archive.
    This method packs every file in the project into a single archive.
    @note This method DOES NOT close the project. It only  commits changes to the specified file. A TulipProject is only closed when destroyed.
    @param file Absolute path where files should be packed.
    @param progress A progress handler
    @return Some error code or PROJECT_OK if method returned sucessfully.
    */
  int save(const QString &file, tlp::PluginProgress *progress=0);

  /**
    @brief Lists entries in a directory
    @param path The path to scan. @see TulipProject
    @return The list of files and directories present in the given directory
    */
  QStringList entryList(const QString &path);

  /**
    @brief Checks if the given path is a directory.
    @param path The path to check. @see TulipProject
    @return true/false wether the path is a directory.
    */
  bool isDir(const QString &path);

  /**
    @brief Removes a file from the project.
    If the given path points to a directory, or if the file does not exist, this method will fail and return false
    @param path The path to delete. @see TulipProject
    */
  bool removeFile(const QString &path);

  /**
    @brief Removes a directory from the project.
    If the given file points to a file, or if the directory does not exist, this method will fail and return false.
    @param path The path to delete. @see TulipProject
    */
  bool removeDir(const QString &path);

  /**
    @brief Gets a STL file stream (R/W access mode) to the given path.
    @warning This method does not check if the given path is a directory or a file. User might get an invalid filestream.
    @param path The path to open. @see TulipProject
    @return an opened filestream on the given path.
    */
  std::fstream *stdFileStream(const QString &path);

  /**
    @brief Gets a Qt I/O device (R/W access mode) to the given path.
    @warning This method does not check if the given path is a directory or a file. User might get an invalid filestream.
    @warning User SHOULD NOT cast the QIODevice returned by this method into any of its subclass since the implementation might change in future versions.
    @param path The path to open. @see TulipProject
    @return an opened Qt device on the given path.
    */
  QIODevice *fileStream(const QString &path);

  /**
    @brief the name of the project
    */
Q_PROPERTY(QString name READ name WRITE setName)
  QString name() const;

  /**
    @brief User-written description of the project
    */
Q_PROPERTY(QString description READ description WRITE setDescription)
  QString description() const;

  /**
    @brief Name of the author
    */
Q_PROPERTY(QString author READ author WRITE setAuthor)
  QString author() const;

  /**
    @brief name of the perspective associated to the project.
    When user open a project from Tulip, this porperty is first read to identify find kind of perspective plugin should be launched to
    open the project

    @warning If the perspective name associated to the project is invalid or correspond to a missing plugin, tulip may not be able to open the file.
    */
Q_PROPERTY(QString perspective READ perspective WRITE setPerspective)
  QString perspective() const;

  /**
    @brief Date of last modification of the project.
    This is basically the date of last modification indicated by the file system.
    */
Q_PROPERTY(QDate date READ date)
  QDate date() const;

  /**
    @brief The version of the Tulip project format with which the file was created.
    Project from older format versions will always by saved into the newest version available.
    */
Q_PROPERTY(QString version READ version)
  QString version() const;

public slots:
  void setName(const QString &);
  void setDescription(const QString &);
  void setAuthor(const QString &);
  void setPerspective(const QString &);
};

}
#endif // TULIPPROJECT_H

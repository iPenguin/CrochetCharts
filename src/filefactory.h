/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef FILEFACTORY_H
#define FILEFACTORY_H

#include <qglobal.h>

#ifdef Q_WS_MAC
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#else
class QXmlStreamReader;
class QXmlStreamWriter;
#endif //Q_WS_MAC

#include <QTableWidget>
class MainWindow;

class FileFactory
{
public:
    friend class File_v1;
    friend class File_v2;

    enum FileVersion { Version_1_0 = 100, Version_1_2 = 102, Version_Auto = 255 };
    enum FileError { No_Error, Err_WrongFileType, Err_UnknownFileVersion, Err_OpeningFile, Err_GettingFileContents,
                     Err_NoTabsToSave, Err_RemovingOrigFile, Err_RenamingTempFile };

    FileFactory(QWidget *parent);

    FileFactory::FileError load();

    /**
     * @brief save - save the file.
     * @param saveVersion - the default is 255 or auto save
     * @return
     */
    FileFactory::FileError save(FileVersion saveVersion = FileFactory::Version_Auto);

    /**
     * @brief isOldFileVersion - tells the software that the file loaded was from a previous savefile version.
     *
     * If the save file is old we want to offer a choice to the user of updating the save file version.
     * And also to remember to save to the correct file version.
     *
     * @return true if it's an old file version.
     */
    bool isOldFileVersion() { return false; }

    void cleanUp();

    bool isSaved;
    QString fileName;

private:

    //mCurrentFileVersion is the fileVersion of the save file we're working with.
    qint32 mCurrentFileVersion;
    //mFileVersion is the native fileVersion of this version of the software.
    qint32 mFileVersion;

    QWidget *mParent;
    MainWindow *mMainWindow;
    QTabWidget *mTabWidget;


};

#endif // FILEFACTORY_H

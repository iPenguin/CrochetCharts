/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef FILEFACTORY_H
#define FILEFACTORY_H

#include <qglobal.h>

#include "stitchset.h"

#include <QTabWidget>

#ifdef Q_WS_MAC
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#else
class QXmlStreamReader;
class QXmlStreamWriter;
#endif //Q_WS_MAC

class FileFactory
{
public:
    friend class FileLoad_v1;
    friend class FileLoad_v2;

    enum FileVersion { Version_1_0 = 100, Version_1_2 = 102 };
    enum FileError { No_Error, Err_WrongFileType, Err_UnknownFileVersion, Err_OpeningFile, Err_GettingFileContents,
                     Err_NoTabsToSave, Err_RemovingOrigFile, Err_RenamingTempFile };

    FileFactory(QWidget* parent);
    
    FileFactory::FileError load();
    FileFactory::FileError save();

    bool isOldFileVersion() { return false; }

    void cleanUp();

    bool isSaved;
    QString fileName;

private:
    void saveCustomStitches(QXmlStreamWriter* stream);
    void saveColors(QXmlStreamWriter* stream);
    bool saveCharts(QXmlStreamWriter* stream);

private:

    //fileVersion of the file we're working with.
    qint32 mCurrentFileVersion;
    //fileVersion of the current version of the software.
    qint32 mFileVersion;

    QTabWidget* mTabWidget;
    QWidget* mParent;
    StitchSet* mInternalStitchSet;
};

#endif // FILEFACTORY_H

/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "filefactory.h"
#include "file_v1.h"
#include "file_v2.h"

#include <QObject>

#include "debug.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QTemporaryFile>

#include "crochettab.h"

#include "scene.h"
#include "stitchlibrary.h"
#include "stitchset.h"
#include "appinfo.h"

#include "mainwindow.h"

FileFactory::FileFactory(QWidget* parent) :
    isSaved(false),
    fileName(""),
    mFileVersion(FileFactory::Version_1_2),
    mParent(parent)
{
    mMainWindow = static_cast<MainWindow*>(mParent);
    mTabWidget = mMainWindow->tabWidget();
}

FileFactory::FileError FileFactory::load()
{
    File *fileLoad = 0;
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        //TODO: some nice dialog to warn the user.
        WARN("Couldn't open file for reading..." + fileName);
        return FileFactory::Err_OpeningFile;
    }

    QDataStream in(&file);

    quint32 magicNumber;
    qint32 version;
    in >> magicNumber;

    if(magicNumber != AppInfo::inst()->magicNumber) {
        qWarning() << "This is not a pattern file";
        file.close();
        return FileFactory::Err_WrongFileType;
    }

    in >> version;

    if(version < FileFactory::Version_1_0) {
        qWarning() << "Unknown file version";
        file.close();
        return FileFactory::Err_UnknownFileVersion;
    }

    if(version > mFileVersion) {
        qWarning() << "This file was created with a newer version of the software.";
        file.close();
        return FileFactory::Err_NewerFileVersion;
    }

    if(version == FileFactory::Version_1_0) {
        in.setVersion(QDataStream::Qt_4_7);
        fileLoad = new File_v1(mMainWindow, this);
    } else if(version == FileFactory::Version_1_2) {
        in.setVersion(QDataStream::Qt_4_7);
        fileLoad = new File_v2(mMainWindow, this);
    }

    return fileLoad->load(&in);
}

FileFactory::FileError FileFactory::save(FileVersion version)
{
    if(version == FileFactory::Version_Auto) {
        version = (FileVersion)mCurrentFileVersion;
    } else {
        mCurrentFileVersion = version;
    }

    //Don't save a file without at least 1 tab.
    if(mTabWidget->count() <= 0)
        return FileFactory::Err_NoTabsToSave;

    QTemporaryFile f;
    if(!f.open()) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for writing..." << f.fileName();
        return FileFactory::Err_OpeningFile;
    }

    QDataStream out(&f);
    // Write a header with a "magic number" and a version
    out << AppInfo::inst()->magicNumber;

    File *saveFile = 0;

    switch(version) {
        default:
        case FileFactory::Version_1_2:
            saveFile = new File_v2(mMainWindow, this);
            break;

        case FileFactory::Version_1_0:
            saveFile = new File_v1(mMainWindow, this);
            break;
    }

    int error = saveFile->save(&out);

    f.close();

    if(error != FileFactory::No_Error)
        return (FileFactory::FileError)error;

    QDir d(QFileInfo(fileName).path());
    //only try to delete the file if it exists.
    if(d.exists(fileName)) {
        if(!d.remove(fileName))
            return FileFactory::Err_RemovingOrigFile;
    }

    if(!f.copy(fileName)) {
        qDebug() << "Could not write final output file." << f.fileName() << fileName;
        return FileFactory::Err_RenamingTempFile;
    }

    return FileFactory::No_Error;
}

void FileFactory::cleanUp()
{

}

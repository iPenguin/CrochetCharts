/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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
    enum FileError { No_Error,
                    Err_OpeningFile,         //could not open file for reading or writing
                    Err_WrongFileType,       //magic number doesn't match
                    Err_UnknownFileVersion,  //file version is less then any known version
                    Err_NewerFileVersion,    //file version is > this version is capable of working with
                    Err_GettingFileContents, //get the xml file content
                    Err_NoTabsToSave,        //don't save the file we don't have any tabs
                    Err_RemovingOrigFile,    //couldn't remove the save file
                    Err_RenamingTempFile,    //couldn't copy temp file to the save file name
                    Err_SavingFile,
                    Err_LoadingFile
                    };

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

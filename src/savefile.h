/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <qglobal.h>
#include <qmap.h>

#include <QString>
#include <QTabWidget>

#ifdef Q_WS_MAC
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#else
class QXmlStreamReader;
class QXmlStreamWriter;
#endif //Q_WS_MAC

class QDataStream;

class CrochetTab;

class SaveFile
{

public:
    SaveFile(QWidget* parent);
    ~SaveFile();

    enum FileVersion { Version_1_0 = 100 };
    enum FileError { No_Error, Err_WrongFileType, Err_UnknownFileVersion, Err_OpeningFile, Err_GettingFileContents };
    
    SaveFile::FileError save();
    SaveFile::FileError load();

    bool isOldFileVersion();

    bool isSaved;
    QString fileName;
    
private:
    bool saveCustomStitches(QDataStream *stream);   
    void saveColors(QXmlStreamWriter *stream);
    bool saveCharts(QXmlStreamWriter *stream);

    bool loadCustomStitches(QDataStream *stream);
    void loadColors(QXmlStreamReader *stream);
    void loadChart(QXmlStreamReader *stream);
    void loadCell(CrochetTab *tab, QXmlStreamReader *stream);
    QTransform loadTransform(QXmlStreamReader *stream);

    void loadIndicator(CrochetTab *tab, QXmlStreamReader *stream);

    //fileVersion of the file we're working with.
    qint32 mCurrentFileVersion;
    //fileVersion of the current version of the software.
    qint32 mFileVersion;

    QTabWidget* mTabWidget;

    QWidget* mParent;
};
#endif //SAVEFILE_H

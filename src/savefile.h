/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <qglobal.h>
#include <qmap.h>

class SaveFile
{

public:
    SaveFile();
    ~SaveFile();

    enum FileVersion { Version_1_0 = 100 };
    
    bool save(QString fileName);
    bool load(QString fileName);

    bool isSaved();

    bool isOldFileVersion();

private:
    bool saveCustomStitches();
    bool saveChart();
    bool saveCell();
    bool saveStitch();

    bool loadCustomStitches();
    bool loadChart();
    bool loadCell();
    bool loadStitch();

    quint32 mFileVersion;
};
#endif //SAVEFILE_H
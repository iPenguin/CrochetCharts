/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "savefile.h"
#include "appinfo.h"

SaveFile::SaveFile()
{
}

SaveFile::~SaveFile()
{
}

bool SaveFile::save(QString fileName)
{

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    
    // Write a header with a "magic number" and a version
    out << AppInfo::magicNumber;
    out << (qint32)123;
    
    out.setVersion(QDataStream::Qt_4_7);
    
    // Write the data
    //out << lots_of_interesting_data;
    
}

bool SaveFile::load(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    
    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != AppInfo::magicNumber)
        return XXX_BAD_FILE_FORMAT;
    
    // Read the version
        qint32 version;
        in >> version;
        if (version < FileVersion::Version_1_0)
            in.setVersion(QDataStream::Qt_4_7);
        
        // Read the data
        //in >> lots_of_interesting_data;
        //if (version >= 120)
        //    in >> data_new_in_XXX_version_1_2;
        //in >> other_interesting_data;
}

bool SaveFile::isSaved()
{
    
}

bool SaveFile::saveCustomStitches()
{
    
}

bool SaveFile::saveChart()
{
    
}

bool SaveFile::saveCell()
{
    
}

bool SaveFile::saveStitch()
{
    
}

bool SaveFile::loadCustomStitches()
{
    
}

bool SaveFile::loadChart()
{
    
}

bool SaveFile::loadCell()
{
    
}

bool SaveFile::loadStitch()
{
    
}

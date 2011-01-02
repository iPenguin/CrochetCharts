#include "stitchset.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// Global static pointer
StitchSet* StitchSet::mInstance = NULL;

// singleton constructor:
StitchSet* StitchSet::inst()
{
   if (!mInstance)   // Only allow one instance of the settings.
      mInstance = new StitchSet();
   return mInstance;
}

StitchSet::StitchSet()
{

}

void StitchSet::loadStitches()
{
    this->loadXmlStitches("stitches/stitches.xml"); //default stitches.
/*
    QStringList userFolders;

    foreach(QString folder, userFolders) {
        //load the stitches.xml file from each custom set.
        this->loadXmlStitches(folder+"/stitches.xml");
    }

    QString userConfigFolder;
    //load any user overlays.
    this->loadXmlStitches(userConfigFolder+"/stitches.xml");
*/
}

void StitchSet::loadXmlStitches(QString fileName)
{
    QXmlStreamReader xml;

    while (!xml.atEnd()) {
        xml.readNext();
        // do processing
    }
    if (xml.hasError()) {
        // do error handling
    }

}

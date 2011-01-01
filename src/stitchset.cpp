#include "stitchset.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

StitchSet::StitchSet()
{

}

void StitchSet::loadStitches()
{
    this->loadXmlStitches("stitches.xml");

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

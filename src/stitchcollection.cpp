/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchcollection.h"

#include "stitchset.h"
#include "stitch.h"

#include "settings.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QComboBox>

#include <QDebug>

// Global static pointer
StitchCollection* StitchCollection::mInstance = NULL;

// singleton constructor:
StitchCollection* StitchCollection::inst()
{
   if (!mInstance)   // Only allow one instance of the settings.
      mInstance = new StitchCollection();
   return mInstance;
}

StitchCollection::StitchCollection()
{ 
    mMasterSet = new StitchSet(tr("Master Stitch Set"), this);
}

StitchCollection::~StitchCollection()
{
    saveMasterStitchSet("/home/brian/stitches.xml");
}

void StitchCollection::loadStitchSets()
{
    StitchSet *set = new StitchSet(this);
    set->loadXmlStitchSet("/home/brian/crochet.git/stitches/stitches.xml");

    mStitchSets.append(set);
}

void StitchCollection::populateMasterSet()
{
    //TODO: load from a user config folder. this stitch set should be saved back to disk
    // anytime the set is edited. This is the only set that can be edited.
    mMasterSet->loadXmlStitchSet("/home/brian/stitches.xml");
}

void StitchCollection::saveMasterStitchSet(QString fileName)
{
//TODO: push this off into the StitchSet.
// The user can create a "new" set that can then be saved to a user specified file.
    QString *data = new QString();

    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    QString fName = Settings::inst()->value("firstName").toString();
    QString lName = Settings::inst()->value("lastName").toString();
    QString email = Settings::inst()->value("email").toString();

    //TODO: figure out all the pieces or remove them...
    stream.writeStartElement("stitch_set");
    stream.writeTextElement("name", "Master Stitch Set");
    stream.writeTextElement("author", fName + " " + lName);
    stream.writeTextElement("email", email);
    stream.writeTextElement("org", "");
    stream.writeTextElement("url", "");

    foreach(Stitch *s, mMasterSet->stitches()) {
        stream.writeStartElement("stitch");

        stream.writeTextElement("name", s->name());
        stream.writeTextElement("icon", s->file());
        stream.writeTextElement("description", s->description());
        stream.writeTextElement("category", s->category());
        stream.writeTextElement("ws", s->wrongSide());

        stream.writeEndElement(); //stitch
    }

    stream.writeEndElement(); // stitch_set

    stream.writeEndDocument();

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly)) {
    //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for writing..." << fileName;
        return;
    }

    file.write(data->toLatin1());

    delete data;
    data = 0;
}

StitchSet* StitchCollection::findStitchSet(QString setName)
{
    foreach(StitchSet *set, mStitchSets) {
        if(set->name() == setName)
            return set;
    }

    if(mMasterSet->name() == setName)
        return mMasterSet;

    return 0;
}

void StitchCollection::populateComboBox(QComboBox *cb)
{
    cb->addItem(mMasterSet->name());

    foreach(StitchSet *set, mStitchSets) {
        cb->addItem(set->name());
    }
}

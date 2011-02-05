/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchcollection.h"

#include "stitchset.h"
#include "stitch.h"

#include <QFile>
#include <QComboBox>

#include <QDebug>

// Global static pointer
StitchCollection* StitchCollection::mInstance = NULL;

// singleton constructor:
StitchCollection* StitchCollection::inst()
{
   if (!mInstance)   // Only allow one instance of the StitchCollection.
      mInstance = new StitchCollection();
   return mInstance;
}

StitchCollection::StitchCollection()
{ 
    mMasterSet = new StitchSet(this, true);
}

StitchCollection::~StitchCollection()
{
    saveMasterStitchSet("/home/brian/stitches.xml");
    foreach(StitchSet *set, mStitchSets) {
        delete set;
    }
    mStitchSets.clear();
}

void StitchCollection::loadStitchSets()
{
    mBuiltIn = new StitchSet(this, false, true);
    mBuiltIn->loadXmlStitchSet("/home/brian/crochet.git/stitches/stitches.xml");
    mStitchSets.append(mBuiltIn);

    //TODO: add the rest of the sets.
}

void StitchCollection::populateMasterSet()
{
    //TODO: load from a user config folder. this stitch set should be saved back to disk
    // anytime the set is edited. This is the only set that can be edited.
    mMasterSet->loadXmlStitchSet("/home/brian/stitches.xml");
}

void StitchCollection::saveMasterStitchSet(QString fileName)
{
    mMasterSet->saveXmlStitchSet(fileName);
}

Stitch* StitchCollection::findStitch(QString name)
{
    //FIXME: add alternative sets for example add a per document stitch set that can be searched.
    return mMasterSet->findStitch(name);

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

QStringList StitchCollection::categoryList() const
{
    QStringList list;

    foreach(Stitch *s, mMasterSet->stitches()) {
        if(!list.contains(s->category()))
            list.append(s->category());
    }

    return list;
}

QStringList StitchCollection::stitchList() const
{
    QStringList list;

    foreach(Stitch *s, mMasterSet->stitches()) {
        if(!list.contains(s->name()))
            list.append(s->name());
    }

    return list;
}

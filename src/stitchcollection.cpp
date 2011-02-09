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
#include <QDir>
#include <QFileInfo>
#include "settings.h"

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
    mMasterSet->saveXmlFile();
    //FIXME: only save the files if they've changed
    foreach(StitchSet *set, mStitchSets) {
        if(!set->isTemporary && !set->isBuiltInSet)
            set->saveXmlFile();
        delete set;
    }
    mStitchSets.clear();
}

void StitchCollection::loadStitchSets()
{
    mBuiltIn = new StitchSet(this, false, true);
    mBuiltIn->loadXmlFile(":/stitches.xml");
    mStitchSets.append(mBuiltIn);

    //Load additional stitch sets:
    //TODO: store all custom icons in [confFolder]/sets/setName/
    
    QString confFolder = Settings::inst()->userSettingsFolder();

    QDir dir = QDir(confFolder + "sets/");
    QStringList fileTypes;
    fileTypes << "*.xml";
  
    QFileInfoList list = dir.entryInfoList(fileTypes, QDir::Files | QDir::NoSymLinks);
    foreach(QFileInfo file, list) {
        StitchSet *set = new StitchSet(this, false, false);
        set->loadXmlFile(file.absoluteFilePath());
        mStitchSets.append(set);
    }
}

void StitchCollection::populateMasterSet()
{
    QString confFolder = Settings::inst()->userSettingsFolder();
    bool loaded = mMasterSet->loadXmlFile(confFolder + "stitches.xml");

    //if there isn't a master stitchset create it from the built in stitches.
    if(!loaded) {
        mMasterSet->clearStitches();
        foreach(Stitch *s, mBuiltIn->stitches()) {
            Stitch *newS = new Stitch();
            mMasterSet->addStitch(newS);
            *newS << *s;
        }
    }
}

Stitch* StitchCollection::findStitch(QString name)
{
    //FIXME: This needs some serious logic work.

    Stitch *found = 0;
    found = mMasterSet->findStitch(name);

    foreach(StitchSet *set, mStitchSets) {
        if(set->isTemporary)
            found = mMasterSet->findStitch(name);
    }
    
    return found;

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

QStringList StitchCollection::stitchSetList()
{
    QStringList list;

    list << mMasterSet->name();

    foreach(StitchSet *set, mStitchSets)
        list << set->name();

    return list;
}

QStringList StitchCollection::categoryList() const
{
    QStringList list;

    foreach(Stitch *s, mMasterSet->stitches()) {
        if(!list.contains(s->category()))
            list.append(s->category());
    }

    foreach(StitchSet *set, mStitchSets) {
        foreach(Stitch *s, set->stitches()) {
            if(!list.contains(s->category()))
                list.append(s->category());
        }
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

QString StitchCollection::nextSetSaveFile()
{
    QString baseName, fileName;
    QString ext = ".xml";

    baseName = Settings::inst()->userSettingsFolder();

    baseName += "sets/set";

    fileName = baseName + ext;
    int i = 1;

    while(QFileInfo(fileName).exists()) {
        fileName = baseName + QString::number(i) + ext;
        i++;
    }
    return fileName;
}

StitchSet* StitchCollection::addStitchSet(QString setName)
{
    if(setName.isEmpty())
        return 0;

    StitchSet *set = new StitchSet(this, false, false);
    set->setName(setName);
    mStitchSets.append(set);

    set->stitchSetFileName = nextSetSaveFile();
    return set;
}

void StitchCollection::removeSet(QString setName)
{
   StitchSet *set = findStitchSet(setName);
   mStitchSets.removeOne(set);

   //TODO: delete all files and folders
}

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
    saveMasterList();
    //mMasterSet->saveXmlFile();
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

    //Load additional stitch sets:
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
    bool loaded = loadMasterList();

    //if there isn't a master stitchset create it from the built in stitches.
    if(!loaded) {
        mMasterSet->clearStitches();
        mMasterSet->setName(tr("All Stitches"));
        foreach(Stitch *s, mBuiltIn->stitches()) {
            mMasterSet->addStitch(s);
            mStitchList.insert(s->name(), mBuiltIn->name());
        }
    }
}

bool StitchCollection::loadMasterList()
{
    QString confFolder = Settings::inst()->userSettingsFolder();
    QString fileName = confFolder + "stitches.list";

    if(!QFileInfo(fileName).exists())
        return false;
    
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QDataStream in(&file);

    in >> mStitchList;
    
    file.close();

    foreach(QString key, mStitchList.keys()) {
        Stitch *s = findStitchSet(mStitchList.value(key))->findStitch(key);
        mMasterSet->addStitch(s);
    }

    return true;
}

void StitchCollection::saveMasterList()
{
    QString confFolder = Settings::inst()->userSettingsFolder();
    QString fileName = confFolder + "stitches.list";
    
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QDataStream out(&file);

    out << mStitchList;
    
    file.close();
}

void StitchCollection::addStitchToMasterSet(StitchSet *set, Stitch *s)
{
    if(mMasterSet->hasStitch(s->name())) {
        mMasterSet->removeStitch(s->name());
    }
    mMasterSet->addStitch(s);
    mStitchList[s->name()] = set->name();
}

Stitch* StitchCollection::findStitch(QString name)
{
    Stitch *found = 0;
    found = mMasterSet->findStitch(name);
    
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
    if(mBuiltIn->name() == setName)
        return mBuiltIn;
    
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

    foreach(StitchSet *set, mStitchSets) {
        foreach(Stitch *s, set->stitches()) {
            if(!list.contains(s->name()))
                list.append(s->name());
        }
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

StitchSet* StitchCollection::createStitchSet(QString setName)
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

void StitchCollection::addStitchSet(QString fileName)
{
    if(fileName.isEmpty())
        return;

    if(!QFileInfo(fileName).exists())
        return;
    
    QString dest = nextSetSaveFile();

    //make a set folder
    QFileInfo info(dest);

    QDir(info.path()).mkpath(info.path() + "/" + info.baseName());
    
    StitchSet *set = new StitchSet();

    set->loadDataFile(fileName, dest);

    StitchSet *test = 0;
    test = findStitchSet(set->name());
    if(test) {
        //TODO: warn that a set with this name already exists.
        //Offer to overwrite (delete old and add new)
        //Offer to rename new set.
        //Offer to cancel (delete new)
    }

    mStitchSets.append(set);
}

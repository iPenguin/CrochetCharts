/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibrary.h"

#include "stitchset.h"
#include "stitch.h"

#include <QFile>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "settings.h"

// Global static pointer
StitchLibrary* StitchLibrary::mInstance = NULL;

// singleton constructor:
StitchLibrary* StitchLibrary::inst()
{
   if (!mInstance)   // Only allow one instance of the StitchLibrary.
      mInstance = new StitchLibrary();
   return mInstance;
}

StitchLibrary::StitchLibrary()
{ 
    mMasterSet = new StitchSet(this, true);
    mMasterSet->setName(tr("Master Stitch List"));
    connect(mMasterSet, SIGNAL(movedToOverlay(QString)), SLOT(moveStitchToOverlay(QString)));
}

StitchLibrary::~StitchLibrary()
{
    saveMasterList();

    mOverlay->saveXmlFile();
    
    foreach(StitchSet* set, mStitchSets) {
        mStitchSets.removeOne(set);
        if(!set->isTemporary)
            set->saveXmlFile();
    }
}

void StitchLibrary::saveAllSets()
{
    saveMasterList();

    mOverlay->saveXmlFile();
    
    foreach(StitchSet* set, mStitchSets) {
        if(!set->isTemporary) {
            set->saveXmlFile();
        }
    }
}

void StitchLibrary::loadStitchSets()
{
    QString confFolder = Settings::inst()->userSettingsFolder();

    mMasterSet->loadXmlFile(":/crochet.xml");
    foreach(Stitch* s, mMasterSet->stitches())
        s->isBuiltIn = true;
    
    mOverlay = new StitchSet(this, false);

    QString overlay = confFolder + "overlay.set";
    if(QFileInfo(overlay).exists()) {
        mOverlay->loadXmlFile(overlay);
    } else {
        mOverlay->stitchSetFileName = overlay;
        mOverlay->setName(tr("SWS Overlay"));
    }

    connect(mMasterSet, SIGNAL(stitchNameChanged(QString,QString,QString)),
            SLOT(changeStitchName(QString,QString,QString)));
    connect(mOverlay, SIGNAL(stitchNameChanged(QString,QString,QString)),
            SLOT(changeStitchName(QString,QString,QString)));

    //Load additional stitch sets:
    QDir dir = QDir(confFolder);
    QStringList fileTypes;
    fileTypes << "*.xml";
  
    QFileInfoList list = dir.entryInfoList(fileTypes, QDir::Files | QDir::NoSymLinks);
    foreach(QFileInfo file, list) {
        StitchSet* set = new StitchSet(this, false);
        set->loadXmlFile(file.absoluteFilePath());
        mStitchSets.append(set);
        connect(set, SIGNAL(stitchNameChanged(QString,QString,QString)),
                SLOT(changeStitchName(QString,QString,QString)));
    }

    bool loaded = loadMasterList();

    //if there isn't a master stitchset create it from the built in stitches.
    if(!loaded)
        resetMasterStitchSet();
}

bool StitchLibrary::loadMasterList()
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
        StitchSet* set = findStitchSet(mStitchList.value(key));
        if(set) {
            Stitch* s = set->findStitch(key);
            if(s) {
                if(mMasterSet->hasStitch(s->name())) {
                    mMasterSet->removeStitch(s->name());
                }
                mMasterSet->addStitch(s);
            }
        }
    }
    
    return true;
}

void StitchLibrary::saveMasterList()
{
    QString confFolder = Settings::inst()->userSettingsFolder();
    QString fileName = confFolder + "stitches.list";
    
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QDataStream out(&file);

    out << mStitchList;
    
    file.close();
}

void StitchLibrary::resetMasterStitchSet()
{
    //mMasterSet->beginResetModel();
    mMasterSet->clearStitches();
    mOverlay->clearStitches();
    mStitchList.clear();
    
    mMasterSet->reset();
    //mMasterSet->endResetModel();
}

void StitchLibrary::addStitchToMasterSet(StitchSet* set, Stitch* s)
{
    if(mMasterSet->hasStitch(s->name())) {
        mMasterSet->removeStitch(s->name());
    }
    mMasterSet->addStitch(s);
    mStitchList[s->name()] = set->name();

    emit stitchListChanged();
}

void StitchLibrary::removeStitchFormMasterSet(Stitch* s)
{
    if(!masterHasStitch(s))
        return;

    //Don't delete the stitch as this is the master set
    //and it's only a link to the real stitch.
    mStitchList.remove(s->name());
    mMasterSet->removeStitch(s->name());
}

void StitchLibrary::moveStitchToOverlay(QString stitchName)
{
    mStitchList[stitchName] = mOverlay->name();
    mOverlay->addStitch(mMasterSet->findStitch(stitchName));
}

bool StitchLibrary::masterHasStitch(Stitch* s)
{
    return mMasterSet->stitches().contains(s);
}

Stitch* StitchLibrary::findStitch(QString name)
{
    return mMasterSet->findStitch(name);
}

StitchSet* StitchLibrary::findStitchSet(QString setName)
{
    foreach(StitchSet* set, mStitchSets) {
        if(set->name() == setName)
            return set;
    }

    if(mMasterSet->name() == setName)
        return mMasterSet;
    if(mOverlay->name() == setName)
        return mOverlay;
    
    return 0;
}

QStringList StitchLibrary::stitchSetList()
{
    QStringList list;

    list << mMasterSet->name();

    foreach(StitchSet* set, mStitchSets)
        list << set->name();

    return list;
}

QStringList StitchLibrary::categoryList() const
{
    QStringList list;

    foreach(Stitch* s, mMasterSet->stitches()) {
        if(!list.contains(s->category()))
            list.append(s->category());
    }

    foreach(StitchSet* set, mStitchSets) {
        foreach(Stitch* s, set->stitches()) {
            if(!list.contains(s->category()))
                list.append(s->category());
        }
    }
    
    return list;
}

QStringList StitchLibrary::stitchList(bool showAllSets) const
{
    QStringList list;

    foreach(Stitch* s, mMasterSet->stitches()) {
        if(!list.contains(s->name()))
            list.append(s->name());
    }

    if(showAllSets) {
        foreach(StitchSet* set, mStitchSets) {
            foreach(Stitch* s, set->stitches()) {
                if(!list.contains(s->name()))
                    list.append(s->name());
            }
        }
    }
    list.sort();

    return list;
}

QString StitchLibrary::nextSetSaveFile()
{
    QString baseName, fileName;
    QString ext = ".xml";

    baseName = Settings::inst()->userSettingsFolder();

    baseName += "set";

    fileName = baseName + ext;
    int i = 1;

    while(QFileInfo(fileName).exists()) {
        fileName = baseName + QString::number(i) + ext;
        i++;
    }
    return fileName;
}

StitchSet* StitchLibrary::createStitchSet(QString setName)
{
    if(setName.isEmpty())
        return 0;

    StitchSet* set = new StitchSet(this, false);
    set->setName(setName);
    mStitchSets.append(set);

    set->stitchSetFileName = nextSetSaveFile();
    return set;
}

void StitchLibrary::removeSet(QString setName)
{
   StitchSet* set = findStitchSet(setName);
   removeSet(set);
}

void StitchLibrary::removeSet(StitchSet* set)
{

    if(mStitchSets.contains(set)) {
        mStitchSets.removeOne(set);

        removeMasterStitches(set);

        set->removeDir(set->stitchSetFolder());
        QDir setsDir(Settings::inst()->userSettingsFolder());
        setsDir.remove(set->stitchSetFileName);

        set->deleteLater();
    }

}

void StitchLibrary::removeMasterStitches(StitchSet* set)
{

    foreach(Stitch* s, set->stitches()) {
        if(mStitchList.contains(s->name())) {
            if(mStitchList.value(s->name()) == set->name()) {
                mMasterSet->removeStitch(s->name());
                mStitchList.remove(s->name());
            }
        }
    }
}

void StitchLibrary::addStitchSet(StitchSet* set)
{
    connect(set, SIGNAL(stitchNameChanged(QString,QString,QString)), this, SLOT(changeStitchName(QString,QString,QString)));
    mStitchSets.append(set);
}

void StitchLibrary::changeStitchName(QString setName, QString oldName, QString newName)
{
    if(setName == mMasterSet->name())
        setName = mStitchList.value(oldName);
    
    //update the stitchList with the new stitch name.
    if (mStitchList.value(oldName) == setName) {
        mStitchList.remove(oldName);
        mStitchList[newName] = setName;
    }
    
    emit stitchListChanged();
}

void StitchLibrary::reloadAllStitchIcons()
{
    foreach(StitchSet* set, mStitchSets)
        set->reloadStitchIcons();
}

QString StitchLibrary::findStitchSetName(QString folderName)
{
    if(!folderName.endsWith("/"))
        folderName.append("/");

    foreach(StitchSet* set, mStitchSets) {
        if(set->stitchSetFolder() == folderName)
            return set->name();
    }

    return QString("Unknown set");
}

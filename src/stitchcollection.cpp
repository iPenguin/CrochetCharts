/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchcollection.h"

#include "stitchset.h"
#include "stitch.h"

#include <QFile>
#include <QComboBox>
#include <QPushButton>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "settings.h"
#include <QMessageBox>
#include <QInputDialog>

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
    mMasterSet->setName(tr("All Stitches"));
}

StitchCollection::~StitchCollection()
{
    saveMasterList();
    //mMasterSet->saveXmlFile();
    //FIXME: only save the files if they've changed
    foreach(StitchSet *set, mStitchSets) {
        if(!set->isTemporary)
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
        connect(set, SIGNAL(stitchNameChanged(QString,QString,QString)), this, SLOT(changeStitchName(QString,QString,QString)));
    }
}

void StitchCollection::populateMasterSet()
{
    bool loaded = loadMasterList();

    //if there isn't a master stitchset create it from the built in stitches.
    if(!loaded)
        resetMasterStitchSet();
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
        StitchSet *set = findStitchSet(mStitchList.value(key));
        if(set){
            Stitch *s = set->findStitch(key);
            if(s)
                mMasterSet->addStitch(s);
        }
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

void StitchCollection::resetMasterStitchSet()
{
    mMasterSet->clearStitches();
    mStitchList.clear();

    foreach(Stitch *s, mBuiltIn->stitches()) {
        mMasterSet->addStitch(s);
        mStitchList.insert(s->name(), mBuiltIn->name());
    }
    mMasterSet->refreshSet();
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

QStringList StitchCollection::stitchList(bool showAllSets) const
{
    QStringList list;

    foreach(Stitch *s, mMasterSet->stitches()) {
        if(!list.contains(s->name()))
            list.append(s->name());
    }

    if(showAllSets) {
        foreach(StitchSet *set, mStitchSets) {
            foreach(Stitch *s, set->stitches()) {
                if(!list.contains(s->name()))
                    list.append(s->name());
            }
        }
    }
    list.sort();

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

//FIXME: return a value that can be checked and move the gui dialogs into the libraryui.
StitchSet* StitchCollection::addStitchSet(QString fileName)
{
    if(fileName.isEmpty())
        return 0;

    if(!QFileInfo(fileName).exists())
        return 0;
    
    QString dest = nextSetSaveFile();

    //make a set folder
    QFileInfo info(dest);

    QDir(info.path()).mkpath(info.path() + "/" + info.baseName());
    
    StitchSet *set = new StitchSet();

    set->loadDataFile(fileName, dest);

    StitchSet *test = 0;
    test = findStitchSet(set->name());
    if(test) {
        QMessageBox msgbox;
        msgbox.setText(tr("A Set with this name already exists in your collection."));
        msgbox.setInformativeText(tr("What would you like to do with this set?"));
        msgbox.setIcon(QMessageBox::Question);
        QPushButton *overwrite = msgbox.addButton(tr("Overwrite the existing set"), QMessageBox::AcceptRole);
        QPushButton *rename = msgbox.addButton(tr("Rename the new stitch set"), QMessageBox::ApplyRole);
        /*QPushButton *cancel =*/ msgbox.addButton(tr("Cancel adding the new set"), QMessageBox::RejectRole);

        msgbox.exec();
        
        if(msgbox.clickedButton() == overwrite) {
            delete test;
            test = 0;
            mStitchSets.removeOne(test);
            //FIXME: this is going to cause crashes when removing sets with sts in the master list!
        } else if(msgbox.clickedButton() == rename) {
            bool ok;
            QString text;
            
            while(!ok || text.isEmpty() || text == set->name() ){
                text = QInputDialog::getText(0, tr("New Set Name"), tr("Stitch set name:"),
                                            QLineEdit::Normal, set->name(), &ok);
            }
            //TODO: allow the user to 'cancel out' of this loop.
            set->setName(text);
            
        } else {
            delete set;
            set = 0;
            return 0;
        }
    }
    connect(set, SIGNAL(stitchNameChanged(QString,QString,QString)), this, SLOT(changeStitchName(QString,QString,QString)));
    mStitchSets.append(set);
    return set;
}

void StitchCollection::changeStitchName(QString setName, QString oldName, QString newName)
{
    //update the stitchList with the new stitch name.
    if (mStitchList.value(oldName) == setName) {
        mStitchList.remove(oldName);
        mStitchList[newName] = setName;
    }
}

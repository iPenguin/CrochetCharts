/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibrary.h"

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
}

StitchLibrary::~StitchLibrary()
{
    qDebug() << "~StitchLibrary";
    saveMasterList();
    
    foreach(StitchSet *set, mStitchSets) {
        mStitchSets.removeOne(set);
        if(!set->isTemporary)
            set->saveXmlFile();
        delete set;
    }
}

void StitchLibrary::saveAllSets()
{
    saveMasterList();
    
    foreach(StitchSet *set, mStitchSets) {
        if(!set->isTemporary) {
            set->saveXmlFile();
        }
    }
}

void StitchLibrary::loadStitchSets()
{
    QString confFolder = Settings::inst()->userSettingsFolder();
    
    mBuiltIn = new StitchSet(this, false, true);

    QString masterSet = confFolder + "master.set";
    if(QFileInfo(masterSet).exists())
        mBuiltIn->loadXmlFile(masterSet);
    else {
        mBuiltIn->loadXmlFile(":/stitches.xml");
        mBuiltIn->stitchSetFileName = masterSet;
        mBuiltIn->saveXmlFile();
    }
    connect(mBuiltIn, SIGNAL(stitchNameChanged(QString,QString,QString)),
            this, SLOT(changeStitchName(QString,QString,QString)));
    
    mStitchSets.append(mBuiltIn);
    //Load additional stitch sets:
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

void StitchLibrary::setupMasterSet()
{
    bool loaded = loadMasterList();

    //if there isn't a master stitchset create it from the built in stitches.
    if(!loaded)
        resetMasterStitchSet();

    connect(mMasterSet, SIGNAL(stitchNameChanged(QString,QString,QString)),
            this, SLOT(changeStitchName(QString,QString,QString)));
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
        StitchSet *set = findStitchSet(mStitchList.value(key));
        if(set){
            Stitch *s = set->findStitch(key);
            if(s)
                mMasterSet->addStitch(s);
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
    mMasterSet->clearStitches();
    mStitchList.clear();

    foreach(Stitch *s, mBuiltIn->stitches()) {
        mMasterSet->addStitch(s);
        mStitchList.insert(s->name(), mBuiltIn->name());
    }
    mMasterSet->refreshSet();
}

void StitchLibrary::addStitchToMasterSet(StitchSet *set, Stitch *s)
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

bool StitchLibrary::masterHasStitch(Stitch* s)
{
    return mMasterSet->stitches().contains(s);
}

Stitch* StitchLibrary::findStitch(QString name)
{
    Stitch *found = 0;
    found = mMasterSet->findStitch(name);
    
    return found;

}

StitchSet* StitchLibrary::findStitchSet(QString setName)
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

QStringList StitchLibrary::stitchSetList()
{
    QStringList list;

    list << mMasterSet->name();

    foreach(StitchSet *set, mStitchSets)
        list << set->name();

    return list;
}

QStringList StitchLibrary::categoryList() const
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

QStringList StitchLibrary::stitchList(bool showAllSets) const
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

QString StitchLibrary::nextSetSaveFile()
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

StitchSet* StitchLibrary::createStitchSet(QString setName)
{
    if(setName.isEmpty())
        return 0;

    StitchSet *set = new StitchSet(this, false, false);
    set->setName(setName);
    mStitchSets.append(set);

    set->stitchSetFileName = nextSetSaveFile();
    return set;
}

void StitchLibrary::removeSet(QString setName)
{
   StitchSet *set = findStitchSet(setName);
   mStitchSets.removeOne(set);

   QDir setsDir(Settings::inst()->userSettingsFolder() + "sets/");
   setsDir.rmdir(set->stitchSetFolder());
   setsDir.remove(set->stitchSetFileName);

   delete set;
   set = 0;
}

//FIXME: return a value that can be checked and move the gui dialogs into the libraryui.
StitchSet* StitchLibrary::addStitchSet(QString fileName)
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
        msgbox.setText(tr("A Set with this name already exists in your stitch library."));
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

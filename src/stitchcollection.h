/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCHCOLLECTION_H
#define STITCHCOLLECTION_H

#include <QObject>
#include <QStringList>

class StitchSet;
class Stitch;

class QComboBox;

/**
 * The StitchCollection is a set of StitchSet*s
 *
 * The Collection keeps track of all sets. Specifically the Collection
 * keeps track of the builtIn Set, and the masterSet. And knows about
 * temporary sets that are loaded when a file with custom stitches is
 * opened.
 *
 * The StitchCollection acts as the point of reference for StitchSet
 * based operations such as findStitch();
 */
class StitchCollection : public QObject
{
    Q_OBJECT

public:
    static StitchCollection* inst();
    ~StitchCollection();

    //return the list of stitch sets.
    QList<StitchSet *> stitchSets() { return mStitchSets; }
    //return the master stitch set.
    StitchSet* masterStitchSet() { return mMasterSet; }
    StitchSet* builtIn() { return mBuiltIn; }
    
    //load all known stitch sets.
    void loadStitchSets();
    //create links to the stitches being used from each set/overlay
    void populateMasterSet();

    //fill in a dropdown list for selecting a stitch set.
    QStringList stitchSetList();

    //this function finds the named stitch it can search the master set and any alternative sets.
    Stitch* findStitch(QString name);
    
    StitchSet* findStitchSet(QString setName);
    QStringList categoryList() const;
    QStringList stitchList() const;

    //creates a new set to the collection, and returns a pointer to it.
    StitchSet* createStitchSet(QString setName);
    void removeSet(QString setName);

    //add a stitch set from a file.
    void addStitchSet(QString fileName);
    
private:
    StitchCollection();

    //generates the next file name that can be used for a stitch set.
    QString nextSetSaveFile();
    
    static StitchCollection* mInstance;

    QList<StitchSet*> mStitchSets;
    StitchSet* mMasterSet;
    StitchSet* mBuiltIn;

};

#endif //STITCHCOLLECTION_H

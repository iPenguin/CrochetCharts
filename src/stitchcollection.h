#ifndef STITCHCOLLECTION_H
#define STITCHCOLLECTION_H

#include <QObject>

class StitchSet;
class Stitch;

/*
    for every set of stitches the user has we'll load a StitchSet (including the user overlay).
    Once all the sets are loaded we want to combine all the stitches into one master list.

    This master list of stitches is what gets searched, and used. The collection acts as the
    gatekeeper and if the user 'imports' a stitch from one set that is already in use it turns
    the other stitch 'off'.

    Ideally the collection is just a collection of pointers to the real stitches in the other
    sets.
*/
class StitchCollection : public QObject
{
    Q_OBJECT

public:
    StitchCollection(QObject *parent = 0);

private:
    QList<StitchSet *> mStitchSets;
    StitchSet *mStitches;

};

#endif //STITCHCOLLECTION_H

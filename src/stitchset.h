#ifndef STITCHSET_H
#define STITCHSET_H

#include <QList>
#include <QStandardItemModel>

class StitchSet : public QStandardItemModel
{
    Q_OBJECT
public:
    StitchSet();

    void loadStitches();
private:
    void loadXmlStitches(QString fileName);

    //TODO: figure out which stitches should be saved to which file...
    void saveXmlStitches(QString fileName);

    QList<Stitch *> mStitches;

};


#endif //STITCHSET_H

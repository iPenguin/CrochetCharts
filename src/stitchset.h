#ifndef STITCHSET_H
#define STITCHSET_H

#include <QStandardItemModel>

class StitchSet : public QStandardItemModel
{
    Q_OBJECT
public:
    StitchSet();

    void loadXmlStitches(QString fileName);
private:

};


#endif //STITCHSET_H

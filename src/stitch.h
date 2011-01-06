/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCH_H
#define STITCH_H

#include <QString>
#include <QList>
#include <QVariant>

class QSvgRenderer;
class QPixmap;

class Stitch
{
public:
    Stitch(Stitch *parent = 0);
    ~Stitch();

    void setName(QString name) { mName = name; }
    const QString name() { return mName; }

    void setFile(QString file) { mFile = file; }
    const QString file() { return mFile; }

    void setDescription(QString desc) { mDescription = desc; }
    const QString description() { return mDescription; }

    void setCategory(QString cat) { mCategory = cat; }
    const QString category() { return mCategory; }

    void setWrongSide(QString ws) { mWrongSide = ws; }
    const QString wrongSide() { return mWrongSide; }

    bool isSvg();

    QPixmap* renderPixmap();
    QSvgRenderer* renderSvg();

private:

    QString mName;
    QString mFile;
    QString mDescription;
    QString mCategory;
    QString mWrongSide;

    QPixmap *mPixmap;
    QSvgRenderer *mSvgRenderer;

/********************************************************************\
| Functions and properties for working with the                      |
| QAbstractItemModel parts of the StitchSet and StitchLibrary.       |
\********************************************************************/
public:
    QVariant data(int column) const;
    Stitch* child(int row);
    int childCount() const;
    int row() const;
    Stitch* parent();
private:
    Stitch* parentItem;
    QList<Stitch*> childItems;
/********************************************************************/


};

#endif //STITCH_H

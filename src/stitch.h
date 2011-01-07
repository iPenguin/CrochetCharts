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
    QString name() const { return mName; }

    void setFile(QString file) { mFile = file; }
    QString file() const { return mFile; }

    void setDescription(QString desc) { mDescription = desc; }
    QString description() const { return mDescription; }

    void setCategory(QString cat) { mCategory = cat; }
    QString category() const { return mCategory; }

    void setWrongSide(QString ws) { mWrongSide = ws; }
    QString wrongSide() const { return mWrongSide; }

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
    void appendChild(Stitch *child);

    QVariant data(int column) const;
    Stitch* child(int row);
    int childCount() const;
    int row() const;
    Stitch* parent();
    QList<Stitch*> children() { return childItems; }

protected:
    //setParent is called from appendChild() calling both is redundant.
    void setParent(Stitch *parent);
private:
    Stitch* parentItem;
    QList<Stitch*> childItems;
/********************************************************************/
};

QDataStream& operator<<(QDataStream &out, const Stitch &s);
Stitch& operator<<(Stitch &out, const Stitch &s);

QDataStream& operator>>(QDataStream &in, Stitch &s);

#endif //STITCH_H

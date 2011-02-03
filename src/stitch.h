/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCH_H
#define STITCH_H

#include <QString>
#include <QList>
#include <QPointer>

class QSvgRenderer;
class QPixmap;

class Stitch
{
public:

    enum StitchParts { Name = 0, Icon, Description, Category, WrongSide };
    
    Stitch();
    ~Stitch();

    void setName(QString n) { mName = n; }
    QString name() const { return mName; }

    void setFile(QString f);
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
    QPointer<QSvgRenderer> mSvgRenderer;
};

QDataStream& operator<<(QDataStream &out, const Stitch &s);
Stitch& operator<<(Stitch &out, const Stitch &s);

QDataStream& operator>>(QDataStream &in, Stitch &s);

#endif //STITCH_H

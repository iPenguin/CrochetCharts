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
    friend class StitchSet;
public:

    enum StitchParts { Name = 0, Icon, Description, Category, WrongSide };
    
    Stitch();
    ~Stitch();

    QString name() const { return mName; }
    QString file() const { return mFile; }
    QString description() const { return mDescription; }
    QString category() const { return mCategory; }
    QString wrongSide() const { return mWrongSide; }

    bool isSvg();

    QPixmap* renderPixmap();
    QSvgRenderer* renderSvg(bool altativeRenderer = false);

protected:
    void setName(QString n) { mName = n; }
    void setFile(QString f);
    void setDescription(QString desc) { mDescription = desc; }
    void setCategory(QString cat) { mCategory = cat; }
    void setWrongSide(QString ws) { mWrongSide = ws; }
    
private:
    void setupSvgFiles();
    
    QString mName;
    QString mFile;
    QString mDescription;
    QString mCategory;
    QString mWrongSide;

    QPixmap *mPixmap;
    QPointer<QSvgRenderer> mSvgRenderer;
    QPointer<QSvgRenderer> mSvgRendererAlt;
};

#endif //STITCH_H

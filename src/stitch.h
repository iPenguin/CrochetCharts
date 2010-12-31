#ifndef STITCH_H
#define STITCH_H

#include <QObject>
class QSvgRenderer;
class QPixmap;

class Stitch : public QObject
{
    Q_OBJECT
public:
    Stitch();
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

};

#endif //STITCH_H

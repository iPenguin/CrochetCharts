#ifndef STITCH_H
#define STITCH_H

#include <QObject>

class Stitch : public QObject
{
    Q_OBJECT
public:
    Stitch();
    
    void setName(QString name) { mName = name; }
    QString name() { return mName; }

    void setFile(QString file) { mFile = file; }
    QString file() { return mFile; }

    void setDescription(QString desc) { mDescription = desc; }
    QString description() { return mDescription; }

    void setCategory(QString cat) { mCategory = cat; }
    QString category() { return mCategory; }

    void setWrongSide(QString ws) { mWrongSide = ws; }
    QString wrongSide() { return mWrongSide; }


    void setRotation(double rotation) { mRotation = rotation; }
    double rotation() { return mRotation; }

    void setAngle(double angle) { mAngle = angle; }
    double angle() { return mRotation; }

private:

    QString mName;
    QString mFile;
    QString mDescription;
    QString mCategory;
    QString mWrongSide;

    double mRotation;
    double mAngle;

};

#endif //STITCH_H

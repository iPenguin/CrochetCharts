#ifndef STITCHSET_H
#define STITCHSET_H

#include <QList>
#include <QStandardItemModel>
#include "stitch.h"

class QDomElement;

class StitchSet : public QStandardItemModel
{
    Q_OBJECT
public:
    static StitchSet* inst();

    void loadXmlStitchSet(QString fileName);

    const QString name() { return mName; }
    void setName(QString name) { mName = name; }

    const QString author() { return mAuthor; }
    void setAuthor(QString author) { mAuthor = author; }

    const QString email() { return mEmail; }
    void setEmail(QString email) { mEmail = email; }

    const QString org() { return mOrg; }
    void setOrg(QString org) { mOrg = org; }

    const QString url() { return mUrl; }
    void setUrl(QString url) { mUrl = url; }

    Stitch* findStitch(QString name);

private:
    StitchSet();
    static StitchSet *mInstance;

    void loadXmlStitch(QDomElement e);

    //TODO: figure out which stitches should be saved to which file...
    void saveXmlStitches(QString fileName);

    //TODO: make the set class
    QList<Stitch *> mStitches;

    QString mName,
            mAuthor,
            mEmail,
            mOrg,
            mUrl;
};

#endif //STITCHSET_H

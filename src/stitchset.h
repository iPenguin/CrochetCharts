#ifndef STITCHSET_H
#define STITCHSET_H

#include <QList>
#include <QStandardItemModel>
#include "stitch.h"

class QDomElement;
class QXmlStreamWriter;

class StitchSet : public QStandardItemModel
{
    Q_OBJECT
public:
    StitchSet();
    ~StitchSet();

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

    void loadXmlStitch(QDomElement e);

    //TODO: figure out which stitches should be saved to which file...
    void saveXmlStitchSet(QString fileName);
    void saveXmlStitches(QXmlStreamWriter stream);

    //TODO: make the set class
    QList<Stitch *> mStitches;

    QString mName,
            mAuthor,
            mEmail,
            mOrg,
            mUrl;
};

#endif //STITCHSET_H

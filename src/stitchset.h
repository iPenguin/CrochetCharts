#ifndef STITCHSET_H
#define STITCHSET_H

#include <QList>
#include <QAbstractItemModel>
#include "stitch.h"

class QDomElement;

class StitchSet : public QAbstractItemModel
{
    Q_OBJECT
    friend class StitchCollection;
    friend class StitchLibraryUi;
public:
    StitchSet(QObject *parent = 0);
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

    bool hasStitch(QString name);
    void addStitch(Stitch *s);

    Stitch* stitch(QModelIndex index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
    QList<Stitch *> stitches() { return mStitches; }

private:
    Stitch *rootItem;

    void loadXmlStitch(QDomElement e);

    //TODO: convert to model functions where possible: this->invisibleRootItem();?
    QList<Stitch *> mStitches;

    QString mName,
            mAuthor,
            mEmail,
            mOrg,
            mUrl;
};

#endif //STITCHSET_H

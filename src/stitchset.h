/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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
    StitchSet(QString name, QObject *parent = 0);
    ~StitchSet();

    void loadXmlStitchSet(QString fileName);

    const QString name() const { return mName; }
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
    void addStitch(Stitch *s, Stitch *parent = 0);

    int stitchCount();

protected:
    QList<Stitch *> stitches() { return mStitches; }

private:
    void loadXmlStitch(QDomElement e);

    //FIXME: convert to model functions where possible: this->invisibleRootItem();?
    QList<Stitch *> mStitches;

    QString mName,
            mAuthor,
            mEmail,
            mOrg,
            mUrl;

/***************************************************************\
|QAbstractItemModel
\***************************************************************/
public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex rootItemIndex() const { return createIndex(0, 0, rootItem); }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
    Stitch *rootItem;
/***************************************************************/
};

QDataStream& operator<<(QDataStream &out, const StitchSet &set);
QDataStream& operator>>(QDataStream &in, StitchSet &set);

#endif //STITCHSET_H

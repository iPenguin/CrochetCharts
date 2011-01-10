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
    ~StitchSet();

    void loadXmlStitchSet(QString fileName);

    const QString name() const { return mName; }
    void setName(QString n) { mName = n; }

    const QString author() { return mAuthor; }
    void setAuthor(QString a) { mAuthor = a; }

    const QString email() { return mEmail; }
    void setEmail(QString e) { mEmail = e; }

    const QString org() { return mOrg; }
    void setOrg(QString o) { mOrg = o; }

    const QString url() { return mUrl; }
    void setUrl(QString u) { mUrl = u; }

    Stitch* findStitch(QString name);

    bool hasStitch(QString name);
    void addStitch(Stitch *s);

    int stitchCount() const;

protected:
    QList<Stitch *> stitches() { return mStitches; }

private:
    void loadXmlStitch(QDomElement e);

    QList<Stitch*> mStitches;
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

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:

/***************************************************************/
};

QDataStream& operator<<(QDataStream &out, const StitchSet &set);
QDataStream& operator>>(QDataStream &in, StitchSet &set);

#endif //STITCHSET_H

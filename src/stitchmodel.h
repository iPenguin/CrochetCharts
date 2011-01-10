/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCHMODEL_H
#define STITCHMODEL_H

#include <QAbstractItemModel>

#include "stitch.h"

class QDomElement;

class StitchModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    StitchModel(QObject *parent = 0);
    ~StitchModel();

    void addStitch(Stitch *s);
    
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void loadXmlStitchSet(QString fileName);
    void loadXmlStitch(QDomElement element);
    
private:

    QList<Stitch*> mStitches;
};
#endif //STITCHMODEL_H
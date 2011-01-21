/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETDATAMODEL_H
#define CROCHETDATAMODEL_H

#include <QAbstractItemModel>

class Cell;

class CrochetDataModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CrochetDataModel(QObject *parent = 0);

    QModelIndex	parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount(int row);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

    void setRowCount(int rows);
    void setColumnCount(int row, int columns);
    //void setColumnCountAll(int columns);

    void appendColumn(int row);
    bool insertColumns(int column, int count, const QModelIndex &parent);
    void removeColumn(int row, int column = -1);

    void addRow(int columns);
    void appendRow(QList<Cell*> row);
    bool insertRows(int row, int count, const QModelIndex &parent);

    void setCell(int row, int column, Cell *c);
    Cell* cell(int row, int column);

signals:

public slots:


private:

    QList<QList<Cell *> > mGrid;
};

#endif // CROCHETDATAMODEL_H

#ifndef CROCHETDATAMODEL_H
#define CROCHETDATAMODEL_H

#include <QAbstractItemModel>

class Cell;
class CrochetCell;

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

    void setRows(int rows);
    void setInitialColumns(int columns);

    void addColumn(int row, int column = -1);
    void removeColumn(int row, int columnt = -1);

    void setCell(int row, int column, Cell *c);
    Cell* cell(int row, int column);

signals:

public slots:


private:

    QList<QList<Cell *> > mRows;
};

#endif // CROCHETDATAMODEL_H

/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetdatamodel.h"

#include "cell.h"

CrochetDataModel::CrochetDataModel(QObject *parent) :
    QAbstractItemModel(parent)
{

}

void CrochetDataModel::setCell(int row, int column, Cell *c)
{

    if(row >= mRows.count())
        return;
    if(column >= mRows[row].count())
        return;

    mRows[row][column] = c;
}

Cell* CrochetDataModel::cell(int row, int column)
{

    if(row >= mRows.count())
        return 0;
    if(column >= mRows[row].count())
        return 0;

    return mRows[row].at(column);
}

QModelIndex CrochetDataModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->createIndex(row, column);
}

QModelIndex CrochetDataModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int CrochetDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mRows.count();
}

int CrochetDataModel::columnCount (const QModelIndex &parent) const
{
    int row = parent.row();
    if (mRows.count() <= row)
        return -1;

    return mRows[row].count();
}

int CrochetDataModel::columnCount(int row)
{
    return columnCount(this->index(row, 0));
}

QVariant CrochetDataModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    return QVariant();
}

void CrochetDataModel::setRowCount(int rows)
{
    if(rows <= 0)
        return;

    //TODO: allow code to truncate the row count here? or rename the function?
    if(rows < mRows.count())
        return;

    for(int i = 0; i < rows; ++i) {
        QList<Cell *> row;
        mRows.append(row);
    }
}

//FIXME: pass in the information about what cells to set as defaults.
void CrochetDataModel::setColumnCount(int row, int columns)
{
    if(columns <= 0)
        return;
    if(mRows.count() <= row)
        return;

    for(int i = 0; i < columns; ++i) {
        Cell *c = new Cell(0);
        mRows[row].append(c);
    }
}

void CrochetDataModel::appendRow()
{
    QList<Cell *> row;
    mRows.append(row);
}

void CrochetDataModel::appendColumn(int row)
{
    if(row >= mRows.count())
        return;

    //FIXME: finish function...
    //mRows[row].insert(column, new Cell());

}

void CrochetDataModel::removeColumn(int row, int column)
{

    if(row >= mRows.count())
        return;
    if(column >= mRows[row].count())
        return;

//FIXME: finish function...
}

bool CrochetDataModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return true;
}

bool CrochetDataModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_UNUSED(column);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return true;
}

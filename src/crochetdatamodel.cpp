#include "crochetdatamodel.h"

#include "crochetcell.h"

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

QModelIndex CrochetDataModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
    return this->index(row, column);
}

QModelIndex CrochetDataModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex();
}

int CrochetDataModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mRows.count();
}

int CrochetDataModel::columnCount (const QModelIndex &parent) const
{
    int row = parent.row();
    return mRows[row].count();
}

QVariant CrochetDataModel::data(const QModelIndex &/*index*/, int /*role*/) const
{
    return QVariant();
}

int CrochetDataModel::columnCount(int row)
{
    if (mRows.count() <= row)
        return -1;

    return mRows.at(row).count();
}

void CrochetDataModel::setRows(int /*rows*/)
{


}

void CrochetDataModel::setInitialColumns(int /*columns*/)
{

}

void CrochetDataModel::addColumn(int row, int column)
{
    if(row >= mRows.count())
        return;
    if(column >= mRows[row].count())
        return;

    if(column == -1)
        column = mRows.count();

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

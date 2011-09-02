#include "rowsandcolumns.h"
#include "ui_rowsandcolumns.h"

RowsAndColumns::RowsAndColumns(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RowsAndColumns)
{
    ui->setupUi(this);
}

RowsAndColumns::~RowsAndColumns()
{
    delete ui;
}

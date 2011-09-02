#include "rowsdock.h"
#include "ui_rowsdock.h"

RowsDock::RowsDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RowsDock)
{
    ui->setupUi(this);
}

RowsDock::~RowsDock()
{
    delete ui;
}

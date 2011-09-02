#include "rowsdock.h"
#include "ui_rowsdock.h"

RowsDock::RowsDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RowsDock)
{
    ui->setupUi(this);

    horizonalGroup.addButton(ui->alignLeft);
    horizonalGroup.addButton(ui->alignCenterH);
    horizonalGroup.addButton(ui->alignRight);
    ui->alignLeft->setChecked(true);
    
    verticalGroup.addButton(ui->alignTop);
    verticalGroup.addButton(ui->alignCenterV);
    verticalGroup.addButton(ui->alignBottom);
    ui->alignTop->setChecked(true);
}

RowsDock::~RowsDock()
{
    delete ui;
}

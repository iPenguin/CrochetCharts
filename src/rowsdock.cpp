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

    connect(ui->arrangeBttn, SIGNAL(clicked(bool)), SLOT(generateArrangement()));
    connect(ui->createGridBttn, SIGNAL(clicked(bool)), SLOT(generateArrangement()));
}

RowsDock::~RowsDock()
{
    delete ui;
}

void RowsDock::generateArrangement()
{
    QSize grid, alignment, spacing;

    grid = QSize(ui->rows->text().toInt(), ui->stitches->text().toInt());

    if(horizonalGroup.checkedButton() == ui->alignLeft) {
        alignment.setWidth(1);
    } else if(horizonalGroup.checkedButton() == ui->alignCenterH) {
        alignment.setWidth(2);
    } else if(horizonalGroup.checkedButton() == ui->alignRight) {
        alignment.setWidth(3);
    }

    if(verticalGroup.checkedButton() == ui->alignTop) {
        alignment.setHeight(1);
    } else if(horizonalGroup.checkedButton() == ui->alignCenterV) {
        alignment.setHeight(2);
    } else if(horizonalGroup.checkedButton() == ui->alignBottom) {
        alignment.setHeight(3);
    }

    spacing = QSize(ui->rowSpacing->text().toInt(), ui->stitchSpacing->text().toInt());

    bool useSelection = true;

    if(sender() == ui->createGridBttn)
        useSelection = false;
    
    emit arrangeGrid(grid, alignment, spacing, useSelection);
}

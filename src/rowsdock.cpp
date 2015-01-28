/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#include "rowsdock.h"
#include "ui_rowsdock.h"

RowsDock::RowsDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RowsDock)
{
    ui->setupUi(this);
    setVisible(false);
    setFloating(true);
    setObjectName("rowsDock");

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

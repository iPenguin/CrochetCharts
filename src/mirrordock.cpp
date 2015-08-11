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
#include "mirrordock.h"
#include "ui_mirrordock.h"

MirrorDock::MirrorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MirrorDock)
{
    ui->setupUi(this);
    setVisible(false);
    setFloating(true);
    setObjectName("mirrorDock");

    ui->rotateCustomWidgets->hide();

    connect(ui->mirrorLeft, SIGNAL(clicked()), SLOT(genMirror()));
    connect(ui->mirrorRight, SIGNAL(clicked()), SLOT(genMirror()));
    connect(ui->mirrorUp, SIGNAL(clicked()), SLOT(genMirror()));
    connect(ui->mirrorDown, SIGNAL(clicked()), SLOT(genMirror()));
	
	connect(ui->copyLeft, SIGNAL(clicked()), SLOT(genCopy()));
	connect(ui->copyRight, SIGNAL(clicked()), SLOT(genCopy()));
	connect(ui->copyUp, SIGNAL(clicked()), SLOT(genCopy()));
	connect(ui->copyDown, SIGNAL(clicked()), SLOT(genCopy()));

    connect(ui->rotate90, SIGNAL(clicked()), SLOT(genRotate()));
    connect(ui->rotate180, SIGNAL(clicked()), SLOT(genRotate()));
    connect(ui->rotate270, SIGNAL(clicked()), SLOT(genRotate()));

    connect(ui->rotateCustom, SIGNAL(clicked()), SLOT(rotateCustom()));

    connect(ui->rotateBttn, SIGNAL(clicked()), SLOT(genRotate()));
}

MirrorDock::~MirrorDock()
{
    delete ui;
}

void MirrorDock::genCopy()
{
    int direction = 1;
    if(sender() == ui->copyLeft)
        direction = 1;
    else if(sender() == ui->copyRight)
        direction = 2;
    else if(sender() == ui->copyUp)
        direction = 3;
    else if(sender() == ui->copyDown)
        direction = 4;

    emit copy(direction);
}

void MirrorDock::genMirror()
{
    int direction = 1;
    if(sender() == ui->mirrorLeft)
        direction = 1;
    else if(sender() == ui->mirrorRight)
        direction = 2;
    else if(sender() == ui->mirrorUp)
        direction = 3;
    else if(sender() == ui->mirrorDown)
        direction = 4;

    emit mirror(direction);
}

void MirrorDock::genRotate()
{
    qreal degrees = 0;

    if(sender() == ui->rotate90)
        degrees = 90;
    else if(sender() == ui->rotate180)
        degrees = 180;
    else if(sender() == ui->rotate270)
        degrees = 270;
    else if(sender() == ui->rotateBttn)
        degrees = ui->degrees->value();

    emit rotate(degrees);
}

void MirrorDock::rotateCustom()
{
    if(ui->rotateCustomWidgets->isVisible()) {
        ui->rotateCustomWidgets->setVisible(false);
    } else {
        ui->rotateCustomWidgets->show();
    }
}

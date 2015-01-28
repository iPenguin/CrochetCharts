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
#include "stitchreplacerui.h"
#include "ui_stitchreplacerui.h"

#include "stitch.h"
#include <QPushButton>
#include "stitchlibrary.h"

StitchReplacerUi::StitchReplacerUi(QString stitch, QList< QString > patternStitches, QWidget* parent) :
    QDialog(parent),
    mOriginalStitchList(patternStitches),
    ui(new Ui::StitchReplacerUi)
{
    ui->setupUi(this);

    if(mOriginalStitchList.isEmpty()) {
        QPushButton *ok = ui->buttonBox->button(QDialogButtonBox::Ok);
        ok->setEnabled(false);
    }

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));

    connect(ui->originalStitch, SIGNAL(currentIndexChanged(QString)), SLOT(updateStitches(QString)));
    connect(ui->replacementStitch, SIGNAL(currentIndexChanged(QString)), SLOT(updateStitches(QString)));

    populateStitchLists();

    if(stitch.isEmpty())
        return;

    ui->originalStitch->setCurrentIndex(ui->originalStitch->findText(stitch));
}

StitchReplacerUi::~StitchReplacerUi()
{
    delete ui;
}

void StitchReplacerUi::updateStitches(QString newStitch)
{
    if(sender() == ui->originalStitch) {
        original = newStitch;
    } else { //replacement stitch
        replacement = newStitch;
    }
}

void StitchReplacerUi::populateStitchLists()
{

    foreach(QString stitch, mOriginalStitchList) {
        Stitch *s = StitchLibrary::inst()->findStitch(stitch);

        ui->originalStitch->addItem(QIcon(s->file()), stitch);
    }

    foreach(QString stitch, StitchLibrary::inst()->stitchList()) {
        Stitch *s = StitchLibrary::inst()->findStitch(stitch);

        ui->replacementStitch->addItem(QIcon(s->file()), stitch);
    }
}

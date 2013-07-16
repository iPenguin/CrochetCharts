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

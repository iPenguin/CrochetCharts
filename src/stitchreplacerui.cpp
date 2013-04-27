#include "stitchreplacerui.h"
#include "ui_stitchreplacerui.h"

#include "settings.h"
#include "stitch.h"

#include "stitchlibrary.h"

StitchReplacerUi::StitchReplacerUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StitchReplacerUi)
{

    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));

    connect(ui->originalStitch, SIGNAL(currentIndexChanged(QString)), SLOT(updateStitches(QString)));
    connect(ui->replacementStitch, SIGNAL(currentIndexChanged(QString)), SLOT(updateStitches(QString)));

    populateStitchLists();

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

    QString name = Settings::inst()->value("defaultStitch").toString();

    foreach(QString stitch, StitchLibrary::inst()->stitchList()) {
        Stitch *s = StitchLibrary::inst()->findStitch(stitch);

        ui->originalStitch->addItem(QIcon(s->file()), stitch);
    }

    ui->originalStitch->setCurrentIndex(ui->originalStitch->findText(name));

    foreach(QString stitch, StitchLibrary::inst()->stitchList()) {
        Stitch *s = StitchLibrary::inst()->findStitch(stitch);

        ui->replacementStitch->addItem(QIcon(s->file()), stitch);
    }
}

#include "stitchlibraryui.h"

#include "stitchcollection.h"
#include "stitchset.h"

StitchLibraryUi::StitchLibraryUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::StitchLibraryDialog)
{
    ui->setupUi(this);

    foreach(StitchSet *set, StitchCollection::inst()->stitchSets()) {
        ui->stitchSource->addItem(set->name());
    }
    
    ui->listView->setModel(StitchCollection::inst()->masterStitchSet());
}

StitchLibraryUi::~StitchLibraryUi()
{
    delete ui;
}

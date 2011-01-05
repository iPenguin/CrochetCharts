#include "stitchlibraryui.h"

#include "stitchcollection.h"
#include "stitchset.h"

StitchLibraryUi::StitchLibraryUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::StitchLibraryDialog)
{
    ui->setupUi(this);

    ui->listView->setModel(StitchCollection::inst()->stitchSet());
}

StitchLibraryUi::~StitchLibraryUi()
{
    delete ui;
}
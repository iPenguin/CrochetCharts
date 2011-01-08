/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibraryui.h"

#include "stitchcollection.h"
#include "stitchset.h"
#include "stitchlibrarydelegate.h"

#include <QDebug>

StitchLibraryUi::StitchLibraryUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::StitchLibraryDialog)
{
    ui->setupUi(this);

    StitchCollection::inst()->populateComboBox(ui->stitchSource);
    StitchSet *master = StitchCollection::inst()->masterStitchSet();
    ui->listView->setModel(master);
qDebug() << "create delegate for library";
    ui->listView->setItemDelegate(new StitchLibraryDelegate(ui->listView));

    setDialogSize();



    connect(ui->stitchSource, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(changeStitchSet(QString)));
}

StitchLibraryUi::~StitchLibraryUi()
{
    delete ui;
}

void StitchLibraryUi::setDialogSize()
{
/*
//for a table widget.
    ui->listView->resizeColumnsToContents();

    int width = 0;
    int height = ui->listView->height();
    for(int i = 0; i < 5; ++i)
        width += ui->listView->columnWidth(i);

    ui->listView->setMinimumSize(QSize(width+25, height));
*/
}

void StitchLibraryUi::changeStitchSet(QString setName)
{
    StitchSet *set = StitchCollection::inst()->findStitchSet(setName);
    if(!set)
        return;
    
    ui->listView->setModel(set);
    ui->listView->update();
}

/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibraryui.h"

#include "stitchcollection.h"
#include "stitchset.h"
#include "stitchlibrarydelegate.h"

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

StitchLibraryUi::StitchLibraryUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::StitchLibraryDialog)
{
    ui->setupUi(this);

    StitchCollection::inst()->populateComboBox(ui->stitchSource);
    StitchSet *master = StitchCollection::inst()->masterStitchSet();
    ui->listView->setModel(master);

    StitchLibraryDelegate *delegate = new StitchLibraryDelegate(ui->listView);
    ui->listView->setItemDelegate(delegate);

    setDialogSize();

    connect(delegate, SIGNAL(addStitchToMasterSet(int)), this, SLOT(addStitchToMasterSet(int)));
    connect(ui->stitchSource, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(changeStitchSet(QString)));

    connect(ui->resetLibrary, SIGNAL(clicked()), this, SLOT(resetLibrary()));
}

StitchLibraryUi::~StitchLibraryUi()
{
    delete ui;
}

void StitchLibraryUi::setDialogSize()
{
    //for a table widget.
    ui->listView->resizeColumnsToContents();

    int width = 0;
    int height = ui->listView->height();
    for(int i = 0; i < 5; ++i)
        width += ui->listView->columnWidth(i);

    ui->listView->setMinimumSize(QSize(width+25 + 125, height));

}

void StitchLibraryUi::changeStitchSet(QString setName)
{
    StitchSet *set = StitchCollection::inst()->findStitchSet(setName);
    if(!set)
        return;
    
    ui->listView->setModel(set);
    ui->listView->update();
}

void StitchLibraryUi::addStitchToMasterSet(int row)
{
    StitchSet *set = static_cast<StitchSet*>(ui->listView->model());
    Stitch *s = static_cast<Stitch*>(set->index(row, 0).internalPointer());

    StitchSet *master = StitchCollection::inst()->masterStitchSet();

    Stitch *masterStitch;
    
    if(master->hasStitch(s->name())) {
        QMessageBox msgbox;
        msgbox.setText(tr("A stitch with this name already exists in your set."));
        msgbox.setInformativeText(tr("Would you like to replace it with this one?"));
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        
        if(msgbox.exec() != QMessageBox::Yes)
            return;
        
        masterStitch = master->findStitch(s->name());
    } else {
        masterStitch = new Stitch();
        master->addStitch(masterStitch);
    }

    *masterStitch << *s;

    //TODO: make some other indication that the stitch has 'transfered' as switching
    //sets is annoying and burdensome.
    ui->stitchSource->setCurrentIndex(ui->stitchSource->findText(master->name()));
}

void StitchLibraryUi::resetLibrary()
{
    QMessageBox msgbox;
    msgbox.setText(tr("If you reset the stitch set you will loose any customizations you have made to the Master Stitch Set."));
    msgbox.setInformativeText(tr("Are you sure you want to continue?"));
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if(msgbox.exec() != QMessageBox::Yes)
        return;
    
    StitchSet *master = StitchCollection::inst()->masterStitchSet();
    StitchSet *builtIn = StitchCollection::inst()->builtIn();

    ui->listView->clearSelection();
    
    master->clearStitches();
    master->cloneStitchSet(builtIn);
    
}

void StitchLibraryUi::addStitchToSet()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New Stitch"), tr("Stitch name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        //TODO: check if the stitch already exists. if not add it.

    }
}

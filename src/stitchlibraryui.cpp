/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibraryui.h"

#include "stitchcollection.h"
#include "stitchset.h"
#include "stitchlibrarydelegate.h"

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>

#include "stitchiconui.h"

//TODO: add a hash based on the sn to the stitch set to grant write access to a stitch set.
//TODO: convert the pushbutton to checkbox for add stitch and add a button at the bottom.
//TODO: add a checkbox in the 'add stitch' header that allows to select all stitches.

StitchLibraryUi::StitchLibraryUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::StitchLibraryDialog)
{
    ui->setupUi(this);

    ui->stitchSource->addItems(StitchCollection::inst()->stitchSetList());
    
    StitchSet *master = StitchCollection::inst()->masterStitchSet();
    ui->listView->setModel(master);

    StitchLibraryDelegate *delegate = new StitchLibraryDelegate(ui->listView);
    ui->listView->setItemDelegate(delegate);

    setDialogSize();

    ui->propertiesBox->setVisible(false);
    connect(ui->moreBttn, SIGNAL(clicked(bool)), this, SLOT(hideProperties()));
    connect(ui->printSet, SIGNAL(clicked()), this, SLOT(printStitchSet()));
    connect(ui->addStitch, SIGNAL(clicked()), this, SLOT(addStitchToSet()));

    connect(ui->setName, SIGNAL(editingFinished()), this, SLOT(updateStitchSetProperties()));
    connect(ui->author,  SIGNAL(editingFinished()), this, SLOT(updateStitchSetProperties()));
    connect(ui->email,   SIGNAL(editingFinished()), this, SLOT(updateStitchSetProperties()));
    connect(ui->org,     SIGNAL(editingFinished()), this, SLOT(updateStitchSetProperties()));
    connect(ui->url,     SIGNAL(editingFinished()), this, SLOT(updateStitchSetProperties()));
    
    setupPropertiesBox();
    
    connect(delegate, SIGNAL(addStitchToMasterSet(int)), this, SLOT(addStitchToMasterSet(int)));
    connect(ui->stitchSource, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(changeStitchSet(QString)));

    connect(ui->resetLibrary, SIGNAL(clicked()), this, SLOT(resetLibrary()));

    connect(ui->icons, SIGNAL(clicked()), this, SLOT(iconDialog()));
}

StitchLibraryUi::~StitchLibraryUi()
{
    delete ui;
}

void StitchLibraryUi::setDialogSize()
{
    ui->listView->resizeColumnsToContents();
    ui->listView->resizeRowsToContents();
    
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

    ui->listView->resizeColumnsToContents();
    ui->listView->resizeRowsToContents();
    setupPropertiesBox();
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
    QString text = QInputDialog::getText(this, tr("New Stitch"), tr("Stitch name:"),
                                         QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        StitchSet *set = static_cast<StitchSet*>(ui->listView->model());

        Stitch *s = set->findStitch(text.toLower());
        if(s) {
            QMessageBox::warning(this, tr("Stitch Exists"),
                tr("The stitch %1 is already in this stitch set").arg(text), QMessageBox::Ok);
            //TODO: find a way to take the user to the current cell.
        } else {
            Stitch *newS = new Stitch();
            newS->setName(text.toLower());
            set->addStitch(newS);
        }
    }
}

void StitchLibraryUi::updateStitchSetProperties()
{
    StitchSet *set = static_cast<StitchSet*>(ui->listView->model());
    set->setName(ui->setName->text());
    set->setAuthor(ui->author->text());
    set->setEmail(ui->email->text());
    set->setOrg(ui->org->text());
    set->setUrl(ui->url->text());
}

void StitchLibraryUi::setupPropertiesBox()
{
    StitchSet *set = static_cast<StitchSet*>(ui->listView->model());
    ui->setName->setText(set->name());
    ui->author->setText(set->author());
    ui->email->setText(set->email());
    ui->org->setText(set->org());
    ui->url->setText(set->url());
}

void StitchLibraryUi::hideProperties()
{
    if(ui->moreBttn->text() == tr("&More >>")) {
        ui->moreBttn->setText(tr("&Less <<"));
        ui->propertiesBox->setVisible(true);
    } else {
        ui->moreBttn->setText(tr("&More >>"));
        ui->propertiesBox->setVisible(false);
    }
}

void StitchLibraryUi::printStitchSet()
{
    //TODO: make the rect fit the page better. Also see if the page breaks work ok.
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() != QDialog::Accepted)
        return;
    
    QPainter p;
    p.begin(&printer);
    ui->listView->render(&p);
    p.end();
}

void StitchLibraryUi::iconDialog()
{
    StitchIconUi d(this);

    d.exec();
    
}

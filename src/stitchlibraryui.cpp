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

#include "settings.h"

#include "stitchiconui.h"
#include <QFileDialog>

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
    connect(ui->moreBttn, SIGNAL(clicked()), this, SLOT(hideProperties()));
    connect(ui->printSet, SIGNAL(clicked()), this, SLOT(printStitchSet()));
    
    connect(ui->addStitch, SIGNAL(clicked()), this, SLOT(addStitch()));
    connect(ui->removeStitch, SIGNAL(clicked()), this, SLOT(removeStitch()));

    connect(ui->createSet, SIGNAL(clicked()), this, SLOT(createSet()));
    connect(ui->removeSet, SIGNAL(clicked()), this, SLOT(removeSet()));
    
    connect(ui->importSet, SIGNAL(clicked()), this, SLOT(importSet()));
    connect(ui->exportSet, SIGNAL(clicked()), this, SLOT(exportSet()));

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

    setButtonStates(master);
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

    StitchSet *curSet = static_cast<StitchSet*>(ui->listView->model());
    curSet->clearSelection();
    
    ui->listView->setModel(set);
    ui->listView->update();

    ui->listView->resizeColumnsToContents();
    ui->listView->resizeRowsToContents();
    setupPropertiesBox();

    setButtonStates(set);
}

void StitchLibraryUi::setButtonStates(StitchSet *set)
{
    bool state = (set != StitchCollection::inst()->masterStitchSet());
    
    ui->removeSet->setEnabled(state);
    ui->addStitch->setEnabled(state);
    ui->removeStitch->setEnabled(state);
    ui->moreBttn->setEnabled(state);
    ui->propertiesBox->setEnabled(state);
}

void StitchLibraryUi::addStitchToMasterSet(int row)
{
    StitchSet *set = static_cast<StitchSet*>(ui->listView->model());
    Stitch *s = static_cast<Stitch*>(set->index(row, 0).internalPointer());

    StitchSet *master = StitchCollection::inst()->masterStitchSet();
    
    if(master->hasStitch(s->name()) && s != master->findStitch(s->name())) {
        QMessageBox msgbox;
        msgbox.setText(tr("A stitch with this name already exists in your set."));
        msgbox.setInformativeText(tr("Would you like to replace it with this one?"));
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        //TODO: clean up dialog box.
        if(msgbox.exec() != QMessageBox::Yes)
            return;
    }

    StitchCollection::inst()->addStitchToMasterSet(set, s);

    //TODO: make some other indication that the stitch has 'transfered' as switching 
    //between active sets is annoying and burdensome.
    ui->stitchSource->setCurrentIndex(ui->stitchSource->findText(master->name()));
}

void StitchLibraryUi::resetLibrary()
{
    QMessageBox msgbox;
    msgbox.setText(tr("If you reset the stitch set you will loose any customizations you have made to the Master Stitch Set."));
    msgbox.setInformativeText(tr("Are you sure you want to continue?"));
    msgbox.setIcon(QMessageBox::Question);
    QPushButton *reset = msgbox.addButton(tr("Yes, reset the library"), QMessageBox::AcceptRole);
    /*QPushButton *cancel =*/ msgbox.addButton(tr("No, keep the library as it is"), QMessageBox::RejectRole);

    msgbox.exec();
    
    if(msgbox.clickedButton() != reset)
        return;

    ui->listView->clearSelection();

    StitchCollection::inst()->resetMasterStitchSet();

    ui->listView->resizeColumnsToContents();
    ui->listView->resizeRowsToContents();
}

void StitchLibraryUi::addStitch()
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
            set->createStitch(text.toLower());
            ui->listView->resizeRowsToContents();
        }
    }
}

void StitchLibraryUi::removeStitch()
{
    //FIXME: check if the stitch is in the master set, if so remove it or we'll crash!
    //TODO: warn that we'll also be removing this stitch from the master set.
    StitchSet *set = static_cast<StitchSet*>(ui->listView->model());

    for(int i = 0; i < set->stitchCount(); ++i) {
        bool selected = set->data(set->index(i, 5), Qt::EditRole).toBool();
        if(selected) {
            qDebug() << set->data(set->index(i, 0), Qt::EditRole).toString();
            set->removeStitch(set->data(set->index(i, 0), Qt::EditRole).toString());
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
    //FIXME: this only prints the visible part of the widget.
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

void StitchLibraryUi::createSet()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New Stitch Set"), tr("Stitch set name:"),
                                         QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        StitchCollection::inst()->createStitchSet(text);

        //switch to the new set.
        updateSourceDropDown(text);
    }
}

void StitchLibraryUi::removeSet()
{
    QMessageBox msgbox(this);
    msgbox.setText(tr("This will remove the set and it's associated files."));
    msgbox.setInformativeText(tr("Are you sure you want to remove the set?"));
    msgbox.setIcon(QMessageBox::Question);
    QPushButton *remove = msgbox.addButton(tr("Yes, remove the set and it's files"),QMessageBox::AcceptRole);
    /*QPushButton *keep =*/ msgbox.addButton(tr("Cancel"), QMessageBox::RejectRole);

    msgbox.exec();
    if(msgbox.clickedButton() != remove)
        return;
    
    QString setName = ui->stitchSource->currentText();
    
    StitchCollection::inst()->removeSet(setName);

    //switch to the master set.
    updateSourceDropDown(StitchCollection::inst()->masterStitchSet()->name());
}

void StitchLibraryUi::exportSet()
{   
    StitchSet *set = StitchCollection::inst()->findStitchSet(ui->stitchSource->currentText());

    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return;
    }
    
    QString fileLoc = Settings::inst()->value("fileLocation").toString();
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Stitch Set"), fileLoc, tr("Stitch Set (*.set)"));
    
    if(fileName.isEmpty())
        return;
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    set->saveDataFile(fileName);
    QApplication::restoreOverrideCursor();
}

void StitchLibraryUi::importSet()
{
    QString fileLoc = Settings::inst()->value("fileLocation").toString();
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Stitch Set"), fileLoc, tr("Stitch Set (*.set)"));
    
    if(fileName.isEmpty() || fileName.isNull())
        return;
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    StitchSet *set = 0;
    set = StitchCollection::inst()->addStitchSet(fileName);

    if(set) {
        set->saveXmlFile();
        updateSourceDropDown(set->name());
    }

    QApplication::restoreOverrideCursor();
}

void StitchLibraryUi::updateSourceDropDown(QString setName)
{
    if(setName.isEmpty())
        setName = ui->stitchSource->currentText();
    
    ui->stitchSource->clear();
    ui->stitchSource->addItems(StitchCollection::inst()->stitchSetList());
    int index = ui->stitchSource->findText(setName, Qt::MatchExactly);
    ui->stitchSource->setCurrentIndex(index);
}

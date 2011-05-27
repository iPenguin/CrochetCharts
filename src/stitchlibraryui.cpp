/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibraryui.h"

#include "stitchlibrary.h"
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
//TODO: add a check box in the 'add stitch' header that allows to select all stitches.
//TODO: make the msgboxes reusbale functions.

StitchLibraryUi::StitchLibraryUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::StitchLibraryDialog)
{
    ui->setupUi(this);

    ui->stitchSource->addItems(StitchLibrary::inst()->stitchSetList());
    
    StitchSet *master = StitchLibrary::inst()->masterStitchSet();
    ui->listView->setModel(master);

    StitchLibraryDelegate *delegate = new StitchLibraryDelegate(ui->listView);
    ui->listView->setItemDelegate(delegate);

    setDialogSize();

    ui->propertiesBox->setVisible(false);
    connect(ui->moreBttn, SIGNAL(clicked()), this, SLOT(hideProperties()));
    connect(ui->printSet, SIGNAL(clicked()), this, SLOT(printStitchSet()));
    
    connect(ui->addStitch, SIGNAL(clicked()), this, SLOT(addStitch()));
    connect(ui->removeStitch, SIGNAL(clicked()), this, SLOT(removeStitch()));
    connect(ui->addSelected, SIGNAL(clicked()), this, SLOT(addSelected()));

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
    StitchSet *set = StitchLibrary::inst()->findStitchSet(setName);
    if(!set)
        return;

    StitchSet *curSet = qobject_cast<StitchSet*>(ui->listView->model());
    if(curSet)
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
    bool state = (set != StitchLibrary::inst()->masterStitchSet());
    
    ui->removeSet->setEnabled(state);
    ui->addStitch->setEnabled(state);
    ui->moreBttn->setEnabled(state);
    ui->propertiesBox->setEnabled(state);
    ui->addSelected->setEnabled(state);
}

void StitchLibraryUi::resetLibrary()
{
    QMessageBox msgbox;
    msgbox.setText(tr("If you reset the stitch library you will lose any changes you have made."));
    msgbox.setInformativeText(tr("Are you sure you want to continue?"));
    msgbox.setIcon(QMessageBox::Question);
    QPushButton *reset = msgbox.addButton(tr("Yes, reset the library"), QMessageBox::AcceptRole);
    /*QPushButton *cancel =*/ msgbox.addButton(tr("No, keep the library as is"), QMessageBox::RejectRole);

    msgbox.exec();
    
    if(msgbox.clickedButton() != reset)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//TODO: clean up this code. bring all the reset code together.
//FIXME: resetting a user set returns it to it's first save. keep user set's in a different folder?
    ui->listView->clearSelection();
    QString curSet = ui->stitchSource->currentText();
    
    updateSourceDropDown(StitchLibrary::inst()->masterStitchSet()->name());
    foreach(StitchSet *set, StitchLibrary::inst()->stitchSets()) {
        set->reset();
    }

    updateSourceDropDown(StitchLibrary::inst()->builtIn()->name());
    StitchLibrary::inst()->resetMasterStitchSet();

    if(curSet != StitchLibrary::inst()->builtIn()->name())
        updateSourceDropDown(curSet);
    
    ui->listView->resizeColumnsToContents();
    ui->listView->resizeRowsToContents();

    QApplication::restoreOverrideCursor();
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
    StitchSet *set = static_cast<StitchSet*>(ui->listView->model());

    for(int i = 0; i < set->stitchCount(); ++i) {
        bool selected = set->data(set->index(i, 5), Qt::EditRole).toBool();
        if(selected) {
            if(set->stitchCount() == 1) {
                //FIXME: make this a question, if yes remove set.
                QMessageBox msgbox(this);
                msgbox.setText(tr("A set must have at least one stitch."));
                msgbox.setInformativeText(tr("If you wish to remove this stich you must remove the set."));
                msgbox.setIcon(QMessageBox::Information);
                QPushButton *removeSet = msgbox.addButton(tr("Remove the stitch set"), QMessageBox::AcceptRole);
                /*QPushButton *keepStitch =*/ msgbox.addButton(tr("Keep the last stitch"), QMessageBox::RejectRole);
                
                msgbox.exec();

                if(msgbox.clickedButton() == removeSet) {
                    StitchLibrary::inst()->removeSet(set->name());
                    
                    //switch to the master set.
                    updateSourceDropDown(StitchLibrary::inst()->masterStitchSet()->name());
                }
                
                return;
            }
            QString st = set->data(set->index(i, 0), Qt::EditRole).toString();
            Stitch *s = set->findStitch(st);

            if(!set->isMasterSet && StitchLibrary::inst()->masterHasStitch(s)) {
                QMessageBox msgbox(this);
                msgbox.setText(tr("This stitch is linked to the master set. "
                                "If you remove this stitch it will be removed from the master list too."));
                msgbox.setInformativeText(tr("Are you sure you want to remove the stitch?"));
                msgbox.setIcon(QMessageBox::Question);
                QPushButton *confirm = msgbox.addButton(tr("Remove stitch from both lists"), QMessageBox::AcceptRole);
                /*QPushButton *cancel =*/ msgbox.addButton(tr("Don't remove the stitch"), QMessageBox::RejectRole);

                msgbox.exec();

                if(msgbox.clickedButton() != confirm)
                    continue;
            }
            
            StitchLibrary::inst()->removeStitchFormMasterSet(s);
            if(!set->isMasterSet) {
                set->removeStitch(st);
                delete s;
            }
        }
    }
}

void StitchLibraryUi::addSelected()
{
    StitchSet *set = static_cast<StitchSet*>(ui->listView->model());
    for(int i = 0; i < set->stitchCount(); ++i) {
        bool selected = set->data(set->index(i, 5), Qt::EditRole).toBool();
        if(selected) {
            Stitch *s = 0;
            s = static_cast<Stitch*>(set->index(i, 0).internalPointer());
            StitchSet *master = StitchLibrary::inst()->masterStitchSet();
            
            if(master->hasStitch(s->name()) && s != master->findStitch(s->name())) {
                QMessageBox msgbox;
                msgbox.setText(tr("A stitch with the name '%1' already exists in the master set.").arg(s->name()));
                msgbox.setInformativeText(tr("Would you like to replace it with this one?"));
                msgbox.setIcon(QMessageBox::Question);
                QPushButton *confirm = msgbox.addButton(tr("Replace the existing stitch"), QMessageBox::AcceptRole);
                QPushButton *no      = msgbox.addButton(tr("Keep this stitch as is"), QMessageBox::ApplyRole);
                QPushButton *cancel  = msgbox.addButton(tr("Stop adding stitches"), QMessageBox::DestructiveRole);

                msgbox.exec();
                
                if(msgbox.clickedButton() == no)
                    continue;
                else if(msgbox.clickedButton() == cancel)
                    return;
                //else if
                Q_UNUSED(confirm);
            }
            
            if(s)
                StitchLibrary::inst()->addStitchToMasterSet(set, s);
            else
                qWarning() << "Error: Couldn't get stitch from the original set.";
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

        StitchSet *found = StitchLibrary::inst()->findStitchSet(text);
        if(found) {
            QMessageBox msgbox;
            msgbox.setText(tr("A stitch set with the name '%1' already exists in your library.").arg(text));
            msgbox.setInformativeText(tr("What would you like to do?"));
            msgbox.setIcon(QMessageBox::Question);
            QPushButton *overwrite = msgbox.addButton(tr("Replace the existing set"), QMessageBox::AcceptRole);
            QPushButton *rename = msgbox.addButton(tr("Change the name of the new set"), QMessageBox::ApplyRole);
            QPushButton *cancel = msgbox.addButton(tr("Don't add the new set"), QMessageBox::RejectRole);

             msgbox.exec();
             if(msgbox.clickedButton() == overwrite) {
                StitchLibrary::inst()->removeSet(found->name());
                found = 0;
             } else if(msgbox.clickedButton() == rename) {
                 
                 //TODO: allow the user to 'cancel out' of this loop.
                 while(!ok || text.isEmpty() || text == found->name() ){
                     text = QInputDialog::getText(0, tr("New Set Name"), tr("Stitch set name:"),
                     QLineEdit::Normal, text, &ok);
                 }
             } else if (msgbox.clickedButton() == cancel) {
                 return;
            }
        }
        
        StitchSet *set = StitchLibrary::inst()->createStitchSet(text);
        //WIN32: crashes if there isn't at least one stitch in the set when you add the model to the view.
        set->createStitch("");
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
    /*QPushButton *keep =*/ msgbox.addButton(tr("No, keep the set as is"), QMessageBox::RejectRole);

    msgbox.exec();
    if(msgbox.clickedButton() != remove)
        return;
    
    QString setName = ui->stitchSource->currentText();
    
    StitchLibrary::inst()->removeSet(setName);

    //switch to the master set.
    updateSourceDropDown(StitchLibrary::inst()->masterStitchSet()->name());
}

void StitchLibraryUi::exportSet()
{   
    StitchSet *set = StitchLibrary::inst()->findStitchSet(ui->stitchSource->currentText());

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
    set = StitchLibrary::inst()->addStitchSet(fileName);

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
    ui->stitchSource->addItems(StitchLibrary::inst()->stitchSetList());
    int index = ui->stitchSource->findText(setName, Qt::MatchExactly);

    ui->stitchSource->setCurrentIndex(index);
}

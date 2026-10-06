/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
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
#include "stitchlibraryui.h"

#include "stitchlibrary.h"
#include "stitchset.h"
#include "stitchlibrarydelegate.h"

#include "debug.h"
#include <QMessageBox>
#include <QInputDialog>

#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QSortFilterProxyModel>

#include "settings.h"

#include "stitchiconui.h"
#include <QFileDialog>

//TODO: make the msgboxes reusbale functions.

StitchLibraryUi::StitchLibraryUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::StitchLibraryDialog)
{
    ui->setupUi(this);

    ui->stitchSource->addItems(StitchLibrary::inst()->stitchSetList());

    StitchSet* master = StitchLibrary::inst()->masterStitchSet();

    mProxyModel = new QSortFilterProxyModel(this);

    mProxyModel->setSourceModel(master);
    mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    ui->listView->setModel(mProxyModel);
    ui->listView->setSortingEnabled(true);

    StitchLibraryDelegate *delegate = new StitchLibraryDelegate(ui->listView);
    ui->listView->setItemDelegate(delegate);

    setDialogSize();
    ui->listView->horizontalHeader()->setSectionsClickable(true);
    ui->listView->horizontalHeader()->setSortIndicatorShown(true);

    //TODO: Wrong Side.
    ui->listView->hideColumn(4);

    ui->propertiesBox->setVisible(false);
    connect(ui->moreBttn, SIGNAL(clicked()), SLOT(hideProperties()));
    connect(ui->printSet, SIGNAL(clicked()), SLOT(printStitchSet()));

    connect(ui->addStitch, SIGNAL(clicked()), SLOT(addStitch()));
    connect(ui->removeStitch, SIGNAL(clicked()), SLOT(removeStitch()));
    connect(ui->addSelected, SIGNAL(clicked()), SLOT(addSelected()));

    connect(ui->createSet, SIGNAL(clicked()), SLOT(createSet()));
    connect(ui->removeSet, SIGNAL(clicked()), SLOT(removeSet()));

    connect(ui->importSet, SIGNAL(clicked()), SLOT(importSet()));
    connect(ui->exportSet, SIGNAL(clicked()), SLOT(exportSet()));

    connect(ui->setName, SIGNAL(editingFinished()), SLOT(updateStitchSetProperties()));
    connect(ui->author,  SIGNAL(editingFinished()), SLOT(updateStitchSetProperties()));
    connect(ui->email,   SIGNAL(editingFinished()), SLOT(updateStitchSetProperties()));
    connect(ui->org,     SIGNAL(editingFinished()), SLOT(updateStitchSetProperties()));
    connect(ui->url,     SIGNAL(editingFinished()), SLOT(updateStitchSetProperties()));

    setupPropertiesBox();
    
    connect(ui->stitchSource, SIGNAL(currentIndexChanged(QString)), SLOT(changeStitchSet(QString)));

    connect(ui->resetLibrary, SIGNAL(clicked()), SLOT(resetLibrary()));
    connect(ui->icons, SIGNAL(clicked()), SLOT(iconDialog()));

    connect(ui->listView->horizontalHeader(),
            SIGNAL(sectionClicked(int)), SLOT(updateRowSizes()));

    connect(ui->stitchFilter, SIGNAL(textChanged(QString)), SLOT(filterStitchList(QString)));
    connect(ui->clearBttn, SIGNAL(clicked()), SLOT(clearStitchFilter()));

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

    ui->listView->setMinimumSize(QSize(width+ 25 + 75, height));

}

void StitchLibraryUi::changeStitchSet(QString setName)
{

    StitchSet* set = StitchLibrary::inst()->findStitchSet(setName);
    if(!set)
        return;

    const QSortFilterProxyModel *model =  static_cast<const QSortFilterProxyModel*>(ui->listView->model());

    StitchSet* curSet = qobject_cast<StitchSet*>(model->sourceModel());
    if(curSet)
        curSet->clearSelection();

    mProxyModel->setSourceModel(set);

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
    QPushButton* reset = msgbox.addButton(tr("Yes, reset the library"), QMessageBox::AcceptRole);
    /*QPushButton* cancel =*/ msgbox.addButton(tr("No, keep the library as is"), QMessageBox::RejectRole);

    msgbox.exec();
    
    if(msgbox.clickedButton() != reset)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//TODO: clean up this code. bring all the reset code together.
//FIXME: resetting a user set returns it to it's first save. keep user set's in a different folder?
    ui->listView->clearSelection();
    QString curSet = ui->stitchSource->currentText();

    updateSourceDropDown(StitchLibrary::inst()->overlay()->name());
    StitchLibrary::inst()->resetMasterStitchSet();

    if(curSet != StitchLibrary::inst()->overlay()->name())
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
        const QSortFilterProxyModel *model =  static_cast<const QSortFilterProxyModel*>(ui->listView->model());

        StitchSet* set = static_cast<StitchSet*>(model->sourceModel());

        Stitch* s = set->findStitch(text.toLower());
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
    const QSortFilterProxyModel *model =  static_cast<const QSortFilterProxyModel*>(ui->listView->model());
    StitchSet* set = static_cast<StitchSet*>(model->sourceModel());

    for(int i = set->stitchCount() - 1; i >= 0; --i) {
        bool selected = set->data(set->index(i, 5), Qt::EditRole).toBool();

        if(!selected)
            continue;
        
        if(set->stitchCount() == 1) {
            //FIXME: make this a question, if yes remove set.
            QMessageBox msgbox(this);
            msgbox.setText(tr("A set must have at least one stitch."));
            msgbox.setInformativeText(tr("If you wish to remove this stitch you must remove the set."));
            msgbox.setIcon(QMessageBox::Information);
            QPushButton* removeSet = msgbox.addButton(tr("Remove the stitch set too"), QMessageBox::AcceptRole);
            /*QPushButton* keepStitch =*/ msgbox.addButton(tr("Keep this stitch"), QMessageBox::RejectRole);

            msgbox.exec();

            if(msgbox.clickedButton() == removeSet) {
                StitchLibrary::inst()->removeSet(set->name());

                //switch to the master set.
                updateSourceDropDown(StitchLibrary::inst()->masterStitchSet()->name());
            }

            return;
        }
        
        QString st = set->data(set->index(i, 0), Qt::EditRole).toString();
        Stitch* s = set->findStitch(st);

        if(!set->isMasterSet && StitchLibrary::inst()->masterHasStitch(s)) {
            QMessageBox msgbox(this);
            msgbox.setText(tr("This stitch is linked to the master set. "
                            "If you remove this stitch it will be removed from the master list too."));
            msgbox.setInformativeText(tr("Are you sure you want to remove the stitch?"));
            msgbox.setIcon(QMessageBox::Question);
            QPushButton* confirm = msgbox.addButton(tr("Remove stitch from both lists"), QMessageBox::AcceptRole);
            /*QPushButton* cancel =*/ msgbox.addButton(tr("Don't remove the stitch"), QMessageBox::RejectRole);

            msgbox.exec();

            if(msgbox.clickedButton() != confirm)
                continue;
        }

        StitchLibrary::inst()->removeStitchFormMasterSet(s);

        if(!set->isMasterSet) {
            set->removeStitch(st);
            s->deleteLater();
        }
    }
}

void StitchLibraryUi::addSelected()
{
    const QSortFilterProxyModel *model =  static_cast<const QSortFilterProxyModel*>(ui->listView->model());
    StitchSet* set = static_cast<StitchSet*>(model->sourceModel());
    for(int i = 0; i < set->stitchCount(); ++i) {
        bool selected = set->data(set->index(i, 5), Qt::EditRole).toBool();
        if(selected) {
            Stitch* s = 0;
            s = static_cast<Stitch*>(set->index(i, 0).internalPointer());
            StitchSet* master = StitchLibrary::inst()->masterStitchSet();
            
            if(master->hasStitch(s->name()) && s != master->findStitch(s->name())) {
                QMessageBox msgbox;
                msgbox.setText(tr("A stitch with the name '%1' already exists in the master set.").arg(s->name()));
                msgbox.setInformativeText(tr("Would you like to replace it with this one?"));
                msgbox.setIcon(QMessageBox::Question);
                QPushButton* confirm = msgbox.addButton(tr("Replace the existing stitch"), QMessageBox::AcceptRole);
                QPushButton* no      = msgbox.addButton(tr("Keep original stitch as is"), QMessageBox::ApplyRole);
                QPushButton* cancel  = msgbox.addButton(tr("Stop adding stitches"), QMessageBox::DestructiveRole);

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
    const QSortFilterProxyModel *model =  static_cast<const QSortFilterProxyModel*>(ui->listView->model());
    StitchSet* set = static_cast<StitchSet*>(model->sourceModel());
    set->setName(ui->setName->text());
    set->setAuthor(ui->author->text());
    set->setEmail(ui->email->text());
    set->setOrg(ui->org->text());
    set->setUrl(ui->url->text());
}

void StitchLibraryUi::setupPropertiesBox()
{
    const QSortFilterProxyModel *model =  static_cast<const QSortFilterProxyModel*>(ui->listView->model());
    StitchSet* set = static_cast<StitchSet*>(model->sourceModel());
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

    ui->listView->setWordWrap(true);
    ui->listView->setTextElideMode(Qt::ElideNone);
    const int rows = ui->listView->model()->rowCount();
    ui->listView->resizeRowsToContents();
    ui->listView->resizeColumnsToContents();

    // calculate the total width/height table would need without scaling
    double totalWidth = 0.0;   
    double totalHeight = 0.0;
    QList<int> widths;
    widths << 0 << 0 << 0;

    for (int r = 0; r < rows; ++r) {
        totalHeight += ui->listView->rowHeight(r);
        for (int c = 0; c < 3; ++c) {
            int temp = ui->listView->columnWidth(c);
            if(temp > widths.at(c))
                widths[c] = temp;
        }
    }

    foreach(int w, widths)
        totalWidth += w;


    //QSizeF size = QSize(totalWidth, totalHeight);
    //printer.setPaperSize(size, QPrinter::Point);
  
    QPainter p;
    p.begin(&printer);

    int rowHeight = 35;
    // paint cells
    for (int r = 0; r < rows; ++r) {

        if(rowHeight + ui->listView->rowHeight(r) > (printer.pageRect().height() - 35)) {
            printer.newPage();
            rowHeight = 35;
        }

        int startX = 0;
        for (int c = 0; c < 3; ++c) {
            QModelIndex idx = ui->listView->model()->index(r, c);
            QStyleOptionViewItem option;// = ui->listView->viewOptions();
            option.palette.setColor(QPalette::Text, QColor(Qt::black));
            option.rect.setTop(rowHeight);
            option.rect.setLeft(startX + 35);
            option.rect.setHeight(ui->listView->visualRect(idx).height());
            //FIXME: this shouldn't be hard coded but oh well
            option.rect.setWidth(widths[c] +100);
            ui->listView->itemDelegate()->paint(&p, option, idx);
            startX += widths[c];
        }

        rowHeight += ui->listView->rowHeight(r);
    }
    
    p.end();
}

void StitchLibraryUi::iconDialog()
{
    StitchIconUi d(this);

    d.exec();
    
}

void StitchLibraryUi::updateRowSizes()
{
    ui->listView->resizeRowsToContents();
}

void StitchLibraryUi::filterStitchList(QString newText)
{
    QRegExp regExp(newText, Qt::CaseInsensitive, QRegExp::FixedString);
    mProxyModel->setFilterRegExp(regExp);
    updateRowSizes();
}

void StitchLibraryUi::clearStitchFilter()
{
    ui->stitchFilter->clear();
}

void StitchLibraryUi::createSet()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New Stitch Set"), tr("Stitch set name:"),
                                         QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {

        StitchSet* found = StitchLibrary::inst()->findStitchSet(text);
        if(found) {
            QMessageBox msgbox;
            msgbox.setText(tr("A stitch set with the name '%1' already exists in your library.").arg(text));
            msgbox.setInformativeText(tr("What would you like to do?"));
            msgbox.setIcon(QMessageBox::Question);
            QPushButton* overwrite = msgbox.addButton(tr("Replace the existing set"), QMessageBox::AcceptRole);
            QPushButton* rename = msgbox.addButton(tr("Change the name of the new set"), QMessageBox::ApplyRole);
            QPushButton* cancel = msgbox.addButton(tr("Don't add the new set"), QMessageBox::RejectRole);

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
        
        StitchSet* set = StitchLibrary::inst()->createStitchSet(text);
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
    QPushButton* remove = msgbox.addButton(tr("Yes, remove the set and it's files"),QMessageBox::AcceptRole);
    /*QPushButton* keep =*/ msgbox.addButton(tr("No, keep the set as is"), QMessageBox::RejectRole);

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
    StitchSet* set = StitchLibrary::inst()->findStitchSet(ui->stitchSource->currentText());

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
    StitchSet* set = 0;
    set = addStitchSet(fileName);

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

StitchSet* StitchLibraryUi::addStitchSet(QString fileName)
{
    if(fileName.isEmpty())
        return 0;

    if(!QFileInfo(fileName).exists())
        return 0;

    QString dest = StitchLibrary::inst()->nextSetSaveFile();

    //make a set folder
    QFileInfo info(dest);

    QDir(info.path()).mkpath(info.path() + "/" + info.baseName());

    StitchSet* set = new StitchSet();

    set->loadDataFile(fileName, dest);

    StitchSet* test = 0;
    test = StitchLibrary::inst()->findStitchSet(set->name());
    if(test) {
        QMessageBox msgbox;
        msgbox.setText(tr("A stitch set with the name '%1' already exists in your library.").arg(set->name()));
        msgbox.setInformativeText(tr("What would you like to do?"));
        msgbox.setIcon(QMessageBox::Question);
        QPushButton* overwrite = msgbox.addButton(tr("Replace the existing set"), QMessageBox::AcceptRole);
        QPushButton* rename = msgbox.addButton(tr("Rename the new set"), QMessageBox::ApplyRole);
        /*QPushButton* cancel =*/ msgbox.addButton(tr("Don't add the new set"), QMessageBox::RejectRole);

        msgbox.exec();

        if(msgbox.clickedButton() == overwrite) {
            StitchLibrary::inst()->removeSet(test);

            //FIXME: this is going to cause crashes when removing sets with sts in the master list!
        } else if(msgbox.clickedButton() == rename) {
            bool ok = false;
            QString text;

            while(!ok || text.isEmpty() || text == set->name() ){
                text = QInputDialog::getText(0, tr("New Set Name"), tr("Stitch set name:"),
                                            QLineEdit::Normal, set->name(), &ok);
            }
            //TODO: allow the user to 'cancel out' of this loop.
            set->setName(text);

        } else {
            set->deleteLater();
            return 0;
        }
    }

    StitchLibrary::inst()->addStitchSet(set);
    return set;
}

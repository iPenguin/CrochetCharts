/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchiconui.h"
#include "settings.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "stitchlibrary.h"

StitchIconUi::StitchIconUi(QWidget *parent)
    : QDialog(parent), ui(new Ui::StitchIconDialog)
{
    ui->setupUi(this);

    connect(ui->addIcon, SIGNAL(clicked()), this, SLOT(addIcon()));
    connect(ui->removeIcon, SIGNAL(clicked()), this, SLOT(removeIcon()));
    connect(ui->saveIcon, SIGNAL(clicked()), this, SLOT(saveIcon()));

    connect(this, SIGNAL(newIconAdded(QString)), this, SLOT(updateIconList(QString)));

    connect(ui->iconList, SIGNAL(itemSelectionChanged()), this, SLOT(updateIconSelection()));
    loadIcons();
}

StitchIconUi::~StitchIconUi()
{
}

void StitchIconUi::loadIcons()
{
    QStringList dirs, setDir;
    QString userFolder = Settings::inst()->userSettingsFolder();


    dirs << userFolder + "icons";

    QDir dir;

    dir.setPath(userFolder);
    
    //get all set folders.
    foreach(QString folder, dir.entryList(QDir::Dirs)) {
        if(folder != "." && folder != "..")
            dirs << userFolder + folder;
    }

    //get all files from all set folders.
    foreach(QString folder, dirs) {
        dir.setPath(folder);
        foreach(QString file, dir.entryList(QDir::Files)) {
            QIcon icon = QIcon(folder + "/" + file);
            QString name = QFileInfo(file).baseName();
            QString setName = StitchLibrary::inst()->findStitchSetName(folder);
            QString description = QString("%1 (from %2)").arg(name).arg(setName);
            QListWidgetItem *item = new QListWidgetItem(icon, description, ui->iconList);
            item->setData(Qt::UserRole, QVariant(folder + "/" + file));
            item->setToolTip(folder + "/" + file);
            ui->iconList->addItem(item);
        }
    }
}

void StitchIconUi::addIcon()
{
    //TODO: make this a multi-file importer.
    QString dir = Settings::inst()->value("fileLocation").toString();
    QString source = QFileDialog::getOpenFileName(this, tr("Add Icon"), dir, "Image Files (*.svg *.svgz *.png *.gif *.jpg *.jpeg)");

    if(source.isEmpty())
        return;
    
    QString dest = Settings::inst()->userSettingsFolder() + "icons/";
    
    if(!QFileInfo(dest).exists())
        QDir(Settings::inst()->userSettingsFolder()).mkpath(dest);
    
    QFileInfo srcInfo(source);

    dest += srcInfo.fileName();
    QFileInfo destInfo(dest);
    
    if(destInfo.exists()) {
        QMessageBox msgbox(this);
        msgbox.setText(tr("A file with the name '%1' already exists.").arg(srcInfo.fileName()));
        msgbox.setInformativeText(tr("Would you like to replace the existing file?"));
        msgbox.setIcon(QMessageBox::Question);
        QPushButton *overwrite = msgbox.addButton(tr("Replace existing file"), QMessageBox::AcceptRole);
        /*QPushButton *cancel =*/ msgbox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        
        msgbox.exec();

        if (msgbox.clickedButton() != overwrite)
            return;

        QFile::remove(dest);
    }

    QFile::copy(source, dest);
    emit newIconAdded(dest);
}

void StitchIconUi::removeIcon()
{
    QList<QListWidgetItem*> items = ui->iconList->selectedItems();

    if(items.count() <= 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //TODO: do we want to allow the user to remove set icons?
    //TODO: indicate that you cannot remove built in stitches.
    foreach(QListWidgetItem *item, items) {
        QString fileName = item->data(Qt::UserRole).toString();
        if(fileName.startsWith(":/"))
            continue;
        QFile::remove(fileName);
        ui->iconList->takeItem(ui->iconList->row(item));
        ui->iconList->removeItemWidget(item);

        ui->iconList->clearSelection();
    }
    ui->iconList->update();

    QApplication::restoreOverrideCursor();
}

void StitchIconUi::saveIcon()
{
    QList<QListWidgetItem*> items = ui->iconList->selectedItems();
    
    if(items.count() <= 0)
        return;

    foreach(QListWidgetItem *item, items) {
        QString dir = Settings::inst()->value("fileLocation").toString();
        dir += "/" + QFileInfo(item->data(Qt::UserRole).toString()).fileName();
        QString dest = QFileDialog::getSaveFileName(this, tr("Save Icon"), dir, "");

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        if(dest.isEmpty())
            return;

        QFile::copy(item->data(Qt::UserRole).toString(), dest);
        QApplication::restoreOverrideCursor();
    }
    
}

void StitchIconUi::updateIconList(QString fileName)
{

    if(fileName.isEmpty())
        return;

    QListWidgetItem *item = 0;
    
    QFileInfo fileInfo(fileName);
    QList<QListWidgetItem*> foundItems = ui->iconList->findItems(fileInfo.baseName(), Qt::MatchExactly);
    if(foundItems.count() > 0) {
        foreach(QListWidgetItem *i, foundItems) {
            QString itemPath =  i->data(Qt::UserRole).toString();
            if(itemPath == fileName)
                item = i;
        }
    }

    QIcon icon = QIcon(fileName);
    
    if(!item) {
        QListWidgetItem *item = new QListWidgetItem(icon,fileInfo.baseName(),ui->iconList);
        item->setData(Qt::UserRole, QVariant(fileName));
        item->setToolTip(fileName);
        ui->iconList->addItem(item);    
    } else {
        item->setIcon(icon);
        ui->iconList->update();
    }
}

void StitchIconUi::updateIconSelection()
{
    QList<QListWidgetItem*> items = ui->iconList->selectedItems();

    bool state = (items.count() > 0);

    ui->removeIcon->setEnabled(state);
    ui->saveIcon->setEnabled(state);
}


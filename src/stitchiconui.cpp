/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchiconui.h"
#include "stitchcollection.h"
#include "settings.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

StitchIconUi::StitchIconUi(QWidget *parent)
    : QDialog(parent), ui(new Ui::StitchIconDialog)
{
    ui->setupUi(this);

    connect(ui->addIcon, SIGNAL(clicked()), this, SLOT(addIcon()));
    connect(ui->removeIcon, SIGNAL(clicked()), this, SLOT(removeIcon()));
    connect(ui->saveIcon, SIGNAL(clicked()), this, SLOT(saveIcon()));

    connect(this, SIGNAL(newIconAdded(QString)), this, SLOT(updateIconList(QString)));
    loadIcons();
}

StitchIconUi::~StitchIconUi()
{
}

void StitchIconUi::loadIcons()
{
    QStringList dirs, setDir;
    QString userFolder = Settings::inst()->userSettingsFolder();

    dirs << ":/stitches/";   
    dirs << userFolder + "icons";

    QDir dir;
    dir.setPath(userFolder + "sets/");

    //get all set folders.
    foreach(QString folder, dir.entryList(QDir::Dirs)) {
        if(folder != "." && folder != "..")
            dirs << userFolder + "sets/" + folder;
    }

    //get all files from all set folders.
    foreach(QString folder, dirs) {
        dir.setPath(folder);
        foreach(QString file, dir.entryList(QDir::Files)) {
            QIcon icon = QIcon(folder + "/" + file);
            QString name = QFileInfo(file).baseName();
            QListWidgetItem *item = new QListWidgetItem(icon, name, ui->iconList);
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
    
    QFileInfo srcInfo(source);

    dest += srcInfo.fileName();
    QFileInfo destInfo(dest);
    
    if(destInfo.exists()) {
        QMessageBox msgbox(this);
        msgbox.setText(tr("A file with the name '%1' already exists.").arg(srcInfo.fileName()));
        QPushButton *overwrite = msgbox.addButton(tr("Overwrite the file"), QMessageBox::AcceptRole);
        QPushButton *cancel = msgbox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        
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
    qDebug() << "removeIcon";
    
}

void StitchIconUi::saveIcon()
{
    qDebug() << "saveIcon";
    
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
            QString itemPath =  i->data(Qt::ToolTipRole).toString();
            if(itemPath == fileName)
                item = i;
        }
    }

    QIcon icon = QIcon(fileName);
    
    if(!item) {
        QListWidgetItem *item = new QListWidgetItem(icon,fileInfo.baseName(),ui->iconList);
        item->setToolTip(fileName);
        ui->iconList->addItem(item);    
    } else {
        item->setIcon(icon);
        ui->iconList->update();
    }
    
    
}

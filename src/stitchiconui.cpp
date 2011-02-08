/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchiconui.h"
#include "stitchcollection.h"
#include "settings.h"
#include <QDir>

StitchIconUi::StitchIconUi(QWidget *parent)
    : QDialog(parent), ui(new Ui::StitchIconDialog)
{
    ui->setupUi(this);

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
    
    dirs << userFolder + "icons/";

    QDir dir;
    dir.setPath(userFolder + "sets/");

    foreach(QString folder, dir.entryList(QDir::Dirs)) {
        if(folder != "." && folder != "..")
            dirs << userFolder + "sets/" + folder;
    }

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

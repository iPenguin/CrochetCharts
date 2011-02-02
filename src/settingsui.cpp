/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "settingsui.h"
#include "ui_settings.h"

#include <QDesktopServices>
#include <QFileDialog>

#include "settings.h"

SettingsUi::SettingsUi() :
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

#ifdef Q_WS_MAC
    this->setWindowTitle(tr("Preferences"));
#else
    this->setWindowTitle(tr("Options"));
#endif //Q_WS_MAC

    connect(ui->folderSelector, SIGNAL(clicked()), this, SLOT(selectFolder()));

    //TODO: load settings from disk:
    //TODO: use auto completer to help fill in this field.

    QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);

    ui->defaultFileLocation->setText(
                Settings::inst()->value("fileLocation", QVariant(userDocs)).toString());

    ui->checkForUpdates->setChecked(
                Settings::inst()->value("checkForUpdates", QVariant(true)).toBool());

}

SettingsUi::~SettingsUi()
{
    delete ui;
    ui = 0;
}

int SettingsUi::exec()
{

    int retValue = QDialog::exec();

    //TODO: save values to disk based on retValue;
    if(retValue != QDialog::Accepted)
        return retValue;

    Settings::inst()->setValue("fileLocation", QVariant(ui->defaultFileLocation->text()));

    Settings::inst()->setValue("checkForUpdates", QVariant(ui->checkForUpdates->isChecked()));

    return retValue;
}

void SettingsUi::selectFolder()
{
    QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QString defLoc = Settings::inst()->value("fileLocation", QVariant(userDocs)).toString();

    QString folder = QFileDialog::getExistingDirectory(this, tr("Select Default Folder"), defLoc);

    if(folder.isEmpty())
        return;

    ui->defaultFileLocation->setText(folder);
}

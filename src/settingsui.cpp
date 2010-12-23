#include "settingsui.h"
#include "ui_settings.h"

#include <QDesktopServices>
#include <QFileDialog>

#include "settings.h"

SettingsUi::SettingsUi() :
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->folderSelector, SIGNAL(clicked()), this, SLOT(selectFolder()));

    //TODO: load settings from disk:

    QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);

    ui->defaultFileLocation->setText(
                Settings::inst()->value("fileLocation", QVariant(userDocs)).toString());

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

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

    loadApplicationSettings();
    loadRoundChartSettings();
    loadInstructionSettings();
    loadLegendSettings();

}

SettingsUi::~SettingsUi()
{
    delete ui;
    ui = 0;
}

void SettingsUi::loadApplicationSettings()
{
    //TODO: use auto completer to help fill in this field.
    QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    
    ui->defaultFileLocation->setText(
        Settings::inst()->value("fileLocation", QVariant(userDocs)).toString());
    
    ui->checkForUpdates->setChecked(
        Settings::inst()->value("checkForUpdates", QVariant(true)).toBool());
}

void SettingsUi::loadRoundChartSettings()
{

    ui->defaultRows->setValue(
        Settings::inst()->value("defaultsRows", QVariant(15)).toInt());
    ui->defaultStitches->setValue(
        Settings::inst()->value("defaultsStitches", QVariant(15)).toInt());

}

void SettingsUi::loadInstructionSettings()
{

}

void SettingsUi::loadLegendSettings()
{

}

int SettingsUi::exec()
{

    int retValue = QDialog::exec();

    //TODO: save values to disk based on retValue;
    if(retValue != QDialog::Accepted)
        return retValue;

    saveApplicationSettings();
    saveRoundChartSettings();
    saveInstructionSettings();
    saveLegendSettings();

    return retValue;
}

void SettingsUi::saveApplicationSettings()
{
    Settings::inst()->setValue("fileLocation", QVariant(ui->defaultFileLocation->text()));
    Settings::inst()->setValue("checkForUpdates", QVariant(ui->checkForUpdates->isChecked()));
    
}

void SettingsUi::saveRoundChartSettings()
{
    Settings::inst()->setValue("defaultRows", QVariant(ui->defaultRows->value()));
    Settings::inst()->setValue("defaultStitches", QVariant(ui->defaultStitches->value()));
    
}

void SettingsUi::saveInstructionSettings()
{

}

void SettingsUi::saveLegendSettings()
{

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

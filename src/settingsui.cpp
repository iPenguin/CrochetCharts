#include "settingsui.h"
#include "ui_settings.h"

#include "settings.h"

SettingsUi::SettingsUi() :
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    //TODO: load settings from disk:

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


    return retValue;
}

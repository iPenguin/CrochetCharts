#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsUi : public QDialog
{
    Q_OBJECT
public:
    SettingsUi();
    ~SettingsUi();

    int exec();
private:
    Ui::SettingsDialog *ui;
};

#endif //SETTINGSUI_H

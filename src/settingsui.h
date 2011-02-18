/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QDialog>

class QAbstractButton;
class QPixmap;

namespace Ui {
    class SettingsDialog;
}

class SettingsUi : public QDialog
{
    Q_OBJECT
public:
    SettingsUi(QWidget *parent);
    ~SettingsUi();

    int exec();
public slots:
    void selectFolder();

private slots:
    void updatePrimaryColor();
    void updateAlternateColor();

    void buttonClicked(QAbstractButton *button);
private:

    //TODO: Using the same code in MainWindow
    QPixmap drawColorBox(QColor color, QSize size);
    
    void loadApplicationSettings();
    void loadRoundChartSettings();
    void loadInstructionSettings();
    void loadLegendSettings();

    void saveApplicationSettings();
    void saveRoundChartSettings();
    void saveInstructionSettings();
    void saveLegendSettings();

    void resetApplicationSettings();
    void resetRoundChartSettings();
    void resetInstructionSettings();
    void resetLegendSettings();

    void load(QWidget *w);
    void loadDefualt(QWidget *w);
    void save(QWidget *w);
    
    Ui::SettingsDialog *ui;

    QColor mPrimaryColor;
    QColor mAlternateColor;
    
};

#endif //SETTINGSUI_H

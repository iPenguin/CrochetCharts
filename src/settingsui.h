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

    bool stitchColorUpdated;
    QColor mOriginalColor;
    QColor mNewColor;

public slots:
    void selectFolder();

private slots:
    void setColor();

    void buttonClicked(QAbstractButton* button);
private:

    //TODO: Using the same code in MainWindow
    QPixmap drawColorBox(QColor color, QSize size);

    void setupDialogWidgets();
    void resetDialogWidgets();
    void saveDialogWidgets();

    void load(QObject *w);
    void loadDefualt(QObject *w);
    void save(QObject *w);

    bool isSettingsWidget(QObject *obj);

    Ui::SettingsDialog *ui;

    QColor mPrimaryColor;
    QColor mAlternateColor;
    QColor mDotColor;
    QColor mKeywordColor;

};

#endif //SETTINGSUI_H

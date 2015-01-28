/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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

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
#ifndef STITCHLIBRARYUI_H
#define STITCHLIBRARYUI_H

#include <QDialog>
#include "ui_stitchlibrary.h"
#include <QSortFilterProxyModel>

class StitchSet;

namespace Ui {
    class StitchLibraryDialog;
}

class StitchLibraryUi : public QDialog
{
    Q_OBJECT
public:
    explicit StitchLibraryUi(QWidget *parent = 0);
    ~StitchLibraryUi();

private slots:
    void changeStitchSet(QString setName);

    void addStitch();
    void removeStitch();
    void addSelected();
    
    void resetLibrary();

    void hideProperties();
    void updateStitchSetProperties();
    void printStitchSet();

    void createSet();
    void removeSet();
    
    void importSet();
    void exportSet();
   
private slots:
    void iconDialog();

    /**
     * After the user re-sorts the stitches we want to
     * resize the cells to fit the stitch that's now in the row.
     **/
    void updateRowSizes();
    
    void filterStitchList(QString newText);
    void clearStitchFilter();

private:
    Ui::StitchLibraryDialog *ui;
    QSortFilterProxyModel *mProxyModel;

    void setDialogSize();

    /**
     *Set the author, set name, email, url, etc.
     */
    void setupPropertiesBox();
    void setButtonStates(StitchSet *set);

    //update the source dropdown list and switch to setName
    void updateSourceDropDown(QString setName = "");

    StitchSet* addStitchSet(QString fileName);
};

#endif //STITCHLIBRARYUI_H

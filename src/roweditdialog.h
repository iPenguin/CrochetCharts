/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
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
#ifndef ROWEDITDIALOG_H
#define ROWEDITDIALOG_H

#include <QWidget>

#include "scene.h"
#include "textview.h"

namespace Ui {
    class RowEditDialog;
}

class QListWidgetItem;

class RowEditDialog : public QWidget
{
    Q_OBJECT
public:
    RowEditDialog(Scene* scene, TextView* textView, QWidget* parent = 0);
    ~RowEditDialog();

    void updateRowList();

    void show();
    
signals:
    /**
     *Row is the grid row.
     */
    void displayRow(int row);
    
private slots:
    void addRow();
    void removeRow();
    
    void moveUp();
    void moveDown();

    /**
     * row is the row in the rowList widget.
     */
    void listItemChanged(int listRow);
    void listItemClicked(QListWidgetItem* item);
    
    void updateRow();
    
private:

    void removeEmptyRows();
    
    Scene* mScene;
    TextView *mTextView;
    
    Ui::RowEditDialog* ui;
};
#endif //ROWEDITDIALOG_H

/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef ROWEDITDIALOG_H
#define ROWEDITDIALOG_H

#include <QWidget>

#include "scene.h"
#include "textview.h"

namespace Ui {
    class RowEditDialog;
}

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

    void updateRow();
    
private:

    Scene* mScene;
    TextView *mTextView;
    
    Ui::RowEditDialog* ui;
};
#endif //ROWEDITDIALOG_H

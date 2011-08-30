/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef ROWEDITDIALOG_H
#define ROWEDITDIALOG_H

#include <QWidget>

#include "scene.h"

namespace Ui {
    class RowEditDialog;
}

class RowEditDialog : public QWidget
{
    Q_OBJECT
public:
    RowEditDialog(Scene* scene, QWidget* parent = 0);
    ~RowEditDialog();

    void updateRowList();
    
signals:
    void createRow(int row);
    void selectRow(int row);
    
private slots:
    void addRow();
    void removeRow();
    
    void moveUp();
    void moveDown();

    /**
     * row is the row in the rowList widget.
     */
    void listItemChanged(int listRow);

private:

    Scene* mScene;
    
    Ui::RowEditDialog* ui;
};
#endif //ROWEDITDIALOG_H
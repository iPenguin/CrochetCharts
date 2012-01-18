/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "roweditdialog.h"
#include "ui_roweditdialog.h"

#include "stitchlibrary.h"

#include "debug.h"
#include <QMessageBox>


RowEditDialog::RowEditDialog(Scene* scene, TextView* textView, QWidget* parent)
    : QWidget(parent),
    mScene(scene),
    mTextView(textView),
    ui(new Ui::RowEditDialog)
{
    ui->setupUi(this);
    
    connect(ui->addRow, SIGNAL(clicked(bool)), SLOT(addRow()));
    connect(ui->removeRow, SIGNAL(clicked(bool)), SLOT(removeRow()));

    connect(ui->moveUp, SIGNAL(clicked(bool)), SLOT(moveUp()));
    connect(ui->moveDown, SIGNAL(clicked(bool)), SLOT(moveDown()));

    connect(ui->rowList, SIGNAL(currentRowChanged(int)), SLOT(listItemChanged(int)));
    connect(ui->rowList, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(listItemClicked(QListWidgetItem*)));
    
    connect(ui->updateRow, SIGNAL(clicked(bool)), SLOT(updateRow()));
}

RowEditDialog::~RowEditDialog()
{
}

void RowEditDialog::show()
{
    QWidget::show();
    updateRowList();
}

void RowEditDialog::addRow()
{
    if(mScene->selectedItems().count() <= 0) {
        QMessageBox msgbox;
        msgbox.setText(tr("Please select the stitches you would like to create a row from."));
        msgbox.setIcon(QMessageBox::Information);
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.exec();
        return;
    }

    mScene->createRow();

    updateRowList();

}

void RowEditDialog::removeRow()
{

    int curRow = ui->rowList->currentRow();
    if(curRow < 0)
        return;
    
    ui->rowList->reset();
    ui->rowList->takeItem(curRow);
    
    mScene->removeRow(curRow);
    updateRowList();

    //select the next item in the list.
    QListWidgetItem* item;
    if(ui->rowList->count() > curRow)
        item = ui->rowList->item(curRow);
    else {
        curRow = ui->rowList->count() - 1;
        if(curRow < 0)
            return;
        item = ui->rowList->item(curRow);
    }
    if(item)
        ui->rowList->setCurrentItem(item);
}

void RowEditDialog::moveUp()
{

    int curRow = ui->rowList->currentRow();
    if(curRow <= 0 || curRow >= ui->rowList->count())
        return;
    
    mScene->moveRowUp(curRow);
    updateRowList();

}

void RowEditDialog::moveDown()
{

    int curRow = ui->rowList->currentRow();

    if(curRow + 1 >= ui->rowList->count() || curRow < 0)
        return;

    mScene->moveRowDown(curRow);
    updateRowList();
}

void RowEditDialog::listItemChanged(int listRow)
{
    listItemClicked(ui->rowList->item(listRow));
}

void RowEditDialog::listItemClicked(QListWidgetItem* item)
{
    int r = item->text().toInt() - 1;
    mScene->highlightRow(r);

    QString rowText = mTextView->generateTextRow(r, true,true);
    ui->rowView->setText(rowText);

    mScene->drawRowLines(r);
}

void RowEditDialog::updateRowList()
{

    ui->rowList->reset();
    ui->rowList->clear();

    removeEmptyRows();
    
    int rows = mScene->grid.count();

    for(int i = 0; i < rows; ++i) {
        //using this slightly more complicated addItem() allows the sorting to work correctly for numbers.
        QListWidgetItem* item = new QListWidgetItem();
        item->setData(Qt::DisplayRole, i + 1);
        ui->rowList->addItem(item);
    }

}

void RowEditDialog::removeEmptyRows()
{
    int rows = mScene->grid.count();
    
    for(int i = rows - 1 ; i >= 0; i--) {
        int cols = mScene->grid.at(i).count();
        for(int j = cols - 1; j >= 0; j--) {
            if(mScene->grid[i][j] == 0)
                mScene->grid[i].removeAt(j);
        }
        if(mScene->grid[i].count() == 0)
            mScene->grid.removeAt(i);
    }
    
}

void RowEditDialog::updateRow()
{

    if(mScene->selectedItems().count() <= 0)
        return;
    
    int r = ui->rowList->currentItem()->text().toInt();
    if(r <= 0)
        return;
    r--;

    mScene->updateRow(r);

    QString rowText = mTextView->generateTextRow(r, true,true);
    ui->rowView->setText(rowText);
    
}

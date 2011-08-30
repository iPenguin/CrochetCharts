/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "roweditdialog.h"
#include "ui_roweditdialog.h"

#include "stitchlibrary.h"

#include <QDebug>
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
    
}

RowEditDialog::~RowEditDialog()
{
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
    int newRow = mScene->rows.count();
    mScene->createRow(newRow);

    updateRowList();

}

void RowEditDialog::removeRow()
{
    //TODO: remove actual row from the chart.
    
    ui->rowList->takeItem(ui->rowList->currentRow());
    updateRowList();
}

void RowEditDialog::moveUp()
{

    int curRow = ui->rowList->currentRow();

    if(curRow == 0)
        return;

    mScene->moveRowUp(curRow);
    updateRowList();
}

void RowEditDialog::moveDown()
{

    int curRow = ui->rowList->currentRow();

    if(curRow + 1 == ui->rowList->count())
        return;

    mScene->moveRowDown(curRow);
    updateRowList();
}

void RowEditDialog::listItemChanged(int listRow)
{
    int r = ui->rowList->item(listRow)->text().toInt();
    mScene->highlightRow(r - 1);

    QString rowText = mTextView->generateTextRow(listRow, true,true);
    qDebug() << rowText;
    ui->rowView->setText(rowText);
}

void RowEditDialog::updateRowList()
{

    ui->rowList->reset();
    ui->rowList->clear();

    int rows = mScene->rows.count();

    for(int i = 0; i < rows; ++i) {
        ui->rowList->addItem(QString::number(i + 1));
    }

}

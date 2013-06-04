#include "colorreplacer.h"
#include "ui_colorreplacer.h"

#include <QMessageBox>
#include <QPushButton>

#include <QColorDialog>

#include "colorlistwidget.h"

ColorReplacer::ColorReplacer(QList<QString> colorList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorReplacer),
    mOriginalColorList(colorList)
{
    ui->setupUi(this);

    if(mOriginalColorList.isEmpty()) {
        QPushButton *ok = ui->buttonBox->button(QDialogButtonBox::Ok);
        ok->setEnabled(false);
    }

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(ui->stitchesAndBackground, SIGNAL(clicked()), SLOT(setSelection()));
    connect(ui->stitchesOnly, SIGNAL(clicked()), SLOT(setSelection()));
    connect(ui->backgroundOnly, SIGNAL(clicked()), SLOT(setSelection()));

    connect(ui->originalColor, SIGNAL(currentIndexChanged(QString)), SLOT(origColorChanged(QString)));
    connect(ui->newColor, SIGNAL(currentIndexChanged(QString)), SLOT(newColorChanged(QString)));

    populateColorLists();
}

ColorReplacer::~ColorReplacer()
{
    delete ui;
}

void ColorReplacer::accept()
{

    if(!originalColor.isValid()) {
        QMessageBox msgbox;
        msgbox.setText("Test msgbox");
        return;
    }

    if(!newColor.isValid()) {
        QMessageBox msgbox;
        msgbox.setText("Test2 msgbox");
        return;
    }

    QDialog::accept();
}

void ColorReplacer::setSelection()
{

    if(sender() == ui->stitchesAndBackground)
        selection = 3;
    else if(sender() == ui->stitchesOnly)
        selection = 1;
    else if(sender() == ui->backgroundOnly)
        selection = 2;

}

void ColorReplacer::origColorChanged(QString color)
{
    originalColor = QColor(color);
}

void ColorReplacer::newColorChanged(QString color)
{
    if(color == tr("More colors...")) {
        QColor c = QColorDialog::getColor(Qt::black, this);
        if(c.isValid()) {
            ui->newColor->addItem(
                        QIcon(ColorListWidget::drawColorBox(c, QSize(32,32))), c.name());
            ui->newColor->setCurrentIndex(ui->newColor->findText(c.name()));
            newColor = c;
        }
        return;
    }

    newColor = QColor(color);
}

void ColorReplacer::populateColorLists()
{

    foreach(QString color, mOriginalColorList) {
        QColor curColor = QColor(color);

        ui->originalColor->addItem(
                    QIcon(ColorListWidget::drawColorBox(curColor, QSize(32,32))),curColor.name());
    }

    foreach(QString color, mOriginalColorList) {
        QColor curColor = QColor(color);

        ui->newColor->addItem(
                    QIcon(ColorListWidget::drawColorBox(curColor, QSize(32,32))),curColor.name());
    }

    ui->newColor->addItem(tr("More colors..."));
}

#include "guidelinesdialog.h"
#include "ui_guidelinesdialog.h"

GuidelinesDialog::GuidelinesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuidelinesDialog)
{
    ui->setupUi(this);
}

GuidelinesDialog::~GuidelinesDialog()
{
    delete ui;
}

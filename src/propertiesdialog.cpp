#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"

PropertiesDialog::PropertiesDialog(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PropertiesDialog)
{
    ui->setupUi(this);
}

PropertiesDialog::~PropertiesDialog()
{
    delete ui;
}

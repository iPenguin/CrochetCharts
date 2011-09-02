#include "aligndock.h"
#include "ui_aligndock.h"

AlignDock::AlignDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AlignDock)
{
    ui->setupUi(this);
}

AlignDock::~AlignDock()
{
    delete ui;
}

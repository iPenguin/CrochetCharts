#include "alignanddistribute.h"
#include "ui_alignanddistribute.h"

AlignAndDistribute::AlignAndDistribute(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlignAndDistribute)
{
    ui->setupUi(this);
}

AlignAndDistribute::~AlignAndDistribute()
{
    delete ui;
}

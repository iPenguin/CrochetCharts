#include "mirrordock.h"
#include "ui_mirrordock.h"

MirrorDock::MirrorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MirrorDock)
{
    ui->setupUi(this);

    ui->rotateCustomWidgets->hide();

    connect(ui->mirrorLeft, SIGNAL(clicked()), SLOT(genMirror()));
    connect(ui->mirrorRight, SIGNAL(clicked()), SLOT(genMirror()));
    connect(ui->mirrorUp, SIGNAL(clicked()), SLOT(genMirror()));
    connect(ui->mirrorDown, SIGNAL(clicked()), SLOT(genMirror()));

    connect(ui->rotate90, SIGNAL(clicked()), SLOT(genRotate()));
    connect(ui->rotate180, SIGNAL(clicked()), SLOT(genRotate()));
    connect(ui->rotate270, SIGNAL(clicked()), SLOT(genRotate()));

    connect(ui->rotateCustom, SIGNAL(clicked()), SLOT(rotateCustom()));

    connect(ui->rotateCustom, SIGNAL(clicked()), SLOT(genRotate()));
}

MirrorDock::~MirrorDock()
{
    delete ui;
}

void MirrorDock::genMirror()
{
    int direction = 1;
    if(sender() == ui->mirrorLeft)
        direction = 1;
    else if(sender() == ui->mirrorRight)
        direction = 2;
    else if(sender() == ui->mirrorUp)
        direction = 3;
    else if(sender() == ui->mirrorDown)
        direction = 4;

    emit mirror(direction);
}

void MirrorDock::genRotate()
{
    qreal degrees = 0;

    if(sender() == ui->rotate90)
        degrees = 90;
    else if(sender() == ui->rotate180)
        degrees = 180;
    else if(sender() == ui->rotate270)
        degrees = 270;
    else if(sender() == ui->rotateBttn)
        degrees = ui->degrees->value();

    emit rotate(degrees);
}

void MirrorDock::rotateCustom()
{
    if(ui->rotateCustomWidgets->isVisible()) {
        ui->rotateCustomWidgets->setVisible(false);
    } else {
        ui->rotateCustomWidgets->show();
    }
}

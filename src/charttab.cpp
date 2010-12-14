#include "charttab.h"

#include <QGraphicsView>
#include <QVBoxLayout>
#include <QSvgRenderer>
#include <QDebug>

#include "crochetscene.h"
#include "crochetcell.h"

ChartTab::ChartTab(QWidget *parent) :
    QWidget(parent)
{

    QVBoxLayout *l = new QVBoxLayout(this);
    mView = new QGraphicsView(this);
    mScene = new CrochetScene(this);
    mView->setScene(mScene);
    l->addWidget(mView);

    double pi = 3.14;
    double circumference = 8 * 64;
    double diameter = circumference / pi;
    double radius = diameter /2 ;

    mScene->addEllipse(-radius,-radius, diameter, diameter);
    mScene->addEllipse(-4, -4, 8, 8);

    CrochetCell *c;

//top right
    c= new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(0);
    c->setY(-64);
    c->rotate((45*7) - 22.5);

    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(48);
    c->setY(-48);
    c->rotate((45*0) - 22.5);

//bottom right
    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(64);
    c->setY(0);
    c->rotate((45*1) - 22.5);

    c= new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(48);
    c->setY(48);
    c->rotate((45*2) - 22.5);

//bottom left
    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(0);
    c->setY(64);
    c->rotate((45*3) - 22.5);

    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(-48);
    c->setY(48);
    c->rotate((45*4) - 22.5);

//top left
    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(-64);
    c->setY(0);
    c->rotate((45*5) - 22.5);

    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(-48);
    c->setY(-48);
    c->rotate((45*6) - 22.5);


/*****************************************\
|row 2:
\*****************************************/

    circumference = 12 * 64;
    radius = radius + radius;
    diameter = radius * 2;
    mScene->addEllipse(-radius,-radius, diameter, diameter);

    qDebug() << "radius" << radius;
//top right
    c= new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(0);
    c->setY(-radius);
    c->rotate((45*7) - 22.5);

    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(64);
    c->setY(-64);
    c->rotate((45*0) - 22.5);

    c = new CrochetCell(":/stitches/chain.svg");
    mScene->addItem(c);
    c->setX(96);
    c->setY(0);
    c->rotate((45*1) - 22.5);





}



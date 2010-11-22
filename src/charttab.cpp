#include "charttab.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QSvgRenderer>

#include "crochetcell.h"

ChartTab::ChartTab(QWidget *parent) :
    QWidget(parent)
{

    QVBoxLayout *l = new QVBoxLayout(this);
    mView = new QGraphicsView(this);
    mScene = new QGraphicsScene(this);
    mView->setScene(mScene);
    l->addWidget(mView);

    mScene->addEllipse(-64,-64, 128, 128);
    mScene->addEllipse(-128,-128, 256, 256);
    mScene->addEllipse(-4, -4, 8, 8);

    CrochetCell *c = new CrochetCell(":/stitches/chain.svg");
    c->setPos(80, -32);
    c->rotate(90);
    mScene->addItem(c);

    c = new CrochetCell(":/stitches/single_crochet.svg");
    mScene->addItem(c);
    //c->setRotation(45);

}



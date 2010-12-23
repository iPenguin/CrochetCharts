#include "charttab.h"

#include <QGraphicsView>
#include <QVBoxLayout>
#include <QSvgRenderer>
#include <QDebug>

#include <math.h>

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
    l->setMargin(0);

    //FIXME: less then 8 stitches gives funny rows.
    this->createRow(8);
    this->createRow(12);
    this->createRow(16);
    this->createRow(20);
    this->createRow(24);
    this->createRow(28);
    this->createRow(32);
    this->createRow(36);
    this->createRow(40);
    this->createRow(44);
    this->createRow(48);
    this->createRow(52);
}

void ChartTab::createRow(int columns)
{
    double pi = M_PI;
    int stitchWidth = 64;
    double widthInDegrees = 360.0 / columns;
    double circumference = (columns -2) * stitchWidth;
    double diameter = circumference / pi;
    double radius = diameter /2;

    CrochetCell *c;

    for(int i = 0; i < columns; ++i) {
        double degrees = (widthInDegrees*i) - (widthInDegrees/2);
        QPointF finish = this->calcPoint(radius, degrees, QPointF(0,0));

        c = new CrochetCell(":/stitches/chain.svg");
        mScene->addItem(c);
        c->setPos(finish);
        c->setToolTip(QString::number(i+1));
        c->rotate(degrees + (widthInDegrees/2));
    }
}

QPointF ChartTab::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    double x = (double)(radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    double y = (double)(radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

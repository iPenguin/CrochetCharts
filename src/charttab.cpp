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

    double pi = M_PI;
    int columns = 8;
    int stitchWidth = 64;
    double widthInDegrees = 360 / columns;
    double circumference = columns * stitchWidth;
    double diameter = circumference / pi;
    double radius = diameter /2;

    mScene->addEllipse(-radius,-radius, diameter, diameter);
    mScene->addEllipse(-4, -4, 8, 8);

    CrochetCell *c;
/*****************************************\
|row 1:
\*****************************************/
    for(int i = 0; i < columns; ++i) {
        double degrees = (widthInDegrees*i) - (widthInDegrees/2);
        QPointF finish = this->calcPoint(radius, degrees, QPointF(0,0));

        c = new CrochetCell(":/stitches/chain.svg");
        mScene->addItem(c);
        c->setX(finish.x());
        c->setY(finish.y());
        c->rotate(degrees + (widthInDegrees/2));
    }


/*****************************************\
|row 2:
\*****************************************/

    columns = 12;
    circumference = columns * stitchWidth;
    widthInDegrees = 360 / columns;
    diameter = circumference / pi;
    radius = diameter /2;

    mScene->addEllipse(-radius,-radius, diameter, diameter);

    for(int i = 0; i < columns; ++i) {
        double degrees = (widthInDegrees*i) - (widthInDegrees/2);
        QPointF finish = this->calcPoint(radius, degrees, QPointF(0,0));

        c = new CrochetCell(":/stitches/chain.svg");
        mScene->addItem(c);
        c->setX(finish.x());
        c->setY(finish.y());
        c->rotate(degrees + (widthInDegrees/2));
    }

}

QPointF ChartTab::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
     //* M_PI / 180
    double x = (double)(radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    double y = (double)(radius * sin(angleInDegrees * M_PI / 180)) + origin.y();

    return QPointF(x, y);
}

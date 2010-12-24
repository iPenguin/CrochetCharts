#include "charttab.h"

#include <QGraphicsView>
#include <QVBoxLayout>
#include <QSvgRenderer>
#include <QDebug>

#include <QThread>
#include <QtConcurrentRun>

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

    this->createRow(8);
    this->createRow(14);
    this->createRow(20);
    this->createRow(26);
    this->createRow(32);
    this->createRow(38);
    this->createRow(44);
    this->createRow(50);
    this->createRow(56);
    this->createRow(62);
    this->createRow(68);
    this->createRow(74);
/*
    //FIXME: less then 8 stitches gives funny rows.
    //TODO: make this work better with threads!
    QFuture<void> f1 = QtConcurrent::run(this, &ChartTab::createRow, 8);
    QFuture<void> f2 = QtConcurrent::run(this, &ChartTab::createRow, 14);
    QFuture<void> f3 = QtConcurrent::run(this, &ChartTab::createRow, 20);
    QFuture<void> f4 = QtConcurrent::run(this, &ChartTab::createRow, 26);
    QFuture<void> f5 = QtConcurrent::run(this, &ChartTab::createRow, 32);
    QFuture<void> f6 = QtConcurrent::run(this, &ChartTab::createRow, 38);
    QFuture<void> f7 = QtConcurrent::run(this, &ChartTab::createRow, 44);
    QFuture<void> f8 = QtConcurrent::run(this, &ChartTab::createRow, 50);
    QFuture<void> f9 = QtConcurrent::run(this, &ChartTab::createRow, 56);
    QFuture<void> f10 = QtConcurrent::run(this, &ChartTab::createRow, 62);
    QFuture<void> f11 = QtConcurrent::run(this, &ChartTab::createRow, 68);
    QFuture<void> f12 = QtConcurrent::run(this, &ChartTab::createRow, 74);

    f1.waitForFinished();
    f2.waitForFinished();
    f3.waitForFinished();
    f4.waitForFinished();
    f5.waitForFinished();
    f6.waitForFinished();
    f7.waitForFinished();
    f8.waitForFinished();
    f9.waitForFinished();
    f10.waitForFinished();
    f11.waitForFinished();
    f12.waitForFinished();
*/

}

void ChartTab::createRow(int columns)
{
    int stitchWidth = 64;
    double widthInDegrees = 360.0 / columns;
    double circumference = (columns -2) * stitchWidth;
    double diameter = circumference / M_PI;
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

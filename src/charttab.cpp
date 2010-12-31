#include "charttab.h"

#include <QGraphicsView>
#include <QVBoxLayout>

#include <QtSvg/QSvgGenerator>
#include <QPrinter>

#include <QDebug>

#include "crochetscene.h"

#include "settings.h"

ChartTab::ChartTab(QWidget *parent) :
    QWidget(parent)
{

    QVBoxLayout *l = new QVBoxLayout(this);
    mView = new QGraphicsView(this);
    mScene = new CrochetScene(this);
    mView->setScene(mScene);
    l->addWidget(mView);
    l->setMargin(0);

    mScene->createRow(8);
    mScene->createRow(14);
    mScene->createRow(20);
    mScene->createRow(26);
    mScene->createRow(32);
    mScene->createRow(38);
    mScene->createRow(44);
    mScene->createRow(50);
    mScene->createRow(56);
    mScene->createRow(62);
    mScene->createRow(68);
    mScene->createRow(74);
}

void ChartTab::savePdf(QPrinter printer, QString fileName, int resolution)
{
    //Export as pdf
    //Test run was missing a stitch in lower right corner.
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setResolution(resolution);
    QPainter p;
    p.begin(&printer);
    mScene->render(&p);
    p.end();
}

void ChartTab::saveSvg(QString fileName)
{
    //Export as svg
    QSvgGenerator gen;
    //TODO: fill in more info about the svg.
    gen.setFileName(fileName);
    gen.setSize(mScene->sceneRect().size().toSize());
    QPainter p;

    p.begin(&gen);
    mScene->render(&p);
    p.end();
}

void ChartTab::saveImage(QString fileName, QSize size, int resolution)
{
    //Export as image
    QPainter p;
    double dpm = resolution * (39.3700787);
    QImage img = QImage(mScene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    img.setDotsPerMeterX(dpm);
    img.setDotsPerMeterY(dpm);
//FIXME: needs a background color so it doesn't use white noise.
// Leave white noise for demo version? can it be reproduced on all OSes?

    p.begin(&img);
    mScene->render(&p, QRectF(0, 0, size.width(), size.height()), mScene->sceneRect());
    p.end();

    img.save(fileName, "", -1);

}

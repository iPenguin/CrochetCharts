/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "charttab.h"

#include <QVBoxLayout>
#include <QSplitter>

#include <QtSvg/QSvgGenerator>
#include <QPrinter>

#include <QXmlStreamWriter>

#include <QDebug>

#include "crochetscene.h"
#include "crochettextview.h"
#include "settings.h"
#include <qdom.h>

ChartTab::ChartTab(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->setObjectName("chartSplitter");
    l->addWidget(splitter);

    mView = new ChartView(this);
    mScene = new CrochetScene(this);
    connect(mScene, SIGNAL(stitchChanged(QString,QString)), this, SLOT(stitchChanged(QString,QString)));
    connect(mScene, SIGNAL(colorChanged(QString,QString)), this, SLOT(colorChanged(QString,QString)));
    mView->setScene(mScene);
    mTextView = new CrochetTextView(this, mScene);

    splitter->addWidget(mView);
    splitter->addWidget(mTextView);
    l->setMargin(0);

    mView->setMinimumSize(width(), height()*2/3);
    splitter->setStretchFactor(0, 8);

    mView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    //click and drag mode.
    //mView->setDragMode(QGraphicsView::ScrollHandDrag);
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

void ChartTab::stitchChanged(QString oldSt, QString newSt)
{
    if (!oldSt.isEmpty()) {
        mPatternStitches->operator[](oldSt)--;
        if (mPatternStitches->operator[](oldSt) == 0)
            mPatternStitches->remove(oldSt);
    }

    if (!mPatternStitches->contains(newSt))
        mPatternStitches->insert(newSt, 1);
    else
        mPatternStitches->operator[](newSt)++;

    emit chartStitchChanged();
}

void ChartTab::colorChanged(QString oldColor, QString newColor)
{
    if (!oldColor.isEmpty()) {
        mPatternColors->operator[](oldColor)["count"]--;
        if (mPatternColors->operator[](oldColor)["count"] == 0)
            mPatternColors->remove(oldColor);
    }

    if (!mPatternColors->contains(newColor)) {
        QMap<QString, int> properties;
        properties["color number"] = mPatternColors->count() + 1;
        properties["count"] = 1;
        mPatternColors->insert(newColor, properties);
    } else
        mPatternColors->operator[](newColor)["count"]++;

    emit chartColorChanged();
}

void ChartTab::zoomIn()
{
    mView->zoomIn();
}

void ChartTab::zoomOut()
{
    mView->zoomOut();
}

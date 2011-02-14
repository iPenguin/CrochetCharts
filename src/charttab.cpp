/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "charttab.h"

#include <QVBoxLayout>
#include <QSplitter>

#include <QPainter>

#include <QXmlStreamWriter>

#include <QDebug>

#include "crochetscene.h"
#include "crochettextview.h"
#include "settings.h"

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

    setupMenuItems();
    //click and drag mode.
    //mView->setDragMode(QGraphicsView::ScrollHandDrag);
}

ChartTab::~ChartTab()
{
    delete mView;
    delete mScene;
    delete mTextView;
    mView = 0;
    mScene = 0;
    mTextView = 0;
}

void ChartTab::setupMenuItems()
{
    QActionGroup group;
    QAction *a;

    a = new QAction(this);
    a->setText(tr("Stitch Mode"));
    group.addAction(a);
    mModeActions.append(a);

    a = new QAction(this);
    a->setText(tr("Code Mode"));
    group.addAction(a);
    mModeActions.append(a);

    a = new QAction(this);
    a->setText(tr("Grid Mode"));
    group.addAction(a);
    mModeActions.append(a);

    a = new QAction(this);
    a->setText(tr("Position Mode"));
    group.addAction(a);
    mModeActions.append(a);

    a = new QAction(this);
    a->setText(tr("Repeat Mode"));
    group.addAction(a);
    mModeActions.append(a);
    
}

void ChartTab::renderChart(QPainter *painter, QRectF rect)
{
    if(!rect.isValid())
        mScene->render(painter);
    else
        mScene->render(painter, rect, mScene->sceneRect());
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

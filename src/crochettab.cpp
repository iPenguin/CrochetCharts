/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochettab.h"
#include "src/ui_optionsbar.h"

#include <QVBoxLayout>
#include <QSplitter>

#include <QPainter>

#include <QXmlStreamWriter>

#include <QDebug>

#include "crochetscene.h"
#include "crochettextview.h"
#include "settings.h"
#include <QDate>
#include <QLayout>
#include <QClipboard>

CrochetTab::CrochetTab(int defEditMode, QString defStitch, QColor defFgColor, QColor defBgColor, QWidget *parent) :
        QWidget(parent), ui(new Ui::OptionsBar)
{    
    QVBoxLayout *l = new QVBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->setObjectName("chartSplitter");
    l->addWidget(splitter);

    QWidget *top = new QWidget(this);
    QVBoxLayout *tl = new QVBoxLayout(top);
    top->setLayout(tl);
    top->layout()->setMargin(0);
    
    mView = new ChartView(top);
    mScene = new CrochetScene(mView);

    connect(mView, SIGNAL(scrollBarChanged(int,int)), mScene, SLOT(updateRubberBand(int,int)));
    
    connect(mScene, SIGNAL(stitchChanged(QString,QString)), SLOT(stitchChanged(QString,QString)));
    connect(mScene, SIGNAL(colorChanged(QString,QString)), SLOT(colorChanged(QString,QString)));

    mView->setScene(mScene);
    mTextView = new CrochetTextView(this, mScene);

    mScene->setEditMode((CrochetScene::EditMode)defEditMode);
    mScene->setEditStitch(defStitch);
    mScene->setEditFgColor(defFgColor);
    mScene->setEditBgColor(defBgColor);

    QWidget *w = new QWidget(top);
    ui->setupUi(w);
    tl->addWidget(mView);
    tl->addWidget(w);

    splitter->addWidget(top);
    splitter->addWidget(mTextView);
    l->setMargin(0);

    mView->setMinimumSize(width(), height()*2/3);
    splitter->setStretchFactor(0, 8);

    mView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    connect(ui->copyInstructions, SIGNAL(clicked()), this, SLOT(copyInstructions()));
    
    //click and drag mode.
    //mView->setDragMode(QGraphicsView::ScrollHandDrag);
}

CrochetTab::~CrochetTab()
{
    delete mView;
    delete mScene;
    delete mTextView;
    mView = 0;
    mScene = 0;
    mTextView = 0;
}

QStringList CrochetTab::editModes()
{
    QStringList modes;
    modes << tr("Stitch Mode") << tr("Color Mode") << tr("Grid Mode") << tr("Position Mode");
    return modes;
}

int CrochetTab::editMode() const
{
    return (int) mScene->editMode();
}

void CrochetTab::setEditMode(int mode)
{
    mScene->setEditMode((CrochetScene::EditMode)mode);
}

void CrochetTab::renderChart(QPainter *painter, QRectF rect)
{
    if(!rect.isValid())
        mScene->render(painter);
    else
        mScene->render(painter, rect, mScene->sceneRect());
}

void CrochetTab::stitchChanged(QString oldSt, QString newSt)
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

void CrochetTab::colorChanged(QString oldColor, QString newColor)
{
    if (!oldColor.isEmpty()) {
        mPatternColors->operator[](oldColor)["count"]--;
        if (mPatternColors->operator[](oldColor)["count"] == 0)
            mPatternColors->remove(oldColor);
    }

    if (!mPatternColors->contains(newColor)) {
        QMap<QString, qint64> properties;
        properties["added"] = QDateTime::currentDateTime().toMSecsSinceEpoch();
        properties["count"] = 1;
        mPatternColors->insert(newColor, properties);
    } else
        mPatternColors->operator[](newColor)["count"]++;

    emit chartColorChanged();
}

void CrochetTab::zoomIn()
{
    mView->zoomIn();
}

void CrochetTab::zoomOut()
{
    mView->zoomOut();
}

QUndoStack* CrochetTab::undoStack()
{
    return mScene->undoStack();
}

void CrochetTab::createChart(QString style, int rows, int cols, QString defStitch)
{
    CrochetScene::ChartStyle st = CrochetScene::Flat;
    
    if(style == tr("Flat"))
        st  = CrochetScene::Flat;
    else if(style == tr("Round"))
        st = CrochetScene::Round;
    
    mScene->createChart(st, rows, cols, defStitch);
}

void CrochetTab::setEditBgColor(QColor color)
{
    mScene->setEditBgColor(color);
}

void CrochetTab::setEditFgColor(QColor color)
{
    mScene->setEditFgColor(color);
}

void CrochetTab::setEditStitch(QString stitch)
{
    mScene->setEditStitch(stitch);
}

void CrochetTab::copyInstructions()
{
    QClipboard *clipboard = QApplication::clipboard();

    QString instructions = mTextView->copyInstructions();
    clipboard->setText(instructions);
    
}

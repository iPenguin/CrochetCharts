/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochettab.h"
#include "ui_optionsbar.h"

#include <QVBoxLayout>
#include <QSplitter>

#include <QPainter>

#include <QXmlStreamWriter>

#include <QDebug>

#include "scene.h"
#include "scenerows.h"
#include "scenerounds.h"

#include "textview.h"

#include "settings.h"
#include <QDate>
#include <QLayout>
#include <QClipboard>

CrochetTab::CrochetTab(Scene::ChartStyle style, int defEditMode, QString defStitch,
                       QColor defFgColor, QColor defBgColor, QWidget* parent)
        : QWidget(parent),
        ui(new Ui::OptionsBar),
        mChartStyle(style)
{
    QVBoxLayout* l = new QVBoxLayout(this);
    QWidget* top = new QWidget(this);
    l->addWidget(top);
    
    QVBoxLayout* tl = new QVBoxLayout(top);
    top->setLayout(tl);
    top->setContentsMargins(0, 0, 0, 0);

    QPoint centerOn = QPoint(0,0);
    
    mView = new ChartView(top);
    if(style == Scene::Blank) {
        mScene = new Scene(mView);
        mScene->setSceneRect(0,0, 5000,5000);
        centerOn = QPoint(2500, 2500);
    } else if(style == Scene::Rounds) {
        mScene = new SceneRounds(mView);
        mScene->setSceneRect(-2500, -2500, 5000, 5000);
    } else {
        mScene = new SceneRows(mView);
        mScene->setSceneRect(-100, -100, 5000, 5000);
    }

    mTextView = new TextView(this, mScene);

    connect(mView, SIGNAL(scrollBarChanged(int,int)), mScene, SLOT(updateRubberBand(int,int)));
    
    connect(mScene, SIGNAL(stitchChanged(QString,QString)), SLOT(stitchChanged(QString,QString)));
    connect(mScene, SIGNAL(colorChanged(QString,QString)), SLOT(colorChanged(QString,QString)));

    mView->setScene(mScene);
    QPoint pt = mView->mapFromScene(centerOn);
    mView->centerOn(pt.x(), pt.y());

    mScene->setEditMode((Scene::EditMode)defEditMode);
    mScene->setEditStitch(defStitch);
    mScene->setEditFgColor(defFgColor);
    mScene->setEditBgColor(defBgColor);
    
    QWidget* w = new QWidget(top);
    ui->setupUi(w);
    tl->addWidget(mView);
    tl->addWidget(w);

    ui->horizontalLayout->setMargin(0);
       
    l->setMargin(0);
    tl->setMargin(0);
    w->setContentsMargins(0, 0, 0, 0);
    
    setContentsMargins(0, 0, 0, 0);

    mView->setMinimumSize(width(), height()*2/3);
    
    mView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    mRowEditDialog = new RowEditDialog(scene(), mTextView, this);
    ui->dialogLayout->addWidget(mRowEditDialog);
    mRowEditDialog->hide();
    
    ui->chartOptionsBox->setVisible(false);
    connect(ui->moreBttn, SIGNAL(clicked()), SLOT(showChartOptions()));
   
    connect(ui->copyInstructions, SIGNAL(clicked()), SLOT(copyInstructions()));

    connect(ui->zoom, SIGNAL(valueChanged(int)), SLOT(zoomChanged(int)));
    connect(mView, SIGNAL(zoomLevelChanged(int)), SLOT(updateZoomLevel(int)));

    connect(ui->showChartCenter, SIGNAL(clicked(bool)), SLOT(setShowChartCenter(bool)));

}

CrochetTab::~CrochetTab()
{
}

QStringList CrochetTab::editModes()
{
    return mScene->modes();
}

int CrochetTab::editMode() const
{
    return (int) mScene->editMode();
}

void CrochetTab::setEditMode(int mode)
{
    mScene->setEditMode((Scene::EditMode)mode);
}

void CrochetTab::renderChart(QPainter* painter, QRectF rect)
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

void CrochetTab::zoomChanged(int value)
{
    mView->zoomLevel(value);
}

void CrochetTab::updateZoomLevel(int percent)
{
    int value = 100;
    if(percent <= ui->zoom->maximum() && percent >= ui->zoom->minimum())
        value = percent;
    else if(percent > ui->zoom->maximum())
        value = ui->zoom->maximum();
    else if (percent < ui->zoom->minimum())
        value = ui->zoom->minimum();

    ui->zoom->blockSignals(true);
    ui->zoom->setValue(value);
    ui->zoom->blockSignals(false);
}

QUndoStack* CrochetTab::undoStack()
{
    return mScene->undoStack();
}

void CrochetTab::createChart(int rows, int cols, QString defStitch, QSizeF rowSize)
{
    //mScene->createChart(rows, cols, defStitch, rowSize);

    SceneRounds* rounds = static_cast<SceneRounds*>(mScene);
    if(rounds) {
        ui->showChartCenter->setChecked(rounds->showChartCenter());
    }

    mRowEditDialog->updateRowList();
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
    QClipboard* clipboard = QApplication::clipboard();

    QString instructions = mTextView->copyInstructions();
    clipboard->setText(instructions);
    
}

void CrochetTab::showChartOptions()
{
    if(ui->moreBttn->text() == tr("&More >>")) {
        ui->moreBttn->setText(tr("&Less <<"));
        ui->chartOptionsBox->setVisible(true);
    } else {
        ui->moreBttn->setText(tr("&More >>"));
        ui->chartOptionsBox->setVisible(false);
    }

    SceneRounds* r = static_cast<SceneRounds*>(mScene);
    if(r)
        ui->showChartCenter->setEnabled(true);
    else
        ui->showChartCenter->setEnabled(false);
}

void CrochetTab::setShowChartCenter(bool state)
{
    ui->showChartCenter->setChecked(state);

    SceneRounds* r = static_cast<SceneRounds*>(mScene);
    if(r)
        r->setShowChartCenter(state);

    emit tabModified(true);
}

void CrochetTab::sceneUpdate()
{
    mScene->update();
}

void CrochetTab::clearSelection()
{

    mScene->clearSelection();
    mScene->update();

}

void CrochetTab::showRowEditor(bool state)
{
    if(state) {
        mRowEditDialog->show();
    } else {
        mRowEditDialog->hide();
    }
    
    mScene->clearSelection();
}

void CrochetTab::updateRows()
{
    mRowEditDialog->updateRowList();
}

void CrochetTab::alignSelection(int alignmentStyle)
{
    mScene->alignSelection(alignmentStyle);   
}

void CrochetTab::distributeSelection(int distributionStyle)
{
    mScene->distributeSelection(distributionStyle);
}

void CrochetTab::arrangeGrid(QSize grid, QSize alignment, QSize spacing, bool useSelection)
{
    mScene->arrangeGrid(grid, alignment, spacing, useSelection);
}

void CrochetTab::mirror(int direction)
{
    mScene->mirror(direction);
}

void CrochetTab::rotate(qreal degrees)
{
    mScene->rotate(degrees);
}

void CrochetTab::copy()
{
    mScene->copy();
}

void CrochetTab::cut()
{
    mScene->cut();
}

void CrochetTab::paste()
{
    mScene->paste();
}

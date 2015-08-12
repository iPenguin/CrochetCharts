/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#include "crochettab.h"
#include "ui_optionsbar.h"

#include <QVBoxLayout>
#include <QSplitter>

#include <QPainter>

#include <QXmlStreamWriter>

#include <QDebug>

#include "scene.h"
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
    mScene = new Scene(mView);

    mScene->setSceneRect(-2500,-2500, 5000,5000);
    centerOn = QPoint(0, 0);

    mTextView = new TextView(this, mScene);

    connect(mView, SIGNAL(scrollBarChanged(int,int)), mScene, SLOT(updateRubberBand(int,int)));
    
    connect(mScene, SIGNAL(stitchChanged(QString,QString)), SLOT(stitchChanged(QString,QString)));
    connect(mScene, SIGNAL(colorChanged(QString,QString)), SLOT(colorChanged(QString,QString)));
    connect(mScene, SIGNAL(rowEdited(bool)), SIGNAL(tabModified(bool)));
    connect(mScene, SIGNAL(guidelinesUpdated(Guidelines)), SIGNAL(guidelinesUpdated(Guidelines)));

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
    ui->verticalLayout->insertWidget(0, mRowEditDialog);
    mRowEditDialog->hide();
   
    connect(ui->copyInstructions, SIGNAL(clicked()), SLOT(copyInstructions()));

    connect(ui->zoom, SIGNAL(valueChanged(int)), SLOT(zoomChanged(int)));
    connect(mView, SIGNAL(zoomLevelChanged(int)), SLOT(updateZoomLevel(int)));

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

void CrochetTab::renderChartSelected(QPainter* painter, QRectF rect)
{
	
    QRectF r = mScene->selectedItemsBoundingRect(mScene->selectedItems());
	//make all unselected items invisible
	QList<QGraphicsItem*> selected = mScene->selectedItems();
	foreach(QGraphicsItem* item, mScene->items()) {
		item->setVisible(false);
	}
	foreach(QGraphicsItem* item, selected) {
		item->setVisible(true);
	}
    mScene->render(painter, rect, r);

    mView->centerOn(r.center());
	
	//and make them visible once more
	foreach(QGraphicsItem* item, mScene->items()) {
		item->setVisible(true);
	}
}

void CrochetTab::renderChart(QPainter* painter, QRectF rect)
{

    QRectF r = mScene->itemsBoundingRect();
    mScene->render(painter, rect, r);

    mView->centerOn(r.center());
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

void CrochetTab::createChart(Scene::ChartStyle style, int rows, int cols, QString defStitch, QSizeF rowSize, int increaseBy)
{
    if(style == Scene::Rows) {
        mScene->createRowsChart(rows, cols, defStitch, rowSize);

        mView->centerOn(mView->mapFromScene(mScene->itemsBoundingRect().bottomRight()));
        QPointF pt = mScene->itemsBoundingRect().topLeft();
        mView->ensureVisible(pt.x(), pt.y(), 50, 50);
    } else if(style == Scene::Rounds) {
        mScene->createRoundsChart(rows, cols, defStitch, rowSize, increaseBy);
    } else if(style == Scene::Blank) {
        mScene->createBlankChart();
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

void CrochetTab::setShowChartCenter(bool state)
{

    mScene->setShowChartCenter(state);

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

void CrochetTab::replaceStitches(QString original, QString replacement)
{
    mScene->replaceStitches(original, replacement);
}

void CrochetTab::replaceColor(QColor original, QColor replacement, int selection)
{
    mScene->replaceColor(original, replacement, selection);
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

void CrochetTab::copy(int direction)
{
    mScene->copy(direction);
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

void CrochetTab::group()
{
    mScene->group();
}

void CrochetTab::ungroup()
{
    mScene->ungroup();
}

bool CrochetTab::hasChartCenter()
{
    return mScene->showChartCenter();
}

void CrochetTab::setChartCenter(bool state)
{
    mScene->setShowChartCenter(state);
}

bool CrochetTab::hasGuidelines()
{
    if(mScene->guidelines().type() != tr("None"))
        return true;
    return false;
}

void CrochetTab::propertiesUpdate(QString property, QVariant newValue)
{
    mScene->propertiesUpdate(property, newValue);
}

void CrochetTab::updateDefaultStitchColor(QColor originalColor, QColor newColor)
{
    mScene->updateDefaultStitchColor(originalColor, newColor);
}

QList<QGraphicsItem*> CrochetTab::selectedItems()
{

    return mScene->selectedItems();
}

void CrochetTab::setGuidelinesType(QString guide)
{
    mScene->setGuidelinesType(guide);
}


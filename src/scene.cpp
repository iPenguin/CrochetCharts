/****************************************************************************\
 C opyright (c) 2010-2014 Stitch Works Software                                 *
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
#include "scene.h"

#include "cell.h"

#include <QFontMetrics>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneEvent>
#include <QApplication>
#include <QClipboard>

#include <math.h>

#include "debug.h"

#include "chartview.h"

#include "settings.h"
#include "stitchset.h"
#include "appinfo.h"
#include "crochetchartcommands.h"
#include "indicatorundo.h"
#include <QUrl>
#include <QKeyEvent>
#include "stitchlibrary.h"

#include <QTextCursor>
#include <QAction>
#include <QMenu>
#include <QVector2D>

#include "ChartItemTools.h"

#include "guideline.h"

#ifndef M_PI
	# define M_PI	3.14159265358979323846
#endif

Guidelines::Guidelines()
    : mType("None"),
      mRows(Settings::inst()->value("rowCount").toInt()),
      mColumns(Settings::inst()->value("stitchCount").toInt()),
      mCellHeight(Settings::inst()->value("cellHeight").toInt()),
      mCellWidth(Settings::inst()->value("cellWidth").toInt())
{

}

QDataStream & operator<< ( QDataStream & stream, Guidelines & guidelines )
{

    stream << guidelines.type() << guidelines.rows() << guidelines.columns()
           << guidelines.cellHeight() << guidelines.cellWidth();
    return stream;
}

QDebug operator<< (QDebug d, Guidelines & guidelines)
{
   d << "Guidelines(" << guidelines.type() << guidelines.rows() << guidelines.columns()
     << guidelines.cellHeight() << guidelines.cellWidth() << ")";
   return d;
}

bool Guidelines::operator==(const Guidelines &other) const
{
    bool isEqual = true;

    if(type() != other.type())
        isEqual = false;

    if(rows() != other.rows())
        isEqual = false;
    if(columns() != other.columns())
        isEqual = false;

    if(cellHeight() != other.cellHeight())
        isEqual = false;
    if(cellWidth() != other.cellWidth())
        isEqual = false;

    return isEqual;
}

bool Guidelines::operator!=(const Guidelines &other) const
{
    bool isEqual = true;

    if(type() != other.type())
        isEqual = false;

    if(rows() != other.rows())
        isEqual = false;
    if(columns() != other.columns())
        isEqual = false;

    if(cellHeight() != other.cellHeight())
        isEqual = false;
    if(cellWidth() != other.cellWidth())
        isEqual = false;

    return !isEqual;
}

QDebug operator<<(QDebug d, IndicatorProperties & properties)
{
    d << "IndicatorProperties(" << properties.style() << properties.html() << ")";
    return d;
}

Scene::Scene(QObject* parent) :
    QGraphicsScene(parent),
    mCurItem(0),
    mCellStartPos(QPointF(0,0)),
    mLeftButtonDownPos(QPointF(0,0)),
    mCurIndicator(0),
    mDiff(QSizeF(0,0)),
    mRubberBandStart(QPointF(0,0)),
    mMoving(false),
    mIsRubberband(false),
    mHasSelection(false),
    mSnapTo(false),
	mMultiEdit(false),
    mMode(Scene::StitchEdit),
    mEditStitch("ch"),
    mEditFgColor(QColor(Qt::black)),
    mEditBgColor(QColor(Qt::white)),
    mOldScale(QPointF(1.0, 1.0)),
    mOldTransform(QTransform()),
    mOldSize(QSizeF(1,1)),
    mAngle(0.0),
    mOrigin(0,0),
    mRowSpacing(9),
    mDefaultSize(QSizeF(32.0, 96.0)),
    mDefaultStitch("ch"),
    mRowLine(0),
    mCenterSymbol(0),
    mShowChartCenter(false),
	mSnapAngle(false),
	mSelectedLayer(0),
	mSelectMode(BoxSelect),
	mSelectionBand(0),
	mbackgroundIsEnabled(true)
{
    mPivotPt = QPointF(mDefaultSize.width()/2, mDefaultSize.height());
	
}

Scene::~Scene()
{
	//clean up all layers when destroyed
	foreach (ChartLayer* layer, mLayers) {
		delete layer;
	}
}

QStringList Scene::modes()
{
    QStringList modes;
    modes << tr("Stitch Edit") << tr("Color Edit") << tr("Row Edit")
            << tr("Rotation Edit") << tr("Scale Edit") << tr("Indicator Edit");
    return modes;
}

Cell* Scene::cell(int row, int column)
{
    if(row >= grid.count())
        return 0;
    if(column >= grid[row].count())
        return 0;

    return grid[row][column];
}

Cell* Scene::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

int Scene::rowCount()
{
    return grid.count();
}

int Scene::columnCount(int row)
{
    if(row >= grid.count())
        return 0;

    return grid[row].count();
}

QList<ChartLayer*> Scene::layers()
{
	return mLayers.values();
}

QGraphicsItem* Scene::selectableItemAt(const QPointF& pos)
{
	foreach (QGraphicsItem* item, items()) {
		if (item->sceneBoundingRect().contains(pos) &&
				(item->flags() & QGraphicsItem::ItemIsSelectable) == QGraphicsItem::ItemIsSelectable)
			return item;
	}
	return NULL;
}

int Scene::maxColumnCount()
{
    int max = 0;
    for(int i = 0; i < rowCount(); ++i) {
        int cols = columnCount(i);
        if(cols > max)
            max = cols;
    }
    return max;
}

void Scene::addItem(QGraphicsItem* item)
{
    if(!item)
        return;
    
    switch(item->type()) {
        case Cell::Type: {
            QGraphicsScene::addItem(item);
            Cell* c = qgraphicsitem_cast<Cell*>(item);
            connect(c, SIGNAL(stitchChanged(QString,QString)), SIGNAL(stitchChanged(QString,QString)));
            connect(c, SIGNAL(colorChanged(QString,QString)), SIGNAL(colorChanged(QString,QString)));
            break;
        }
        case Indicator::Type: {
            QGraphicsScene::addItem(item);
            Indicator* i = qgraphicsitem_cast<Indicator*>(item);
            connect(i, SIGNAL(lostFocus(Indicator*)), SLOT(editorLostFocus(Indicator*)));
            connect(i, SIGNAL(gotFocus(Indicator*)), SLOT(editorGotFocus(Indicator*)));
            //TODO: add font selection & styling etc.
            //connect(i, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
            mIndicators.append(i);
            i->setText(QString::number(mIndicators.count()));
            break;
        }
        case ItemGroup::Type: {
            QGraphicsScene::addItem(item);
            ItemGroup* group = qgraphicsitem_cast<ItemGroup*>(item);
            mGroups.append(group);
            break;
        }
        default:
            WARN("Unknown type: " + QString::number(item->type()));
            //fall through
		case ChartImage::Type:
        case Guideline::Type:
        case QGraphicsEllipseItem::Type:
        case QGraphicsLineItem::Type: {
            QGraphicsScene::addItem(item);
            break;
        }
    }

}

void Scene::removeItem(QGraphicsItem* item)
{
    if(!item)
        return;
    
    switch(item->type()) {
        case Cell::Type: {
            QGraphicsScene::removeItem(item);
            Cell* c = qgraphicsitem_cast<Cell*>(item);
            removeFromRows(c);
            break;
        }
        case Indicator::Type: {
            QGraphicsScene::removeItem(item);
            Indicator* i = qgraphicsitem_cast<Indicator*>(item);
            mIndicators.removeOne(i);
            break;
        }
        case ItemGroup::Type: {
            QGraphicsScene::removeItem(item);
            ItemGroup* group = qgraphicsitem_cast<ItemGroup*>(item);
            mGroups.removeOne(group);
            break;
        }

        default:
            WARN("Unknown type: " + QString::number(item->type()));
		case ChartImage::Type:
        case Guideline::Type:
        case QGraphicsEllipseItem::Type:
        case QGraphicsLineItem::Type: {
            QGraphicsScene::removeItem(item);
            break;
        }
    }

}

void Scene::removeFromRows(Cell* c)
{
    for(int y = 0; y < grid.count(); ++y) {
        if(grid[y].contains(c)) {
            grid[y].removeOne(c);
            if(grid[y].count() == 0)
                grid.removeAt(y);
            c->setZValue(10);
            break;
        }
    }
}

void Scene::updateRubberBand(int dx, int dy)
{

    if(mRubberBandStart.isNull())
        return;

    mRubberBandStart.setX(mRubberBandStart.x() - dx);
    mRubberBandStart.setY(mRubberBandStart.y() - dy);
}

void Scene::contextMenuEvent(QGraphicsSceneContextMenuEvent* e)
{

    QMenu menu;
    QAction* copyAction = new QAction(tr("Copy"), 0);
    QAction* cutAction = new QAction(tr("Cut"), 0);
    QAction* pasteAction = new QAction(tr("Paste"), 0);
    QAction *deleteAction = new QAction(tr("Delete"), 0);

    connect(deleteAction, SIGNAL(triggered()), SLOT(deleteSelection()));
    connect(copyAction, SIGNAL(triggered()), SLOT(copy()));
    connect(cutAction, SIGNAL(triggered()), SLOT(cut()));
    connect(pasteAction, SIGNAL(triggered()), SLOT(paste()));
    
    menu.addAction(copyAction);
    menu.addAction(cutAction);
    menu.addAction(pasteAction);
    menu.addSeparator();
    menu.addAction(deleteAction);

    menu.exec(e->screenPos());

    e->accept();
}

void Scene::keyReleaseEvent(QKeyEvent* keyEvent)
{
    
    QGraphicsScene::keyReleaseEvent(keyEvent);
    
    if(keyEvent->isAccepted())
        return;

    if(keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
        deleteSelection();
    }

}

void Scene::keyPressEvent(QKeyEvent* keyEvent)
{

    QGraphicsScene::keyPressEvent(keyEvent);

    if(keyEvent->isAccepted())
        return;

    if(selectedItems().count() <= 0)
        return;
    
    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeKeyRelease(keyEvent);
            break;
        case Scene::RotationEdit:
            angleModeKeyRelease(keyEvent);
            break;
        case Scene::ScaleEdit:
            scaleModeKeyRelease(keyEvent);
            break;
        case Scene::RowEdit:
        default:
            break;
    }
    
}


void Scene::stitchModeKeyRelease(QKeyEvent* keyEvent)
{
    int deltaX = 0, deltaY = 0;
    
    if(keyEvent->key() == Qt::Key_Left) {
        deltaX = -1;
        keyEvent->accept();
    } else if (keyEvent->key() == Qt::Key_Right) {
        deltaX = 1;
        keyEvent->accept();
    } else if (keyEvent->key() == Qt::Key_Up) {
        deltaY = -1;
        keyEvent->accept();
    } else if (keyEvent->key() == Qt::Key_Down) {
        deltaY = 1;
        keyEvent->accept();
    }

    if(!keyEvent->isAccepted())
        return;
    
    undoStack()->beginMacro(tr("adjust item positions"));
    foreach(QGraphicsItem *i, selectedItems()) {
        QPointF oldPos = i->pos();
        i->setPos(i->pos().x() + deltaX, i->pos().y() + deltaY);
        undoStack()->push(new SetItemCoordinates(i, oldPos));
    }
    undoStack()->endMacro();
    
}

void Scene::angleModeKeyRelease(QKeyEvent* keyEvent)
{
	//TODO transform_refactor
    int delta = 0;

    if(keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Up) {
        delta = -1;
        keyEvent->accept();
    } else if (keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_Down) {
        delta = 1;
        keyEvent->accept();
    }

    if(!keyEvent->isAccepted())
        return;

    undoStack()->beginMacro(tr("adjust item rotation"));
    foreach(QGraphicsItem *i, selectedItems()) {
        if(i->type() != Cell::Type)
            continue;
        Cell* c = qgraphicsitem_cast<Cell*>(i);
        
        qreal oldAngle = ChartItemTools::getRotation(c);
		QPointF pvtPt = QPointF(c->boundingRect().width()/2, c->boundingRect().bottom());
        ChartItemTools::setRotationPivot(c, pvtPt);
        ChartItemTools::addRotation(c, delta);
		undoStack()->push(new SetItemRotation(c, oldAngle, pvtPt));
    }
    undoStack()->endMacro();

}

void Scene::scaleModeKeyRelease(QKeyEvent* keyEvent)
{
	//TODO transform_refactor
    QPointF delta(0.0, 0.0);
    
    if(keyEvent->key() == Qt::Key_Left) {
        delta.setX(-0.1);
        keyEvent->accept();
    } else if (keyEvent->key() == Qt::Key_Right) {
        delta.setX(0.1);
        keyEvent->accept();
    } else if (keyEvent->key() == Qt::Key_Up) {
        delta.setY(-0.1);
        keyEvent->accept();
    } else if (keyEvent->key() == Qt::Key_Down) {
        delta.setY(0.1);
        keyEvent->accept();
    }
	
	if (Settings::inst()->value("scaleAroundCenter").toBool() == true)
		delta.ry() = - delta.y();

    if(!keyEvent->isAccepted())
        return;

    //Keep in perportion:
    if(keyEvent->modifiers() == Qt::ControlModifier) {
        if(delta.x() != 0)
            delta.ry() = delta.x();
        else
            delta.rx() = delta.y();
    }
    
    undoStack()->beginMacro(tr("adjust item scale"));
    foreach(QGraphicsItem *i, selectedItems()) {
        if(i->type() != Cell::Type)
            continue;
        Cell* c = qgraphicsitem_cast<Cell*>(i);

        QPointF oldScale = ChartItemTools::getScale(c);
        QPointF newScale = oldScale + delta;
        ChartItemTools::setScaleX(c, newScale.x());
        ChartItemTools::setScaleY(c, newScale.y());
        undoStack()->push(new SetItemScale(c, oldScale, ChartItemTools::getScalePivot(i)));
    }
    undoStack()->endMacro();
    
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{

    if(selectedItems().count() > 0)
        mHasSelection = true;

    if(mHasSelection && e->modifiers() == Qt::ControlModifier)
        mSelectionPath = selectionArea();
	//
    if(e->buttons() & Qt::LeftButton &&(e->modifiers() != Qt::ShiftModifier || selectedItems().count() >= 1))
        QGraphicsScene::mousePressEvent(e);

	//FIXME: there has to be a better way to keep the current selection.
    //FIXME 2: I commented out the following two lines of codes, which fixed ctrl-selection. No idea why. Hope I didn't break anything else.
	//if(mHasSelection && e->modifiers() == Qt::ControlModifier)
    //    setSelectionArea(mSelectionPath);
    
    mLeftButtonDownPos = e->buttonDownPos(Qt::LeftButton);

    mMoving = false;
    mIsRubberband = false;
    
	//select an item unless we force a rubberband using shift
	if (e->modifiers() != Qt::ShiftModifier)
		mCurItem = selectableItemAt(e->scenePos());
	else
		mCurItem = NULL;
	//get the biggest group it is in
	
	if (mCurItem)
		while (mCurItem->parentItem() != NULL) {
		if (mCurItem->parentItem()->type() == ItemGroup::Type)
			mCurItem = mCurItem->parentItem();
		else
			break;
		}
	
    if(mCurItem) {
        switch(mCurItem->type()) {
            case Cell::Type: {
                mCellStartPos = mCurItem->scenePos();
                mDiff = QSizeF(e->scenePos().x() - mCellStartPos.x(), e->scenePos().y() - mCellStartPos.y());
                break;
            }
            case Indicator::Type: {
                Indicator* i = qgraphicsitem_cast<Indicator*>(mCurItem);
            
                mCurIndicator = i;
                mCellStartPos = i->scenePos();
                break;
            }
            case QGraphicsEllipseItem::Type: {
                mMoving = true;
                break;
            }
            case ItemGroup::Type: {
                //mMoving = true;
                break;
            }
            case QGraphicsSimpleTextItem::Type: {
                //If we've selected the background text pretend we didn't select it.
                mCurItem = 0;
                break;
            }
            case Guideline::Type: {
                //We don't want to select the guidelines when we're manipulating the chart or stitches.
                clearSelection();
                mCurItem = 0;
                break;
            }
			case ChartImage::Type: {
				//mMoving = true;
				break;
			}
            default:
                qWarning() << "mousePress - Unknown object type: " << mCurItem->type();
                break;
        }
    }

    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeMousePress(e);
            break;

        case Scene::RotationEdit:
            angleModeMousePress(e);
            break;

        case Scene::ScaleEdit:
            scaleModeMousePress(e);
            break;

        case Scene::RowEdit:
            rowEditMousePress(e);
            //This is a special case. We don't want to
            //do any of the usual object editing/moving.
            return;

        default:
            break;
    }		

    if(e->buttons() != Qt::LeftButton)
        return;

    if(selectedItems().count() <= 0 || e->modifiers() == Qt::ControlModifier) {
        ChartView* view = qobject_cast<ChartView*>(parent());

        //if(!mRubberBand)
        //    mRubberBand = new QRubberBand(QRubberBand::Rectangle, view);
		
		if(!mSelectionBand) {
			if (mSelectMode == Scene::BoxSelect)
				mSelectionBand = new RubberBand(view);
			else if (mSelectMode == Scene::LassoSelect)
				mSelectionBand = new LassoBand(view);
		}

        mRubberBandStart = view->mapFromScene(e->scenePos());
		
        //mRubberBand->setGeometry(QRect(mRubberBandStart.toPoint(), QSize()));
        //mRubberBand->show();
		
		mSelectionBand->reset();
		mSelectionBand->setPosition(mRubberBandStart.toPoint());
		mSelectionBand->moveMouseTo(mRubberBandStart.toPoint());
		mSelectionBand->show();
    } else {
        
    //Track object movement on scene.
        foreach(QGraphicsItem* item, selectedItems()) {
            mOldPositions.insert(item, item->pos());
        }
    }

}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeMouseMove(e);
            break;
        case Scene::ColorEdit:
            colorModeMouseMove(e);
            break;
        case Scene::IndicatorEdit:
            indicatorModeMouseMove(e);
            break;
        case Scene::RotationEdit:
            angleModeMouseMove(e);
            break;
        case Scene::ScaleEdit:
            scaleModeMouseMove(e);
            break;
        case Scene::RowEdit:
            rowEditMouseMove(e);
            return;
        default:
            break;
    }

    qreal diff = (e->buttonDownScenePos(Qt::LeftButton)- e->scenePos()).manhattanLength();

    if(diff >= QApplication::startDragDistance()) {

        if(!mCurItem && !mMoving) {
            if(mSelectionBand) {
                QGraphicsView* view = views().first();
				
				mSelectionBand->setPosition(mRubberBandStart.toPoint());
				mSelectionBand->moveMouseTo(view->mapFromScene(e->scenePos()));
				
				mIsRubberband = true;
            }
        } else if (mMoving) {
            QGraphicsScene::mouseMoveEvent(e);
            if(selectedItems().contains(mCenterSymbol)) {
                updateGuidelines();
            }
        }
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsScene::mouseReleaseEvent(e);

    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeMouseRelease(e);
            break;
        case Scene::ColorEdit:
            colorModeMouseRelease(e);
            break;
        case Scene::IndicatorEdit:
            indicatorModeMouseRelease(e);
            break;
        case Scene::RotationEdit:
            angleModeMouseRelease(e);
            break;
        case Scene::ScaleEdit:
            scaleModeMouseRelease(e);
            break;
        case Scene::RowEdit:
            rowEditMouseRelease(e);
            break;
        default:
            break;
    }

    if(mIsRubberband && mSelectionBand) {
        ChartView* view = qobject_cast<ChartView*>(parent());
		
        QPainterPath path;
		path.addPath(view->mapToScene(mSelectionBand->path()));

        //if user is holding down ctrl add items to existing selection.
        if(mHasSelection && e->modifiers() == Qt::ControlModifier) {
            path.addPath(mSelectionPath);
        }
		blockSignals(true);
        setSelectionArea(path);
		blockSignals(false);
		emit selectionChanged();
		mSelectionBand->hide();
    }

    if((selectedItems().count() > 0 && mOldPositions.count() > 0) && mMoving) {
        undoStack()->beginMacro("move items");
		//first, snap the items to the grid if we need to
		foreach(QGraphicsItem* item, selectedItems()) {
			snapGraphicsItemToGrid(*item);
		}
		
        foreach(QGraphicsItem* item, selectedItems()) {
            if(mOldPositions.contains(item)) {
                QPointF oldPos = mOldPositions.value(item);
                undoStack()->push(new SetItemCoordinates(item, oldPos));
            }
        }
        undoStack()->endMacro();
        mOldPositions.clear();
    }

    mDiff = QSizeF(0,0);
    mCellStartPos = QPointF(0,0);

    if(mCurIndicator) {
        mCurIndicator = 0;
    }

    mLeftButtonDownPos = QPointF(0,0);

    if (mSelectionBand) {
       delete mSelectionBand;
        mSelectionBand = 0;
    }

    if(mMoving) {
        //update the size of the scene rect based on where the items are on the scene.
        updateSceneRect();

        mMoving = false;
    }

    mCurItem = 0;
    mHasSelection = false;
}

void Scene::snapGraphicsItemToGrid(QGraphicsItem& item)
{
	QPointF centerPos = item.sceneBoundingRect().center();
	item.setPos(snapPositionToGrid(centerPos) - centerPos + item.pos());
	
	//if we need to snap the angle and we are in rounds mode
	if (mSnapAngle && mGuidelines.type() == "Rounds") {
		//get the angle between the item and the center
		QPointF center(0, 0);
		if (mCenterSymbol)
			center = mCenterSymbol->pos();
			
		QPointF newCenterPos = item.sceneBoundingRect().center();
		qreal angle = std::atan2(newCenterPos.y() - center.y(), newCenterPos.x() - center.x());

		//rotate the object so it looks at the center
		ChartItemTools::setRotationPivot(&item, item.boundingRect().center());
		ChartItemTools::setRotation(&item, (angle * 180/M_PI) + 90);
	}
}

QPointF Scene::snapPositionToGrid(const QPointF& pos) const
{
	if (mGuidelines.type() == "None")
		return pos;
	else if (mGuidelines.type() == "Rows")
		return snapPositionToRows(pos);
	else if (mGuidelines.type() == "Rounds")
		return snapPositionToRounds(pos);
	else if (mGuidelines.type() == "Triangles")
		return snapPositionToTriangles(pos);
	else {
		return pos;
		WARN("Unknown guideline type.");
		WARN(mGuidelines.type());
	}
}

QPointF Scene::snapPositionToTriangles(const QPointF& pos) const
{
	//get the needed variables
	QPointF center(0, 0);
	if (mCenterSymbol)
		center = mCenterSymbol->pos();
		
    int triangles = mGuidelines.rows();
    
    int spacingW = mGuidelines.cellWidth();
    int spacingH = mGuidelines.cellHeight();
	
	//get the relative position to the triangles
	QPointF relPos = pos - center;
	
	//get the row of the current position
	int relRow = relPos.y() / spacingH + 0.5;
	
	//clamp the row
	relRow = std::max(0, std::min(triangles, relRow));
	
	//get the max offsetX of that row
	qreal offsetX = (relRow) * spacingW/2;
	
	//get the distance from that offset
	qreal offsetDist = relPos.x() - offsetX;
	
	//snap that distance to the grid
	int relDistColumn = offsetDist / spacingW - 0.5;
	
	//clamp the column
	relDistColumn = std::max(-relRow, std::min(0, relDistColumn));
	
	//get the new relative pos
	QPointF newPos(relDistColumn * spacingW + offsetX, relRow * spacingH);
	
	return newPos + center;
}

QPointF Scene::snapPositionToRows(const QPointF& pos) const
{
	//get the needed variables
	QPointF center(0, 0);
	if (mCenterSymbol)
		center = mCenterSymbol->pos();
		
    int columns = mGuidelines.columns();
    int rows = mGuidelines.rows();

    int spacingW = mGuidelines.cellWidth();
    int spacingH = mGuidelines.cellHeight();

	//get the relative position to the grid
	QPointF relPos = pos - center;
	
	//get the grid pos of the point
	int relPosGridX = (relPos.x() + spacingW/2) / spacingW;
	int relPosGridY = (relPos.y() + spacingH/2) / spacingH;
	
	//clamp these the the grid size
	relPosGridX = std::max(0, std::min(columns, relPosGridX));
	relPosGridY = std::max(0, std::min(rows, relPosGridY));
	
	//get the new relative position
	QPointF snappedRelPos(relPosGridX * spacingW, relPosGridY * spacingH);

	//return it as scene coordinates
	return snappedRelPos + center;
}

QPointF Scene::snapPositionToRounds(const QPointF& pos) const
{
	//get the needed variables
	QPointF center(0, 0);
	if (mCenterSymbol)
		center = mCenterSymbol->pos();
		
    int columns = mGuidelines.columns();
    int rounds = mGuidelines.rows();
	
    int spacingH = mGuidelines.cellHeight();
	
	//get the relative position to the circle
	QPointF relPos = pos - center;
	
	//get the ring of the position
	qreal distanceToCenter = sqrt(relPos.x()*relPos.x() + relPos.y()*relPos.y()) + spacingH/2;
	int relPosRing = distanceToCenter / spacingH;
	relPosRing = std::max(0, std::min(rounds + 1, relPosRing));
	qreal relPosSnappedDistance = relPosRing*spacingH;
	
	//get the quadrant of the position
	qreal PIPI = M_PI * 2;
	qreal quadrantAngleSize = PIPI/columns;
	qreal relPosAngle = atan2(relPos.y(), relPos.x());
	
	relPosAngle -= M_PI_2;
	
	//we normalize the angle
	relPosAngle = fmod(relPosAngle,PIPI);
    if (relPosAngle < 0)
        relPosAngle += PIPI;
    
	int relPosQuadrant = (relPosAngle) / quadrantAngleSize + 0.5;
	qreal relPosSnappedAngle = relPosQuadrant * quadrantAngleSize + M_PI_2;
	
	//convert it to a point on the circle and as scene coordinates
	return QPointF(
		cos(relPosSnappedAngle) * relPosSnappedDistance,
		sin(relPosSnappedAngle) * relPosSnappedDistance
		) + center;
}

void Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    mCurItem = itemAt(e->scenePos());

    if(!mCurItem)
        return;

    if(mCurItem->type() == Indicator::Type) {
        Indicator *ind = qgraphicsitem_cast<Indicator*>(mCurItem);
        ind->setTextInteractionFlags(Qt::TextEditorInteraction);
    }

    //accept the event so that no one else tries to use it.
    e->accept();
}

void Scene::colorModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    if(!mCurItem || mCurItem->type() != Cell::Type)
        return;

    mMoving = false;
/*
 *FIXME: if the user isn't dragging a stitch we should be painting with color.
    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellBgColor(mCurCell, mEditBgColor));
*/
}

void Scene::colorModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    if(!mCurItem || mCurItem->type() != Cell::Type)
        return;

    Cell* curCell = static_cast<Cell*>(mCurItem);
    undoStack()->beginMacro("set cell color");
    if(curCell->color() != mEditFgColor) {
        undoStack()->push(new SetCellColor(curCell, mEditFgColor));
    }
    if(curCell->bgColor() != mEditBgColor) {
        undoStack()->push(new SetCellBgColor(curCell, mEditBgColor));
    }
    undoStack()->endMacro();
}

void Scene::indicatorModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    if(!mCurItem)
        return;

    if(mCurItem->type() == Cell::Type || mCurItem->type() == Indicator::Type) {
        mMoving = true;
        e->accept();
    }

}

void Scene::indicatorModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    if(e->isAccepted())
        return;

    if((e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)) {
        if(mCurIndicator) {
            undoStack()->push(new RemoveIndicator(this, mCurIndicator));
        }
        return;
    }

    //if we're moving another indicator we shouldn't be creating a new one.
    if(mMoving) {
        return;
    }

    if(!mCurItem && !mHasSelection && !mIsRubberband) {

        QPointF pt = e->buttonDownScenePos(Qt::LeftButton);
        //FIXME: dont hard code the offset for the indicator.
        pt = QPointF(pt.x() - 10, pt.y() - 10);

        undoStack()->push(new AddIndicator(this, snapPositionToGrid(pt)));

    } else {
        if(mCurIndicator)
            mCurIndicator->setTextInteractionFlags(Qt::TextEditorInteraction);
    }

}

qreal Scene::scenePosToAngle(QPointF pt)
{

    qreal rads = atan2(pt.x(), pt.y());
    qreal angleX = rads * 180 / M_PI;

    return angleX;
}

void Scene::angleModeMousePress(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    if(!mCurItem)
        return;

    //If the item is group we want to rotate the whole group.
    if (mCurItem->parentItem())
        mCurItem = mCurItem->parentItem();
	
	if (selectedItems().count() > 1) {
		undoStack()->beginMacro("Rotate multiple items.");
		GroupItems* g = new GroupItems(this, selectedItems());
		undoStack()->push(g);
		mCurItem = g->group();
		mMultiEdit = true;
	}
	
    mOldAngle = ChartItemTools::getRotation(mCurItem);	
	QPointF localPivot;
	
	if (Settings::inst()->value("rotateAroundCenter").toBool() == true) {
		localPivot = mCurItem->boundingRect().center();
	} else {
	    localPivot = QPointF(mCurItem->boundingRect().center().x(),
                   mCurItem->boundingRect().bottom());
	}
	
	mPivotPt = ChartItemTools::mapToScale(mCurItem, localPivot);
    mOrigin = mCurItem->mapToScene(localPivot);
}

void Scene::angleModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    if(!mCurItem)
        return;

    //FIXME: don't 'move' stitches if multple are selected. rotate them.
    if(selectedItems().count() > 1) {
        mMoving = true;
        return;
    }
	
	//if we have the center selected, move it
	if (mCurItem->type() == QGraphicsEllipseItem::Type) {
        mMoving = true;
        return;
	}

    mMoving = false;

    QPointF first = e->buttonDownScenePos(Qt::LeftButton);
    QPointF second = e->scenePos();

    QPointF rel1 = first - mOrigin;
    QPointF rel2 = second - mOrigin;
    qreal angle1 = scenePosToAngle(rel1);
    qreal angle2 = scenePosToAngle(rel2);

    qreal angle = mOldAngle + (angle1 - angle2);

    qreal diff = fmod(angle, 45.0);
    qreal comp = fabs(diff);
    if(comp < 4 /*&& !mSnapTo*/) {
        qreal div = angle - diff;
        angle = div;
    }

    qNormalizeAngle(angle);
	
	SetItemRotation::setRotation(mCurItem, angle, mPivotPt);
}

void Scene::angleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    if(!mCurItem)
        return;

    if(mMoving)
        return;
	
    undoStack()->push(new SetItemRotation(mCurItem, mOldAngle, mPivotPt));
		
	if (mMultiEdit) {
		ungroup();
		undoStack()->push(new UngroupItems(this, (ItemGroup*)mCurItem));
		undoStack()->endMacro();
		emit selectionChanged();
		mMultiEdit = false;
	}
	
    mOldAngle = 0;
}

void Scene::scaleModeMousePress(QGraphicsSceneMouseEvent *e)
{

    Q_UNUSED(e);
    if(!mCurItem)
        return;

    //If the item is grouped we want to scale the whole group.
    if (mCurItem->parentItem()) {
        mCurItem = mCurItem->parentItem();
	}
	
	if (selectedItems().count() > 1) {
		undoStack()->beginMacro("Rotate multiple items.");
		GroupItems* g = new GroupItems(this, selectedItems());
		undoStack()->push(g);
		mCurItem = g->group();
		mMultiEdit = true;
	}
	
	qDebug() << "transform " << mCurItem->transform();
	
	//mOldscale should always be set, even when editing groups, else it will reset every scaling
	mOldScale = QPointF(ChartItemTools::getScaleX(mCurItem), ChartItemTools::getScaleY(mCurItem));
	mOldSize = mCurItem->boundingRect().size();
	QPointF localPivot;
	if (Settings::inst()->value("scaleAroundCenter").toBool() == true) {
		localPivot = mCurItem->boundingRect().center();
	} else {
		localPivot = mCurItem->boundingRect().topLeft();
	}
	
	mPivotPt = localPivot;
    mOrigin = mCurItem->mapToScene(localPivot);
}

void Scene::scaleModeMouseMove(QGraphicsSceneMouseEvent *e)
{

    if(!mCurItem)
        return;

    if(selectedItems().count() > 1) {
        mMoving = true;
        return;
    }
	
	//if we have the center selected, move it
	if (mCurItem->type() == QGraphicsEllipseItem::Type) {
        mMoving = true;
        return;
	}
	
	QPointF delta = e->scenePos() - e->buttonDownScenePos(Qt::LeftButton);
	QPointF scenePivot = ChartItemTools::getScalePivot(mCurItem) + mCurItem->pos();
	QSizeF originalSize = mCurItem->boundingRect().size();
	
	QSizeF currentSize = QSizeF(originalSize.width() * mOldScale.x(),
								originalSize.height() * mOldScale.y());
	
	QSizeF newSize = QSizeF(currentSize.width() + delta.x(), currentSize.height() + delta.y());
	
	QPointF neededScale = QPointF(newSize.width() / originalSize.width(),
								  newSize.height() / originalSize.height());
	
    //When holding Ctrl or when scaling a group, lock scale to largest dimension.
    if(e->modifiers() == Qt::ControlModifier || mCurItem->type() == ItemGroup::Type) {
        if(neededScale.x() < neededScale.y())
            neededScale.ry() = neededScale.rx();
        else
            neededScale.rx() = neededScale.ry();
    }
	qDebug() << "Setting scale " << neededScale.x() << " " << neededScale.y();
	SetItemScale::setScale(mCurItem, neededScale, mPivotPt);
}

void Scene::scaleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{

    Q_UNUSED(e);
    if(mMoving)
        return;

    if(!mCurItem)
        return;

    undoStack()->push(new SetItemScale(mCurItem, mOldScale, mPivotPt));

	if (mMultiEdit) {
		ungroup();
		undoStack()->push(new UngroupItems(this, (ItemGroup*)mCurItem));
		undoStack()->endMacro();
		emit selectionChanged();
		mMultiEdit = false;
	}

    mOldScale.setX(1.0);
    mOldScale.setY(1.0);
}

void Scene::rowEditMousePress(QGraphicsSceneMouseEvent* e)
{

    if(!e->buttons() == Qt::LeftButton)
        return;


    QGraphicsItem* gi = itemAt(e->scenePos());
    mStartCell = qgraphicsitem_cast<Cell*>(gi);
    if(mStartCell) {

        if(mRowSelection.contains(gi) && mRowSelection.last() == gi) {
            // remove the last stitch when updating the row list, 
            //because we're going to add it again below
            mRowSelection.removeLast();
        } else {
            mRowSelection.clear();
            hideRowLines();
            delete mRowLine;
            mRowLine = 0;
        }
        mRowSelection.append(mStartCell);

        mRowLine = addLine(QLineF(e->scenePos(), e->scenePos()));
        mRowLine->setPen(QPen(QColor(Qt::red), 2));
        
    }
    mPreviousCell = mStartCell;

}

void Scene::rowEditMouseMove(QGraphicsSceneMouseEvent* e)
{
    
    if(!e->buttons() == Qt::LeftButton)
        return;

    mMoving = false;

    if(!mStartCell)
        return;

    QPointF startPt = mRowLine->line().p1();
    
    QGraphicsItem* gi = itemAt(e->scenePos());
    if(gi) {
        Cell* c = qgraphicsitem_cast<Cell*>(gi);
        if(!c)
            return;
        if(!mRowSelection.contains(gi)) {
            mRowSelection.append(gi);
            gi->setSelected(true);

            if(mPreviousCell != c) {
                QGraphicsLineItem* line = addLine(QLineF(mPreviousCell->scenePos(), c->scenePos()));
                line->setPen(QPen(QColor(Qt::black), 2));
                mRowLines.append(line);

                mPreviousCell = c;
                startPt = c->scenePos();
            }
            
        } //else remove from list

    }
    
    if(mRowLine != 0) {
        QLineF newLine(startPt, e->scenePos());
        mRowLine->setLine(newLine);
    }
    
}

void Scene::rowEditMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);

    if(selectedItems().count() <= 0)
        hideRowLines();
    else {
        emit rowEdited(true);
    }

    mStartCell = 0;
    
    if(mRowLine) {
        removeItem(mRowLine);
        delete mRowLine;
        mRowLine = 0;
    }
    
}

void Scene::stitchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    
}

void Scene::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurItem)
        return;
    
    qreal diff = (e->buttonDownScenePos(Qt::LeftButton)- e->scenePos()).manhattanLength();

    if(diff >= QApplication::startDragDistance())
        mMoving = true;
}

void Scene::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{

    if(e->isAccepted())
        return;

    if(mCurItem && e->modifiers() != Qt::ControlModifier) {
		
		Cell *cell = qgraphicsitem_cast<Cell*>(mCurItem);

        if(!cell)
            return;
		
		if (Settings::inst()->value("replaceStitchWithPress").toBool() == false) {
			return;
		}
		
        if(cell->name() != mEditStitch && !mMoving)
            undoStack()->push(new SetCellStitch(cell, mEditStitch));


    } else if(!mIsRubberband && !mMoving && !mHasSelection) {

        if(mCurItem) {
			return;
		}

        if(e->button() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {

            Cell *c = new Cell();

            undoStack()->push(new AddItem(this, c));
            c->setPos(e->scenePos());
            c->setStitch(mEditStitch);
            c->setColor(mEditFgColor);
            c->setBgColor(mEditBgColor);
			c->setLayer(getCurrentLayer()->uid());
			
			//center it on the mouse if that is enabled
			if (Settings::inst()->value("centerNewStitchOnMouse").toBool() == true)
				c->setPos(c->pos() - (c->sceneBoundingRect().center() - c->pos()));
			
			c->setVisible(getCurrentLayer()->visible());
			snapGraphicsItemToGrid(*c);
			updateSceneRect();
        }
    } else if (mCurItem) {
		mMoving = true;
	}
}

void Scene::createRow()
{
    
    if(selectedItems().count() <= 0)
        return;
    
    QList<Cell*> r;

    foreach(QGraphicsItem* i, mRowSelection) {
        Cell* c = qgraphicsitem_cast<Cell*>(i);
        removeFromRows(c);
        c->useAlternateRenderer((grid.count() % 2));
        r.append(c);
    }
    grid.append(r);

}

void Scene::updateRow(int row)
{
    if(selectedItems().count() <= 0)
        return;

    QList<Cell*> r;

    foreach(QGraphicsItem* i, mRowSelection) {
        Cell* c = qgraphicsitem_cast<Cell*>(i);
        removeFromRows(c);
        c->useAlternateRenderer((row % 2));
        c->setZValue(100);
        r.append(c);
    }

    grid.insert(row, r);
    
}

QPoint Scene::indexOf(Cell* c)
{
    for(int y = 0; y < grid.count(); ++y) {
        if(grid[y].contains(c)) {
            return QPoint(grid[y].indexOf(c), y);
        }
    }

    return QPoint(-1,-1);
}

void Scene::highlightRow(int row)
{

    if(row >= grid.count())
        return;

    clearSelection();
    mRowSelection.clear();

    for(int i = 0; i < grid[row].count(); ++i) {
        Cell* c = grid[row][i];
        if(c) {
            c->setSelected(true);
            mRowSelection.append(c);
        }
    }

    emit selectionChanged();
}

void Scene::moveRowDown(int row)
{
    QList<Cell*> r = grid.takeAt(row);
    grid.insert(row + 1, r);
    updateStitchRenderer();
}

void Scene::moveRowUp(int row)
{

    QList<Cell*> r = grid.takeAt(row);
    grid.insert(row - 1, r);
    updateStitchRenderer();
    
}

void Scene::removeRow(int row)
{

    QList<Cell*> r = grid.takeAt(row);

    foreach(Cell* c, r) {
        c->useAlternateRenderer(false);
    }

    updateStitchRenderer();

}

void Scene::updateStitchRenderer()
{

    for(int i = 0; i < grid.count(); ++i) {
        foreach(Cell* c, grid[i]) {
            if(!c) {
                WARN("cell doesn't exist but it's in the grid");
                continue;
            }
            c->useAlternateRenderer((i % 2));
        }
    }

}

void Scene::render(QPainter *painter, const QRectF &target, const QRectF &source,
                   Qt::AspectRatioMode aspectRatioMode)
{
    QFont originalFont = painter->font();
    originalFont.setPixelSize(10);
    originalFont.setBold(false);
    originalFont.setItalic(false);
    originalFont.setUnderline(false);
    painter->setFont(originalFont);
	
	setBackground(false);
    QGraphicsScene::render(painter, target, source, aspectRatioMode);
	setBackground(true);
}

void Scene::drawBackground(QPainter * painter, const QRectF & rect)
{
	//if we must draw a background
	if (mbackgroundIsEnabled) {
		//first make the screen grey
		painter->fillRect(rect, QColor(212,212,212));
		
		//get the sceneboundingrectangle intersected with the rect
		QRectF foreground = sceneRect().intersect(rect);
		
		//draw that in white
		painter->fillRect(foreground, QColor(255,255,255));
	} else {
		//just paint white
		painter->fillRect(rect, QColor(255,255,255));
	}
}

void Scene::setBackground(bool enabled)
{
	mbackgroundIsEnabled = enabled;
}

void Scene::drawRowLines(int row)
{
    if(grid.count() <= row)
        return;

    hideRowLines();

    QPointF start, end;

    int count = grid[row].count();

    QGraphicsItem* prev = grid[row].first();

    for(int i = 0; i < count; ++i) {
        QGraphicsItem* c = grid[row][i];
        if(!c)
            continue;

        if(prev != c) {
            start = prev->scenePos();
            end = c->scenePos();

            QGraphicsLineItem *line = addLine(QLineF(start, end));
            line->setPen(QPen(QColor(Qt::black), 2));
            mRowLines.append(line);

            prev = c;
        }
    }

}

void Scene::hideRowLines()
{
    if(mRowLines.count() > 0) {
        foreach(QGraphicsLineItem* i, mRowLines) {
            delete i;
        }
        mRowLines.clear();
    }
}

//FIXME: simplify the number of foreach loops?
void Scene::alignSelection(int alignmentStyle)
{
    if(selectedItems().count() <= 0)
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //left
    if(alignmentStyle == 1) {
        align(0,1);
        
    //center v
    } else if(alignmentStyle == 2) {
        align(0, 2);
        
    //right
    } else if(alignmentStyle == 3) {
        align(0, 3);
        
    //top
    } else if(alignmentStyle == 4) {
        align(1, 0);

    //center h
    } else if(alignmentStyle == 5) {
        align(2, 0);

    //bottom
    } else if(alignmentStyle == 6) {
        align(3, 0);

    //to path
    } else if(alignmentStyle == 7) {
        alignToPath();

    }

    QApplication::restoreOverrideCursor();
}

void Scene::distributeSelection(int distributionStyle)
{

    if(selectedItems().count() <= 0)
        return;
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    //left
    if(distributionStyle == 1) {
        distribute(0, 1);

    //center v
    } else if(distributionStyle == 2) {
        distribute(0, 2);

    //right
    } else if(distributionStyle == 3) {
        distribute(0, 3);

    //top
    } else if(distributionStyle == 4) {
        distribute(1, 0);

    //center h
    } else if(distributionStyle == 5) {
        distribute(2, 0);
        
    //bottom
    } else if(distributionStyle == 6) {
        distribute(3, 0);

    //to path
    } else if(distributionStyle == 7) {
        distributeToPath();

    }

    QApplication::restoreOverrideCursor();
}

void Scene::align(int vertical, int horizontal)
{
    //TODO: break up function into manageable pieces.
    
    //Use the opposite extremes and walk over to the correct placement.
    qreal left = sceneRect().right();
    qreal right = sceneRect().left();
    qreal top = sceneRect().bottom();
    qreal bottom = sceneRect().top();
    
    foreach(QGraphicsItem* i, selectedItems()) {
        qreal tmpLeft, tmpTop;

        tmpLeft = i->sceneBoundingRect().left();
        tmpTop = i->sceneBoundingRect().top();

        if(tmpLeft < left) {
            left = tmpLeft;
        }
        
        if(i->sceneBoundingRect().right() > right) {
            right = i->sceneBoundingRect().right();    
        }
        
        if(tmpTop < top) {
            top = tmpTop;
        }
        
        if(i->sceneBoundingRect().bottom() > bottom) {
            bottom = i->sceneBoundingRect().bottom();
        }
        
    }

    qreal diff = right - left;
    qreal centerH = left + (diff / 2);
    diff = bottom - top;
    qreal centerV = top + (diff / 2);
    
    qreal baseX = 0;
    qreal baseY = 0;
    
    if(horizontal == 1)
        baseX = left;
    else if(horizontal == 2)
        baseX = centerH;
    else if(horizontal == 3)
        baseX = right;

    if(vertical == 1)
        baseY = top;
    else if(vertical == 2)
        baseY = centerV;
    else if(vertical == 3)
        baseY = bottom;
    
    undoStack()->beginMacro("align selection");
    foreach(QGraphicsItem* i, selectedItems()) {
        QPointF oldPos = i->pos();
        qreal newX = baseX;
        qreal newY = baseY;
        
        if(horizontal == 0) {
            newX = oldPos.x();
        } else if (horizontal == 1) {
            newX = i->pos().x() + (newX - i->sceneBoundingRect().x());
        } else if(horizontal == 2) {
            newX = i->pos().x() + ((newX - i->sceneBoundingRect().x()) - i->sceneBoundingRect().width()/2);
        } else if(horizontal == 3) {
            newX = i->pos().x() + ((newX - i->sceneBoundingRect().x()) - i->sceneBoundingRect().width());
        }

        if(vertical == 0) {
            newY = oldPos.y();
        } else if (vertical == 1) {
            newY = i->pos().y() + (newY - i->sceneBoundingRect().y());
        } else if(vertical == 2) {
            newY = i->pos().y() + ((newY - i->sceneBoundingRect().y()) - i->sceneBoundingRect().height()/2);
        } else if(vertical == 3) {
            newY = i->pos().y() + ((newY - i->sceneBoundingRect().y()) - i->sceneBoundingRect().height());
        }
        
        i->setPos(newX, newY);
        undoStack()->push(new SetItemCoordinates(i, oldPos));
        
    }
    undoStack()->endMacro();
    
}

void Scene::updateGuidelines()
{
    int columns = mGuidelines.columns();
    int rows = mGuidelines.rows();

    int spacingW = mGuidelines.cellWidth();
    int spacingH = mGuidelines.cellHeight();

    QGraphicsItem *i = 0;

    if(mGuidelinesLines.count() > 0) {
        foreach(QGraphicsItem *item, mGuidelinesLines) {
            removeItem(item);
            mGuidelinesLines.removeOne(item);
            delete item;
        }
    }

    if(mGuidelines.type() == "None")
        return;
	
	//get the center position, or make it 0 if centerpos does not exist yet
	QPointF center;
	if (mCenterSymbol)
		center = mCenterSymbol->pos();
	else
		center = QPointF(0,0);
	
	//create the graphical representation
	if(mGuidelines.type() == "Rows") {
		generateGuidelinesRows(spacingW, spacingH, columns, rows, center);
	} else if (mGuidelines.type() == "Rounds") {
		generateGuidelinesRounds(spacingW, spacingH, columns, rows, center);
	} else if (mGuidelines.type() == "Triangles") {
		generateGuidelinesTriangles(spacingW, spacingH, columns, rows, center);
	}

    updateSceneRect();
}

void Scene::generateGuidelinesRows(int spacingW, int spacingH, int columns, int rows, QPointF center)
{
    QGraphicsItem *i = 0;

	//generate columns
	for(int c = 0; c <= columns; c++) {
		i = addLine(
			c*spacingW + center.x(),
			center.y(),
			c*spacingW + center.x(),
			spacingH*rows + center.y()
		);
		i->setZValue(-1);
		mGuidelinesLines.append(i);
    }
	//generate rows
    for(int r = 0; r <= rows; r++) {
		 i = addLine(
			center.x(),
			center.y() + r*spacingH,
			center.x() + spacingW*columns,
			center.y() + r*spacingH
		);
		
		i->setZValue(-1);
		mGuidelinesLines.append(i);
    }
}

void Scene::generateGuidelinesRounds(int spacingW, int spacingH, int columns, int rows, QPointF center)
{
    QGraphicsItem *i = 0;

	//generate dividing lines
	for(int c = 0; c <= columns; c++) {
        qreal radians = (360.0 / columns * c) * M_PI / 180;
		qreal outterX = center.x() + (spacingH * (rows + 1)) /*distance*/ * sin(radians);
		qreal outterY = center.y() + (spacingH * (rows + 1)) /*distance*/ * cos(radians);

		qreal innerX = center.x() + spacingH * sin(radians);
		qreal innerY = center.y() + spacingH * cos(radians);

		i = addLine(innerX,innerY,outterX,outterY);
		i->setZValue(-1);
		mGuidelinesLines.append(i);
    }
	
	//generate circles
    for(int r = 0; r <= rows; r++) {
	   Guideline *g = new Guideline(
			QRectF(
				center.x() + -(r+1)*spacingH,
				center.y() + -(r+1)*spacingH,
				2*(r+1)*spacingH,
				2*(r+1)*spacingH)
			,0
			,this
		)
		;
		g->setZValue(-1);
		mGuidelinesLines.append(g);
    }
}

void Scene::generateGuidelinesTriangles(int spacingW, int spacingH, int columns, int rows, QPointF center)
{
	Q_UNUSED(columns);
    QGraphicsItem *i = 0;
	
	//generate the horizontal lines
	for(int r = 0; r < rows; r++) {
        qreal height = (r+1) * spacingH;
		qreal offsetX = (r+1) * spacingW/2;

		i = addLine(-offsetX + center.x(),height + center.y(),offsetX + center.x(),height + center.y());
		i->setZValue(-1);
		mGuidelinesLines.append(i);
    }
	
	//generate the slanted lines
	qreal maxX = (rows) * spacingW/2;
	qreal maxY = (rows) * spacingH;
	for(int c = 0; c < rows; c++) {
		
		qreal startX = -maxX + ( 2 * c * (maxX / rows));
		qreal startY = maxY;
		qreal endX = maxX - ( (rows - c) * (maxX / rows));
		qreal endY = c * maxY / rows;
        
		i = addLine(startX + center.x(), startY + center.y(), endX + center.x(), endY + center.y());
		i->setZValue(-1);
		mGuidelinesLines.append(i);
		i = addLine(-startX + center.x(), startY + center.y(), -endX + center.x(), endY + center.y());
		i->setZValue(-1);
		mGuidelinesLines.append(i);
    }
}

QList<QGraphicsItem*> Scene::sortItemsHorizontally(QList<QGraphicsItem*> unsortedItems, int sortEdge)
{
    QList<QGraphicsItem*> sorted;
    
    foreach(QGraphicsItem *item, unsortedItems) {
        qreal edge = 0;
        if (sortEdge == 2) {
            edge = item->sceneBoundingRect().center().x();
        } else if (sortEdge == 3) {
            edge = item->sceneBoundingRect().right();
        } else { //sortEdge == 1 or any other un-acceptable value.
            edge = item->sceneBoundingRect().left();
        }

        //append the first item without question and move on to the second item.
        if(sorted.count() <= 0) {
            sorted.append(item);
            continue;
        }
            
        int sortedCount = sorted.count();
        for(int i = 0; i < sortedCount; ++i) {
            qreal curSortedEdge = 0;
            
            if(sortEdge == 2) {
                curSortedEdge = sorted.at(i)->sceneBoundingRect().center().x();
            } else if (sortEdge == 3) {
                curSortedEdge = sorted.at(i)->sceneBoundingRect().right();
            } else { //sortEdge == 1 or any other un-acceptable value.
                curSortedEdge = sorted.at(i)->sceneBoundingRect().left();
            }
            
            if(edge < curSortedEdge) {
                sorted.insert(i, item);
                break;
            } else if(i + 1 == sortedCount) {
                sorted.append(item);
            }
        }
    }
    
    return sorted;
}

QList< QGraphicsItem* > Scene::sortItemsVertically(QList< QGraphicsItem* > unsortedItems, int sortEdge)
{
    QList<QGraphicsItem*> sorted;
    
    foreach(QGraphicsItem *item, unsortedItems) {
        qreal edge = 0;
        if (sortEdge == 2) {
            edge = item->sceneBoundingRect().center().y();
        } else if (sortEdge == 3) {
            edge = item->sceneBoundingRect().bottom();
        } else { //sortEdge == 1 or any other un-acceptable value.
            edge = item->sceneBoundingRect().top();
        }
        
        //append the first item without question and move on to the second item.
        if(sorted.count() <= 0) {
            sorted.append(item);
            continue;
        }
        
        int sortedCount = sorted.count();
        for(int i = 0; i < sortedCount; ++i) {
            qreal curSortedEdge = 0;
            
            if(sortEdge == 2) {
                curSortedEdge = sorted.at(i)->sceneBoundingRect().center().y();
            } else if (sortEdge == 3) {
                curSortedEdge = sorted.at(i)->sceneBoundingRect().bottom();
            } else { //sortEdge == 1 or any other un-acceptable value.
                curSortedEdge = sorted.at(i)->sceneBoundingRect().top();
            }
            
            if(edge < curSortedEdge) {
                sorted.insert(i, item);
                break;
            } else if(i + 1 == sortedCount) {
                sorted.append(item);
            }
        }
    }
    
    return sorted;
}

QRectF Scene::selectionBoundingRect(QList< QGraphicsItem* > items, int vertical, int horizontal) 
{
    qreal left = sceneRect().right();
    qreal right = sceneRect().left();
    qreal top = sceneRect().bottom();
    qreal bottom = sceneRect().top();
    
    QRectF topLeft(QPointF(left, top), QPointF(right, bottom)), 
           center(QPointF(left, top), QPointF(right, bottom)), 
           bottomRight(QPointF(left, top), QPointF(right, bottom)),
           final;
    
    foreach(QGraphicsItem *i, items) {
        //Set the left edges of the 3 possible Rects.
        if(i->sceneBoundingRect().left() < topLeft.left())
            topLeft.setLeft(i->sceneBoundingRect().left());
        if(i->sceneBoundingRect().center().x() < center.left())
            center.setLeft(i->sceneBoundingRect().center().x());
        if(i->sceneBoundingRect().right() < bottomRight.left())
            bottomRight.setLeft(i->sceneBoundingRect().right());
        
        //Set the right edges of the 3 possible Rects.
        if(i->sceneBoundingRect().left() > topLeft.right())
            topLeft.setRight(i->sceneBoundingRect().left());
        if(i->sceneBoundingRect().center().x() > center.right())
            center.setRight(i->sceneBoundingRect().center().x());
        if(i->sceneBoundingRect().right() > bottomRight.right())
            bottomRight.setRight(i->sceneBoundingRect().right());
        
        //Set the top edges of the 3 possible Rects.
        if(i->sceneBoundingRect().top() < topLeft.top())
            topLeft.setTop(i->sceneBoundingRect().top());
        if(i->sceneBoundingRect().center().y() < center.top())
            center.setTop(i->sceneBoundingRect().center().y());
        if(i->sceneBoundingRect().bottom() < bottomRight.top())
            bottomRight.setTop(i->sceneBoundingRect().bottom());
        
        //Set the bottom edges of the 3 possible Rects.
        if(i->sceneBoundingRect().top() > topLeft.bottom())
            topLeft.setBottom(i->sceneBoundingRect().top());
        if(i->sceneBoundingRect().center().y() > center.bottom())
            center.setBottom(i->sceneBoundingRect().center().y());
        if(i->sceneBoundingRect().bottom() > bottomRight.bottom())
            bottomRight.setBottom(i->sceneBoundingRect().bottom());
    }

    switch(horizontal) {
        case 0:
            final.setLeft(topLeft.left());
            final.setRight(bottomRight.right());
            break;
            
        case 1:
            final.setLeft(topLeft.left());
            final.setRight(topLeft.right());
            break;
            
        case 2:
            final.setLeft(center.left());
            final.setRight(center.right());
            break;
            
        case 3:
            final.setLeft(bottomRight.left());
            final.setRight(bottomRight.right());
            break;
    }

    switch(vertical) {
        case 0:
            final.setTop(topLeft.top());
            final.setBottom(bottomRight.bottom());
            break;
            
        case 1:
            final.setTop(topLeft.top());
            final.setBottom(topLeft.bottom());
            break;
            
        case 2:
            final.setTop(center.top());
            final.setBottom(center.bottom());
            break;
            
        case 3:
            final.setTop(bottomRight.top());
            final.setBottom(bottomRight.bottom());
            break;
    }
    
    //For Debugging:
    //addRect(topLeft, QPen(QColor(Qt::red)));
    //addRect(center, QPen(QColor(Qt::green)));
    //addRect(bottomRight, QPen(QColor(Qt::blue)));
    //addRect(final, QPen(QColor(Qt::black)));
    
    return final;
    
}

void Scene::distribute(int vertical, int horizontal)
{
    QList<QGraphicsItem*> unsorted = selectedItems();
    QList<QGraphicsItem*> sortedH;
    QList<QGraphicsItem*> sortedV;

    sortedH = sortItemsHorizontally(unsorted, horizontal);
    sortedV = sortItemsVertically(unsorted, vertical);
/*    
    qDebug() << unsorted;
    qDebug() << "\n\n============================================================\n\n";
    qDebug() << sortedH;
    qDebug() << "\n\n============================================================\n\n";
    qDebug() << sortedV;
*/    
    QRectF selectionRect = selectionBoundingRect(unsorted, vertical, horizontal);
    qreal spaceH = selectionRect.width() / (sortedH.count() - 1);
    qreal spaceV = selectionRect.height() / (sortedV.count() - 1);

    undoStack()->beginMacro("distribute selection");
    
    //go through all cells and adjust them based on the sorting done above.
    foreach(QGraphicsItem* i, unsorted) {
        QPointF oldPos = i->pos();
        qreal newX = 0, newY = 0;
        qreal offsetX = 0, offsetY = 0;
        
        if(vertical == 2) {
            offsetY = i->sceneBoundingRect().height()/2;
        } else if (vertical == 3) {
            offsetY = i->sceneBoundingRect().height();
        }
        
        if(horizontal == 2) {
            offsetX = i->sceneBoundingRect().width()/2;
        } else if (horizontal == 3) {
            offsetX = i->sceneBoundingRect().width();
        }

        int idxX = sortedH.indexOf(i);
        int idxY = sortedV.indexOf(i);
        
        if(horizontal == 0) {
            newX = oldPos.x();
        } else {
            newX = i->pos().x() + ((selectionRect.left() + (idxX * spaceH)) - offsetX - i->sceneBoundingRect().x());
        }
        
        if(vertical == 0) {
            newY = oldPos.y();
        } else {
            newY = i->pos().y() + ((selectionRect.top() + (idxY * spaceV)) - offsetY - i->sceneBoundingRect().y());
        }

        i->setPos(newX, newY);

        undoStack()->push(new SetItemCoordinates(i, oldPos));
    }
    undoStack()->endMacro();
}

void Scene::alignToPath()
{

}

void Scene::distributeToPath()
{

}

void Scene::resizeScene(QRectF size)
{
	setSceneRect(size);
}

void Scene::createRowsChart(int rows, int cols, QString defStitch, QSizeF rowSize)
{
    
    mDefaultSize = rowSize;
    mDefaultStitch = defStitch;
    arrangeGrid(QSize(rows, cols), QSize(1, 1), rowSize.toSize(), false);

    updateSceneRect();
    
}

void Scene::createBlankChart()
{
}

void Scene::arrangeGrid(QSize grd, QSize alignment, QSize spacing, bool useSelection)
{
    //FIXME: make this function create a grid out of a selection of stitches.
    Q_UNUSED(alignment);

    if(useSelection) {
    /*
        boundingRect().height()[.width()] / rows/[cols] = MOE for spacing.

        look in each "box" for a stitch
        if found position stitch per user rules above.
        else leave box empty?

    */
    } else {
        //create new cells.
        //TODO: figure out how to deal with spacing.

        for(int x = grd.width(); x > 0; --x) {

            QList<Cell*> r;
            for(int y = grd.height(); y > 0; --y) {
                Cell* c = new Cell();
                //FIXME: use the user selected stitch
                c->setStitch(mDefaultStitch);
				c->setLayer(getCurrentLayer()->uid());
                addItem(c);
                r.append(c);
                
                c->useAlternateRenderer(((grd.width() - x) % 2));
                c->setPos((c->stitch()->width() + spacing.width()) * y, spacing.height() * x);
            }

            grid.insert(0, r);
        }
    }
}

void Scene::gridAddRow(QList< Cell*> row, bool append, int before)
{
    if(append) {
        grid.append(row);
    } else {
        if(grid.length() >= before)
            grid.insert(before, row);
    }
}

void Scene::propertiesUpdate(QString property, QVariant newValue)
{

    if(property == "ChartCenter") {
        setShowChartCenter(newValue.toBool());
	
	} else if (property ==  "AlignAngle") {
		setSnapAngle(newValue.toBool());
		
    } else if(property == "Guidelines") {
        Guidelines g = newValue.value<Guidelines>();
        if(mGuidelines != g) {
            mGuidelines = g;
            updateGuidelines();
        }

    } else { // all options that require operations on a per stitch level.

        if(selectedItems().count() <= 0)
            return;

        IndicatorProperties ip;
        ip = newValue.value<IndicatorProperties>();

        undoStack()->beginMacro(property);
        foreach(QGraphicsItem *i, selectedItems()) {
            Cell *c = 0;
            Indicator *ind = 0;
            ItemGroup *g = 0;
			ChartImage* ci = 0;

            if(i->type() == Cell::Type) {
                c = qgraphicsitem_cast<Cell*>(i);
            } else if(i->type() == Indicator::Type) {
                ind = qgraphicsitem_cast<Indicator*>(i);

            } else if(i->type() == ItemGroup::Type) {
                g = qgraphicsitem_cast<ItemGroup*>(i);
            } else if (i->type() == ChartImage::Type) {
				ci = qgraphicsitem_cast<ChartImage*>(i);
			}

            if(property == "Angle") {
				qreal rotation = ChartItemTools::getRotation(i);
				ChartItemTools::setRotation(i, newValue.toReal());
                undoStack()->push(new SetItemRotation(i, rotation,
														 ChartItemTools::getRotationPivot(i)));

            } else if(property == "PositionX") {
                QPointF oldPos = i->pos();
                i->setPos(newValue.toReal(), i->pos().y());
                undoStack()->push(new SetItemCoordinates(i, oldPos));

            } else if(property == "PositionY") {
                QPointF oldPos = i->pos();
                i->setPos(i->pos().x(), newValue.toReal());
                undoStack()->push(new SetItemCoordinates(i, oldPos));

            } else if(property == "ScaleX") {
				QPointF scale = ChartItemTools::getScale(i);
                ChartItemTools::setScaleX(i, newValue.toDouble());
                undoStack()->push(new SetItemScale(i, scale,
													  ChartItemTools::getScalePivot(i)));

            } else if(property == "ScaleY") {
                ChartItemTools::setScaleY(i, newValue.toDouble());
                undoStack()->push(new SetItemScale(i, ChartItemTools::getScale(i),
													  ChartItemTools::getScalePivot(i)));

            } else if(property == "Stitch") {
                undoStack()->push(new SetCellStitch(c, newValue.toString()));

            } else if(property == "Delete") {
                undoStack()->push(new RemoveItem(this, i));

            } else if(property == "Indicator") {
                ind->setStyle(ip.style());
				QFont font = ip.font();
				font.setPointSize(ip.size());
				ind->setFont(font);

            } else if(property == "fgColor") {
                undoStack()->push(new SetCellColor(c, newValue.value<QColor>()));

            } else if(property == "bgColor") {
                undoStack()->push(new SetCellBgColor(c, newValue.value<QColor>()));

            } else if(property == "ChartImagePath") {
                undoStack()->push(new SetChartImagePath(ci, newValue.toString()));
				
			} else if (property == "ChartImageZLayer") {
				undoStack()->push(new SetChartZLayer(ci, newValue.toString()));
            } else {
                qWarning() << "Unknown property: " << property;
            }
        }
        undoStack()->endMacro();
    }

}

void Scene::updateDefaultStitchColor(QColor originalColor, QColor newColor)
{

    foreach(QGraphicsItem *item, items()) {
        if(item->type() != Cell::Type)
            continue;

        Cell *c = qgraphicsitem_cast<Cell*>(item);
        if(c->color() == originalColor)
        c->setColor(newColor);
    }
}

QGraphicsItem* Scene::copy_rec(QGraphicsItem* item, QPointF displacement)
{
	if (item->type() == Cell::Type) {
		//get the cell
		Cell* cell = qgraphicsitem_cast<Cell*>(item);
		
		//now we clone the cell
		Cell* clone = cell->copy();
		undoStack()->push(new AddItem(this, clone));
		clone->setPos(cell->pos());
		clone->setLayer(cell->layer());
		
		//move the clone with the displacement
		clone->moveBy(displacement.x(), displacement.y());
		
		return clone;
	} else if (item->type() == ChartImage::Type) {
		//get the image
		ChartImage* image = qgraphicsitem_cast<ChartImage*>(item);
		
		//and clone it
		ChartImage* newImage = new ChartImage(image->filename());
		newImage->setPos(image->pos());
		newImage->setTransformations(ChartItemTools::cloneGraphicsTransformations(image));
		newImage->setRotation(image->rotation());
		newImage->setLayer(getCurrentLayer()->uid());
		undoStack()->push(new AddItem(this, newImage));
		
		//move the clone with the displacement
		newImage->moveBy(displacement.x(), displacement.y());
		
		return newImage;
	} else if (item->type() == ItemGroup::Type) {
		//get the group
		ItemGroup* g = qgraphicsitem_cast<ItemGroup*>(item);
		QList<QGraphicsItem*> childs = g->childItems();
		
		//clone it
		ItemGroup* newGroup = new ItemGroup();
		newGroup->setLayer(g->layer());
		undoStack()->push(new AddItem(this, newGroup));
		
		mPivotPt = QPointF(g->boundingRect().center().x(),
						   g->boundingRect().bottom());
		
		newGroup->setTransformOriginPoint(mPivotPt);
		newGroup->setRotation(0);
		newGroup->setTransformations(ChartItemTools::cloneGraphicsTransformations(g));
		
		foreach(QGraphicsItem* child, childs) {
			g->removeFromGroup(child);
			ChartItemTools::recalculateTransformations(child);
			QGraphicsItem* childCopy = copy_rec(child, displacement);
			g->addToGroup(child);
			if (childCopy != NULL) {
				childCopy->setSelected(false);
				childCopy->setFlag(QGraphicsItem::ItemIsSelectable, false);
				newGroup->addToGroup(childCopy);
			}
		}
		
		return newGroup;
	}
	return NULL;
}

void Scene::copy(int direction)
{
    if(selectedItems().count() <= 0)
        return;
	
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QRectF rect = selectedItemsBoundingRect(selectedItems());
    QList<QGraphicsItem*> list = selectedItems();

	blockSignals(true);

    clearSelection();
	//TODO setSelected is extremely slow, optimize that
    undoStack()->beginMacro("copy selection");
    if(direction == 1) { //left
		foreach(QGraphicsItem* item, list) {
			QGraphicsItem* ret = copy_rec(item, QPointF(-rect.width(), 0));
			if (ret != NULL)
				ret->setSelected(true);
		}
	} else if (direction == 2) { //right
		foreach(QGraphicsItem* item, list) {
			QGraphicsItem* ret = copy_rec(item, QPointF(rect.width(), 0));
			if (ret != NULL)
				ret->setSelected(true);
		}
	} else if (direction == 3) { //up
		foreach(QGraphicsItem* item, list) {
			QGraphicsItem* ret = copy_rec(item, QPointF(0, -rect.height()));
			if (ret != NULL)
				ret->setSelected(true);
		}
	} else if (direction == 4) { //down
		foreach(QGraphicsItem* item, list) {
			QGraphicsItem* ret = copy_rec(item, QPointF(0, rect.height()));
			if (ret != NULL)
				ret->setSelected(true);
		}
	}
	
	blockSignals(false);
	
	emit selectionChanged();
	
	updateSceneRect();
    undoStack()->endMacro();
    QApplication::restoreOverrideCursor();
}

QGraphicsItem* Scene::mirror_rec(QGraphicsItem* item, QPointF displacement, QRectF selectionRect, bool flipX, bool flipY)
{
	//first we copy the item
	QGraphicsItem* newItem = copy_rec(item, displacement);

	if (newItem != NULL)
	{	
		//then we flip it in the selection
		QPointF oldrel = item->sceneBoundingRect().center() - selectionRect.center();
		QPointF targetrel = QPointF(oldrel.x(), oldrel.y());
		
		if (flipX)
			targetrel.setX(-targetrel.x());
		if (flipY)
			targetrel.setY(-targetrel.y());
		
		QPointF flipdisplacement = targetrel - oldrel;
		newItem->moveBy(flipdisplacement.x(), flipdisplacement.y());
		
		//finally, we flip each remaining object in place
		ChartItemTools::setScalePivot(newItem, newItem->boundingRect().center());
		ChartItemTools::setRotationPivot(newItem, newItem->boundingRect().center());
		
		if (flipX) {
			ChartItemTools::setRotation(newItem, -ChartItemTools::getRotation(newItem));
			ChartItemTools::setScaleX(newItem, -ChartItemTools::getScaleX(newItem));
		} else if (flipY) {
			ChartItemTools::setRotation(newItem, -ChartItemTools::getRotation(newItem));
			ChartItemTools::setScaleY(newItem, -ChartItemTools::getScaleY(newItem));
		}
	}
	return newItem;

}

void Scene::mirror(int direction)
{
    if(selectedItems().count() <= 0)
        return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QRectF rect = selectedItemsBoundingRect(selectedItems());
    QList<QGraphicsItem*> list = selectedItems();

	blockSignals(true);

    clearSelection();

    undoStack()->beginMacro("mirror selection");
    if(direction == 1) { //left

        foreach(QGraphicsItem *item, list) {
			QGraphicsItem* mir = mirror_rec(item, QPointF(-rect.width(), 0), rect, true, false);
			if (mir == NULL)
				continue;
			mir->setSelected(true);
        }

    } else if(direction == 2) { //right

        foreach(QGraphicsItem *item, list) {
			QGraphicsItem* mir = mirror_rec(item, QPointF(rect.width(), 0), rect, true, false);
			if (mir == NULL)
				continue;
			mir->setSelected(true);
        }

    } else if(direction == 3) { //up

        foreach(QGraphicsItem *item, list) {
			QGraphicsItem* mir = mirror_rec(item, QPointF(0, -rect.height()), rect, false, true);
			if (mir == NULL)
				continue;
			mir->setSelected(true);
        }

    } else if(direction == 4) { //down

        foreach(QGraphicsItem *item, list) {
			QGraphicsItem* mir = mirror_rec(item, QPointF(0, rect.height()), rect, false, true);
			if (mir == NULL)
				continue;
			mir->setSelected(true);
        }
    }
	updateSceneRect();
	
	blockSignals(false);
	
	emit selectionChanged();
	
    undoStack()->endMacro();
	
    QApplication::restoreOverrideCursor();
}

void Scene::rotate(qreal degrees)
{
    if(selectedItems().count() <= 0)
        return;

    undoStack()->push(new SetSelectionRotation(this, selectedItems(), degrees));
}

void Scene::copy()
{
    if(selectedItems().count() <= 0)
        return;

    QByteArray copyData;
    QDataStream stream(&copyData, QIODevice::WriteOnly);
    QMimeData *mimeData = new QMimeData;

    stream << selectedItems().count();
    copyRecursively(stream, selectedItems());

    mimeData->setData("application/crochet-cells", copyData);
    QApplication::clipboard()->setMimeData(mimeData);

}

void Scene::copyRecursively(QDataStream &stream, QList<QGraphicsItem*> items)
{
    foreach(QGraphicsItem* item, items) {
        stream << item->type();

        switch(item->type()) {
            case Cell::Type: {
                Cell* c = qgraphicsitem_cast<Cell*>(item);
                stream << c->name() << c->color() << c->bgColor()
                    << ChartItemTools::getRotation(c) << ChartItemTools::getRotationPivot(c)
                    << ChartItemTools::getScaleX(c) << ChartItemTools::getScaleY(c)
					<< ChartItemTools::getScalePivot(c) << c->transformOriginPoint() << c->pos();
                break;
            }
            case Indicator::Type: {
                Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                stream << i->scenePos() << i->text()
					<< ChartItemTools::getRotation(i) << ChartItemTools::getRotationPivot(i)
                    << ChartItemTools::getScaleX(i) << ChartItemTools::getScaleY(i)
					<< ChartItemTools::getScalePivot(i);
                break;
            }
            case ItemGroup::Type: {
                ItemGroup* group = qgraphicsitem_cast<ItemGroup*>(item);
				ChartItemTools::recalculateTransformations(group);
                stream << group->pos()
					<< group->childItems().count()
					<< ChartItemTools::getRotation(group) << ChartItemTools::getRotationPivot(group)
                    << ChartItemTools::getScaleX(group) << ChartItemTools::getScaleY(group)
					<< ChartItemTools::getScalePivot(group);
				
				QList<QGraphicsItem*> childs = group->childItems();
				
				foreach(QGraphicsItem* child, childs) {
						group->removeFromGroup(child);
						ChartItemTools::recalculateTransformations(child);
				}
					
                copyRecursively(stream, childs);
				
				foreach(QGraphicsItem* child, childs) {
						group->addToGroup(child);
				}
                break;
            }
			case ChartImage::Type: {
				ChartImage* image = qgraphicsitem_cast<ChartImage*>(item);
				
				stream << image->filename() << image->pos()
					<< ChartItemTools::getRotation(image) << ChartItemTools::getRotationPivot(image)
                    << ChartItemTools::getScaleX(image) << ChartItemTools::getScaleY(image)
					<< ChartItemTools::getScalePivot(image);
				break;
			}
            case Guideline::Type:
                qDebug() << "guideline";
            default:
                WARN("Unknown data type: " + QString::number(item->type()));
                break;
        }
    }
}

void Scene::paste()
{
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    QByteArray pasteData = mimeData->data("application/crochet-cells");
    QDataStream stream(&pasteData, QIODevice::ReadOnly);

    clearSelection();

    undoStack()->beginMacro("paste items");
    int count = 0;
    stream >> count;
    
	//disable signals for performance
	blockSignals(true);
	
    QList<QGraphicsItem*> items;
    for(int i = 0; i < count; ++i) {
        pasteRecursively(stream, &items);
    }

    foreach(QGraphicsItem* item, items)
        item->setSelected(true);
		
	//if we need to center around the mouse
	if (Settings::inst()->value("pasteOnMouseLocation").toBool() == true) {
		if (items.count() > 0) {
			
			//get the bounding box of all pasted items
			QRectF box = items.first()->sceneBoundingRect();
			foreach(QGraphicsItem* item, items) {
				box = box.unite(item->sceneBoundingRect());
			}
			
			//get the cursor position in the scene
			QPointF mousePos = views().first()->mapToScene(views().first()->mapFromGlobal(QCursor::pos()));
			
			//now calculate the amount we need to move the items
			QPointF diff =  mousePos - box.center();
			
			//and move all objects with that diff
			foreach(QGraphicsItem* item, items) {
				item->moveBy(diff.x(), diff.y());
			}
		}
	}
	
	//finally, snap each pasted object on the grid
    foreach(QGraphicsItem* item, items) {
		snapGraphicsItemToGrid(*item);
	}
	
	blockSignals(false);
	
	emit selectionChanged();
	
    undoStack()->endMacro();
}

void Scene::insertImage(const QString& filename, QPointF pos)
{
	//first, create a new image item
	ChartImage* image = new ChartImage(filename);
	image->setPos(pos);
	undoStack()->push(new AddItem(this, image));
}

void Scene::deleteSelection()
{
    QList<QGraphicsItem*> items = selectedItems();
    undoStack()->beginMacro("remove items");
	//undoStack()->push(new RemoveItems(this, items));
	blockSignals(true);
	foreach(QGraphicsItem* item, items) {
		removeItem(item);
        switch(item->type()) {
            case ItemGroup::Type:
            case Cell::Type: {
                undoStack()->push(new RemoveItem(this, item));
                break;
            }
            case Indicator::Type: {
                Indicator *i = qgraphicsitem_cast<Indicator*>(item);
                undoStack()->push(new RemoveIndicator(this, i));
                break;
            }
			case ChartImage::Type: {
				ChartImage *i = qgraphicsitem_cast<ChartImage*>(item);
				undoStack()->push(new RemoveItem(this, i));
				break;
			}
            default:
                qWarning() << "keyReleaseEvent - unknown type: " << item->type();
                break;
        }
    }
	blockSignals(false);
	
	//signals were blocked so we manually emit them
	emit selectionChanged();
	
    undoStack()->endMacro();
}

void Scene::pasteRecursively(QDataStream &stream, QList<QGraphicsItem*> *group)
{
    QPointF offSet;
    QString pasteOS = Settings::inst()->value("pasteOffset").toString();
    if(pasteOS == tr("Up and Left"))
        offSet = QPointF(-10, -10);
    else if (pasteOS == tr("Up and Right"))
        offSet = QPointF(10, -10);
    else if (pasteOS == tr("Down and Left"))
        offSet = QPointF(-10, 10);
    else if (pasteOS == tr("Down and Right"))
        offSet = QPointF(10, 10);
    else
        offSet = QPointF(0,0);

    int type;
    stream >> type;
    switch(type) {

        case Cell::Type: {
            QString name;
            QColor color, bgColor;
            qreal rotation, scaleX, scaleY;
            QPointF pos, transPoint, pivotRotation, pivotScale;
		
            stream >> name >> color >> bgColor >> rotation >> pivotRotation;
			stream >> scaleX >> scaleY >> pivotScale >> transPoint >> pos;

            pos += offSet;

            Cell *c = new Cell();
            undoStack()->push(new AddItem(this, c));
            c->setPos(pos);
            c->setStitch(name);
            c->setColor(color);
            c->setBgColor(bgColor);
			c->setLayer(getCurrentLayer()->uid());

            c->setTransformOriginPoint(transPoint);
            ChartItemTools::setRotation(c, rotation);
            ChartItemTools::setScaleX(c, scaleX);
			ChartItemTools::setScaleY(c, scaleY);
			ChartItemTools::setScalePivot(c, pivotScale, false);
			ChartItemTools::setRotationPivot(c, pivotRotation, false);

            c->setSelected(false);
            group->append(c);
            break;
        }
        case Indicator::Type: {
            Indicator *i = new Indicator();
            //FIXME: add this indicator to the undo stack.
            QPointF pos, pivotScale, pivotRotation;
            QString text;
            qreal rotation, scaleX, scaleY;
			
            stream >> pos >> text >> rotation >> pivotRotation >> scaleX;
			stream >> scaleY >> pivotScale;
			
            pos += offSet;
            i->setText(text);
            addItem(i);
            i->setPos(pos);
			i->setLayer(getCurrentLayer()->uid());

            i->setSelected(false);
			
            ChartItemTools::setRotation(i, rotation);
            ChartItemTools::setScaleX(i, scaleX);
			ChartItemTools::setScaleY(i, scaleY);
			ChartItemTools::setScalePivot(i, pivotScale, false);
			ChartItemTools::setRotationPivot(i, pivotRotation, false);
			
			group->append(i);
			break;
        }
        case ItemGroup::Type: {
			
            QPointF pos, pivotScale, pivotRotation;
			qreal rotation, scaleX, scaleY;
			int childCount;
			
			stream >> pos >> childCount >> rotation >> pivotRotation >> scaleX;
			stream >> scaleY >> pivotScale;
			QList<QGraphicsItem*> items;
            for(int i = 0; i < childCount; ++i) {
                pasteRecursively(stream, &items);
            }
			
			
            GroupItems *grpItems = new GroupItems(this, items);
            undoStack()->push(grpItems);
            ItemGroup *g = grpItems->group();
			g->setSelected(false);
            
			group->append(g);
			
			foreach(QGraphicsItem* child, items) {
				g->removeFromGroup(child);
			}
			
            g->setPos(pos);
			ChartItemTools::setRotation(g, rotation);
			ChartItemTools::setScaleX(g, scaleX);
			ChartItemTools::setScaleY(g, scaleY);
			ChartItemTools::setScalePivot(g, pivotScale, false);
			ChartItemTools::setRotationPivot(g, pivotRotation, false);
			
			foreach(QGraphicsItem* child, items) {
				g->addToGroup(child);
			}
			
			g->setLayer(getCurrentLayer()->uid());
            break;
        }
		case ChartImage::Type: {
			qreal rotation, scaleX, scaleY;
			QPointF pos, pivotScale, pivotRotation;
			QString filename;
					
			stream >> filename >> pos >> rotation >> pivotRotation >> scaleX >> scaleY >> pivotScale;
			
			ChartImage* image = new ChartImage(filename);
			
            undoStack()->push(new AddItem(this, image));
			image->setPos(pos);
			image->setLayer(getCurrentLayer()->uid());
			
            ChartItemTools::setRotation(image, rotation);
            ChartItemTools::setScaleX(image, scaleX);
			ChartItemTools::setScaleY(image, scaleY);
			ChartItemTools::setScalePivot(image, pivotScale, false);
			ChartItemTools::setRotationPivot(image, pivotRotation, false);
			
			group->append(image);
			image->setSelected(false);
			break;
		}
        default: {
            WARN("Unknown data type: " + QString::number(type));
            break;
        }
    }

}

void Scene::cut()
{
    if(selectedItems().count() <= 0)
        return;

    copy();

    undoStack()->beginMacro(tr("cut items"));
    foreach(QGraphicsItem *item, selectedItems()) {

        switch(item->type()) {
            case Cell::Type: {
                Cell *c = qgraphicsitem_cast<Cell*>(item);
                undoStack()->push(new RemoveItem(this, c));
                break;
            }
            case Indicator::Type: {
                Indicator *i = qgraphicsitem_cast<Indicator*>(item);
                undoStack()->push(new RemoveIndicator(this, i));
                break;
            }
            case ItemGroup::Type: {
                ItemGroup *g = qgraphicsitem_cast<ItemGroup*>(item);
                undoStack()->push(new RemoveItem(this, g));
                break;
            }
            default:
                WARN("Unknown data type: " + QString::number(item->type()));
                break;
        }
    }
    undoStack()->endMacro();

}

QRectF Scene::selectedItemsBoundingRect(QList<QGraphicsItem*> items)
{
    if(items.count() <= 0)
        return QRectF();

    qreal left = sceneRect().right();
    qreal right = sceneRect().left();
    qreal top = sceneRect().bottom();
    qreal bottom = sceneRect().top();

    foreach(QGraphicsItem* i, items) {
        if(i->sceneBoundingRect().left() < left) {
            left = i->sceneBoundingRect().left();
        }
        if(i->sceneBoundingRect().right() > right) {
            right = i->sceneBoundingRect().right();
        }
        if(i->sceneBoundingRect().top() < top) {
            top = i->sceneBoundingRect().top();
        }
        if(i->sceneBoundingRect().bottom() > bottom) {
            bottom = i->sceneBoundingRect().bottom();
        }
    }

    return QRectF(QPointF(left, top), QPointF(right, bottom));
}

void Scene::group()
{
    if(selectedItems().count() <= 1)
        return;

    undoStack()->push(new GroupItems(this, selectedItems()));
}

ItemGroup* Scene::group(QList<QGraphicsItem*> items, ItemGroup* g)
{

    //clear selection because we're going to create a new selection.
    clearSelection();
		
    if(!g) {
        g = new ItemGroup(0, this);
		g->setLayer(getCurrentLayer()->uid());
		
		/*if (items.count() > 0) {
			QRectF bigRect = items.first()->sceneBoundingRect();
			foreach(QGraphicsItem* i, items) {
				bigRect = bigRect.united(i->sceneBoundingRect());
			}
			g->setPos(bigRect.center());
			g->setTransformOriginPoint(0, 0);
		}*/
    }

    foreach(QGraphicsItem *i, items) {
        i->setSelected(false);
        i->setFlag(QGraphicsItem::ItemIsSelectable, false);
        g->addToGroup(i);
    }

    g->setFlag(QGraphicsItem::ItemIsMovable);
    g->setFlag(QGraphicsItem::ItemIsSelectable);
    mGroups.append(g);

    g->setSelected(true);

    return g;
}

void Scene::ungroup()
{
    if(selectedItems().count() <= 0)
        return;

    foreach(QGraphicsItem* item, selectedItems()) {
        if(item->type() == ItemGroup::Type) {
            ItemGroup* group = qgraphicsitem_cast<ItemGroup*>(item);
            undoStack()->push(new UngroupItems(this, group));
        }
    }
}

void Scene::ungroup(ItemGroup* group)
{
	blockSignals(true);
	group->setSelected(false);
    mGroups.removeOne(group);
	QList<QGraphicsItem*> childs = group->childItems();
    foreach(QGraphicsItem* item, childs) {
        group->removeFromGroup(item);
        item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        item->setSelected(true);
    }
	
	foreach(QGraphicsItem* item, childs) {
		ChartItemTools::recalculateTransformations(item);
	}
	blockSignals(false);
	//emit selectionChanged();
}

void Scene::addToGroup(int groupNumber, QGraphicsItem *i)
{
    mGroups[groupNumber]->addToGroup(i);
    i->setFlag(QGraphicsItem::ItemIsSelectable, false);
    i->setSelected(false);
}

ItemGroup* Scene::getGroup(int groupNumber)
{
	return mGroups[groupNumber];
}

void Scene::editorLostFocus(Indicator *item)
 {

    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);
 }

void Scene::editorGotFocus(Indicator* item)
{
    foreach(Indicator *i, mIndicators) {
        if(i != item) {
            i->clearFocus();
            i->setSelected(false);
        }
    }
    
}

QRectF Scene::itemsBoundingRect()
{
    QList<QGraphicsItem*> itemList = items();

    QRectF rect = selectedItemsBoundingRect(itemList);

    rect.setTop(rect.top() - 10);
    rect.setBottom(rect.bottom() + 10);

    rect.setLeft(rect.left() - 10);
    rect.setRight(rect.right() + 10);

    return rect;

}

void Scene::updateSceneRect()
{
    QRectF ibr = itemsBoundingRect();
    QRectF sbr = sceneRect();
    QRectF final;

    ibr.setTop(ibr.top() - SCENE_CLAMP_BORDER_SIZE);
    ibr.setBottom(ibr.bottom() + SCENE_CLAMP_BORDER_SIZE);
    ibr.setLeft(ibr.left() - SCENE_CLAMP_BORDER_SIZE);
    ibr.setRight(ibr.right() + SCENE_CLAMP_BORDER_SIZE);
    
    final.setBottom((ibr.bottom() >= sbr.bottom()) ? ibr.bottom() : sbr.bottom());
    final.setTop((ibr.top() <= sbr.top()) ? ibr.top() : sbr.top());
    final.setLeft((ibr.left() <= sbr.left()) ? ibr.left() : sbr.left());
    final.setRight((ibr.right() >= sbr.right()) ? ibr.right() : sbr.right());

    setSceneRect(final);
    
}

/**
 * layer manipulation functions
 */
void Scene::addLayer(const QString& name)
{
	ChartLayer* layer = new ChartLayer(name);
	mLayers[layer->uid()] = layer;
	
	QList<ChartLayer*> l = layers();
	emit layersChanged(l, mSelectedLayer);
}

void Scene::addLayer(const QString& name, unsigned int uid)
{
	ChartLayer* layer = new ChartLayer(uid, name);
	mLayers[layer->uid()] = layer;
	
	QList<ChartLayer*> l = layers();
	emit layersChanged(l, mSelectedLayer);
}

void Scene::removeSelectedLayer()
{
	if (mSelectedLayer != NULL) {
		//first, remove all items in the layer
		QList<QGraphicsItem*> toRemove;
		foreach(QGraphicsItem *item, items()) {
            switch(item->type()) {
                case Cell::Type: {
                    Cell *c = qgraphicsitem_cast<Cell*>(item);
                    if (c->layer() == mSelectedLayer->uid())
                        toRemove.append(c);
                    break;
                }
                case Indicator::Type: {
                    Indicator*c = qgraphicsitem_cast<Indicator*>(item);
                    if (c->layer() == mSelectedLayer->uid())
                        toRemove.append(c);
                    break;
                }
                case ItemGroup::Type: {
                    ItemGroup *c = qgraphicsitem_cast<ItemGroup*>(item);
                    if (c->layer() == mSelectedLayer->uid())
                        toRemove.append(c);
                    break;
                }
                case ChartImage::Type: {
                    ChartImage *c = qgraphicsitem_cast<ChartImage*>(item);
                    if (c->layer() == mSelectedLayer->uid())
                        toRemove.append(c);
                    break;
                }
                default:
                    WARN("Unknown data type: " + QString::number(item->type()));
                    break;
            }
        }
		
		foreach (QGraphicsItem* item, toRemove) {
			removeItem(item);
		}
		
		//then remove the layer
		mLayers.remove(mSelectedLayer->uid());
		delete mSelectedLayer;
		
		//and select another one
		QList<ChartLayer*> l = layers();
		if (l.count() > 0)
			selectLayer(l.first()->uid());
		else
			mSelectedLayer = NULL;
			
		emit layersChanged(l, mSelectedLayer);
	}
}

void Scene::mergeLayer(unsigned int from, unsigned int to)
{
	//we need to have a layer selected
	if (mSelectedLayer != NULL)
	{
		//move all items in the from layer to the to layer
		foreach(QGraphicsItem *item, items()) {
			switch(item->type()) {
				case Cell::Type: {
					Cell *c = qgraphicsitem_cast<Cell*>(item);
					if (c->layer() == from)
						c->setLayer(to);
					break;
				}
				case Indicator::Type: {
					Indicator*c = qgraphicsitem_cast<Indicator*>(item);
					if (c->layer() == from)
						c->setLayer(to);
					break;
				}
				case ItemGroup::Type: {
					ItemGroup *c = qgraphicsitem_cast<ItemGroup*>(item);
					if (c->layer() == from)
						c->setLayer(to);
					break;
				}
				case ChartImage::Type: {
					ChartImage *c = qgraphicsitem_cast<ChartImage*>(item);
					if (c->layer() == from)
						c->setLayer(to);
					break;
				}
				default:
					WARN("Unknown data type: " + QString::number(item->type()));
					break;
			}
		}
		
		//and now we remove from
		selectLayer(from);
		removeSelectedLayer();
		selectLayer(to);
		editedLayer(getLayer(to));
	}
}

void Scene::selectLayer(unsigned int uid)
{
	clearSelection();
	
	ChartLayer* layer = mLayers[uid];
	if (layer != NULL) {
		mSelectedLayer = layer;
	}
	
    foreach(QGraphicsItem *item, items()) {
        switch(item->type()) {
            case Cell::Type: {
			Cell *c = qgraphicsitem_cast<Cell*>(item);
			c->setFlag(QGraphicsItem::ItemIsSelectable, c->layer() == layer->uid() && c->parentItem() == NULL);
			c->setSelected(false);
                break;
            }
            case Indicator::Type: {
			Indicator*c = qgraphicsitem_cast<Indicator*>(item);
			c->setFlag(QGraphicsItem::ItemIsSelectable, c->layer() == layer->uid() && c->parentItem() == NULL);
			c->setSelected(false);
                break;
            }
            case ItemGroup::Type: {
			ItemGroup *c = qgraphicsitem_cast<ItemGroup*>(item);
			c->setFlag(QGraphicsItem::ItemIsSelectable, c->layer() == layer->uid() && c->parentItem() == NULL);
			c->setSelected(false);
                break;
            }
            case ChartImage::Type: {
			ChartImage *c = qgraphicsitem_cast<ChartImage*>(item);
			c->setFlag(QGraphicsItem::ItemIsSelectable, c->layer() == layer->uid() && c->parentItem() == NULL);
			c->setSelected(false);
                break;
            }
            default:
                WARN("Unknown data type: " + QString::number(item->type()));
                break;
        }
    }
}

void Scene::editedLayer(ChartLayer* layer)
{
    foreach(QGraphicsItem *item, items()) {
        switch(item->type()) {
            case Cell::Type: {
                Cell *c = qgraphicsitem_cast<Cell*>(item);
                if (c->layer() == layer->uid()) {
                c->setVisible(layer->visible());
                c->setFlag(QGraphicsItem::ItemIsSelectable, layer->visible()
                    && c->parentItem() == NULL && layer->uid() == mSelectedLayer->uid());
                } 
				break;
            }
            case Indicator::Type: {
                Indicator*c = qgraphicsitem_cast<Indicator*>(item);
                if (c->layer() == layer->uid()) {
                c->setVisible(layer->visible());
                c->setFlag(QGraphicsItem::ItemIsSelectable, layer->visible()
                    && c->parentItem() == NULL && layer->uid() == mSelectedLayer->uid());
                }
                break;
            }
            case ItemGroup::Type: {
                ItemGroup *c = qgraphicsitem_cast<ItemGroup*>(item);
                if (c->layer() == layer->uid()) {
                c->setVisible(layer->visible());
                c->setFlag(QGraphicsItem::ItemIsSelectable, layer->visible()
                    && c->parentItem() == NULL && layer->uid() == mSelectedLayer->uid());
                }
				break;
            } case ChartImage::Type: {
                ChartImage *c = qgraphicsitem_cast<ChartImage*>(item);
                if (c->layer() == layer->uid()) {
                c->setVisible(layer->visible());
                c->setFlag(QGraphicsItem::ItemIsSelectable, layer->visible()
                    && c->parentItem() == NULL && layer->uid() == mSelectedLayer->uid());
                }
				break;
				
			}
            default:
                WARN("Unknown data type: " + QString::number(item->type()));
                break;
        }
    }
}

ChartLayer* Scene::getCurrentLayer()
{
	if (mSelectedLayer == NULL)
	{
		if (mLayers.count() == 0) {
			ChartLayer* layer = new ChartLayer("New Layer");
			mLayers[layer->uid()] = layer;
		}
		
		mSelectedLayer = *mLayers.begin();

		QList<ChartLayer*> l = layers();
		emit layersChanged(l, mSelectedLayer);
	}
	return mSelectedLayer;
}

ChartLayer* Scene::getLayer(int uid)
{
	ChartLayer* layer = mLayers[uid];
	if (layer == NULL) {
		addLayer("New Layer", uid);
		return mLayers[uid];
	}
	return layer;
}

void Scene::refreshLayers()
{
	foreach(ChartLayer* layer, layers()) {
		editedLayer(layer);
	}
}


/*************************************************\
| Rounds Specific functions:                      |
\*************************************************/

bool Scene::showChartCenter()
{
    return mShowChartCenter;
}

void Scene::setSnapAngle(bool state)
{
	mSnapAngle = state;
}

void Scene::setShowChartCenter(bool state)
{

    mShowChartCenter = state;

    if(mShowChartCenter) {
        if(!mCenterSymbol) {
            QPen pen;
            pen.setWidth(5);

            double radius = (96 * 0.5);

            QRectF rect = QRectF(0,0,0,0);

            //FIXME: this is really bad.
            if(sceneRect().left() > -2500) {
                QGraphicsView* view =  views().first();
                QPointF topLeft = view->mapToScene(0, 0);
                QPointF bottomRight = view->mapToScene(view->width(), view->height());

                rect = QRectF(topLeft, bottomRight);
            }

            mCenterSymbol = addEllipse(rect.center().x()-radius, rect.center().y()-radius, radius * 2, radius * 2, pen);
            mCenterSymbol->setFlag(QGraphicsItem::ItemIsMovable);
            mCenterSymbol->setFlag(QGraphicsItem::ItemIsSelectable);

            updateGuidelines();
        } else {

            addItem(mCenterSymbol);
            updateGuidelines();
        }
    } else {

        removeItem(mCenterSymbol);
        updateGuidelines();
    }

}

void Scene::createRoundsChart(int rows, int cols, QString stitch, QSizeF rowSize, int increaseBy)
{

    mDefaultSize = rowSize;

    for(int i = 0; i < rows; ++i) {
        //FIXME: this padding should be dependant on the height of the sts.
        int pad = i * increaseBy;

        createRow(i, cols + pad, stitch);
    }

    setShowChartCenter(Settings::inst()->value("showChartCenter").toBool());

    updateSceneRect();
    
}

void Scene::setCellPosition(int row, int column, Cell* c, int columns)
{

    double widthInDegrees = 360.0 / columns;

    double radius = defaultSize().height() * (row + 1) + (32);

    double degrees = widthInDegrees * column;

    QPointF pivotPt = QPointF(c->boundingRect().width()/2, c->boundingRect().height());
    c->setTransformOriginPoint(pivotPt);
    c->setRotation(degrees + 90);

    QPointF finish = calcPoint(radius, degrees, QPointF(0,0));
    finish.rx() -= c->boundingRect().width()/2;
    finish.ry() -= c->boundingRect().height()/2;

    c->setPos(finish.x(), finish.y());

}

QPointF Scene::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

void Scene::createRow(int row, int columns, QString stitch)
{

    Cell* c = 0;

    QList<Cell*> modelRow;
    for(int i = 0; i < columns; ++i) {
        c = new Cell();
        c->setStitch(stitch);
        addItem(c);
        
        modelRow.append(c);
        setCellPosition(row, i, c, columns);
    }
    grid.insert(row, modelRow);

}

void Scene::setEditMode(EditMode mode)
{
    mMode = mode;
    if(mode != Scene::RowEdit)
        hideRowLines();

    bool state = false;
    if (mode == Scene::IndicatorEdit)
        state = true;
    highlightIndicators(state);
}

void Scene::setSelectMode(Scene::SelectMode mode)
{
	mSelectMode = mode;
}

Scene::SelectMode Scene::selectMode() const
{
	return mSelectMode;
}

void Scene::highlightIndicators(bool state)
{
    foreach(Indicator* i, mIndicators) {
        if(!state)
            i->setTextInteractionFlags(Qt::NoTextInteraction);
        i->highlight = state;
        i->update();
    }
}

void Scene::setGuidelinesType(QString guides)
{

    if(mGuidelines.type() != guides) {
        mGuidelines.setType(guides);
        if(guides == tr("Round")) {
            DEBUG("TODO: show guidelines round");
        } else if(guides == tr("Grid")) {
            DEBUG("TODO: show guidelines grid");
        } else {
            DEBUG("TODO: hide guidelines");
        }
    }
    
    updateGuidelines();
}

void Scene::replaceStitches(QString original, QString replacement)
{

    undoStack()->beginMacro(tr("replace stitches"));
    foreach(QGraphicsItem *i, items()) {
        if(!i)
            continue;
        if(i->type() != Cell::Type)
            continue;

        Cell *c = qgraphicsitem_cast<Cell*>(i);
        if(!c)
            continue;

        if(c->stitch()->name() == original) {
            undoStack()->push(new SetCellStitch(c, replacement));
        }

    }
    undoStack()->endMacro();

}

void Scene::replaceColor(QColor original, QColor replacement, int selection)
{
    undoStack()->beginMacro(tr("replace color"));
    foreach(QGraphicsItem *i, items()) {
        if(!i)
            continue;
        if(i->type() != Cell::Type)
            continue;

        Cell *c = qgraphicsitem_cast<Cell*>(i);
		
		if(!c)
            continue;

        if(selection == 1 || selection == 3) {
            if(c->color().name() == original.name()) {
                undoStack()->push(new SetCellColor(c, replacement));
            }
        }

        if(selection == 2 || selection == 3) {
            if(c->bgColor().name() == original.name()) {
                undoStack()->push(new SetCellBgColor(c, replacement));
            }
        }

    }
    undoStack()->endMacro();
}
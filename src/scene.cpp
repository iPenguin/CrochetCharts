/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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

#include <QKeyEvent>
#include "stitchlibrary.h"

#include <QTextCursor>
#include <QAction>
#include <QMenu>

static void qNormalizeAngle(qreal &angle)
{
    while (angle < 0.0)
        angle += 360.0;
    while (angle > 360.0)
        angle -= 360.0;
}

Scene::Scene(QObject* parent) :
    QGraphicsScene(parent),
    mCurItem(0),
    mSclItem(0),
    mCellStartPos(QPointF(0,0)),
    mLeftButtonDownPos(QPointF(0,0)),
    mCurIndicator(0),
    mDiff(QSizeF(0,0)),
    mRubberBand(0),
    mRubberBandStart(QPointF(0,0)),
    mMoving(false),
    mIsRubberband(false),
    mHasSelection(false),
    mSnapTo(false),
    mMode(Scene::StitchEdit),
    mEditStitch("ch"),
    mEditFgColor(QColor(Qt::black)),
    mEditBgColor(QColor(Qt::white)),
    mOldScale(QPointF(1.0, 1.0)),
    mAngle(0.0),
    mOrigin(0,0),
    mRowSpacing(9),
    mDefaultSize(QSizeF(32.0, 96.0)),
    mDefaultStitch("ch"),
    mRowLine(0),
    mCenterSymbol(0),
    mShowChartCenter(false),
    mShowGuidelines("")
{
    mPivotPt = QPointF(mDefaultSize.width()/2, mDefaultSize.height());

}

Scene::~Scene()
{

}

QStringList Scene::modes()
{
    QStringList modes;
    modes << tr("Stitch Edit") << tr("Color Edit") << tr("Row Edit")
            << tr("Rotation Edit") << tr("Scale Edit") << tr("Indicator Edit");
    return modes;
}

void Scene::initDemoBackground()
{

    if(Settings::inst()->isDemoVersion()) {

        if(mDemoItems.count() > 0) {
            foreach(QGraphicsItem* i, mDemoItems) {
                delete i;
            }
            mDemoItems.clear();
        }
        
        double fontSize = 32.0;
        QFont demoFont = QFont();
        demoFont.setPointSize(fontSize);
        QString demoString = AppInfo::inst()->demoString;

        QFontMetrics fm = QFontMetrics(demoFont);
        double stringWidth = fm.width(demoString);

        QGraphicsSimpleTextItem* demoText;
        QRectF rect = sceneRect();
        double demoRows = rect.height() / fontSize;
        demoRows = demoRows /2.0;

        double demoCols = rect.width() / stringWidth;

        for(int c = 0; c < ceil(demoCols); ++c) {
            for(int i = 0; i < ceil(demoRows); ++i) {
                demoText = addSimpleText(demoString, demoFont);
                demoText->setBrush(QBrush(QColor("#ECECEC")));
                QPointF point = QPointF(rect.left() + c * stringWidth, rect.top() + i * (2 * fontSize));
                demoText->setPos(point);
                demoText->setZValue(-1);
                mDemoItems.append(demoText);
            }
        }

        //restore original rect. letting the demo text overflow off the scene.
        setSceneRect(rect);
    }
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

    connect(copyAction, SIGNAL(triggered()), SLOT(copy()));
    connect(cutAction, SIGNAL(triggered()), SLOT(cut()));
    connect(pasteAction, SIGNAL(triggered()), SLOT(paste()));
    
    menu.addAction(copyAction);
    menu.addAction(cutAction);
    menu.addAction(pasteAction);

    menu.exec(e->screenPos());

    e->accept();
}

void Scene::keyReleaseEvent(QKeyEvent* keyEvent)
{
    
    QGraphicsScene::keyReleaseEvent(keyEvent);
    
    if(keyEvent->isAccepted())
        return;

    if(keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
        QList<QGraphicsItem*> items = selectedItems();
        undoStack()->beginMacro("remove items");
        foreach(QGraphicsItem* item, items) {

            switch(item->type()) {
                case Cell::Type: {
                    Cell* c = qgraphicsitem_cast<Cell*>(item);
                    undoStack()->push(new RemoveCell(this, c));
                    break;
                }
                case Indicator::Type: {
                    Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                    undoStack()->push(new RemoveIndicator(this, i));
                    break;
                }
                case ItemGroup::Type: {
                    ItemGroup* group = qgraphicsitem_cast<ItemGroup*>(item);
                    undoStack()->push(new RemoveGroup(this, group));
                    break;
                }
                default:
                    qWarning() << "keyReleaseEvent - unknown type: " << item->type();
                    break;
            }
        }
        undoStack()->endMacro();
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
        undoStack()->push(new SetItemCoordinates(this, i, oldPos));
    }
    undoStack()->endMacro();
    
}

void Scene::angleModeKeyRelease(QKeyEvent* keyEvent)
{
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
        
        qreal oldAngle = c->rotation();
        c->setRotation(c->rotation() + delta);
        QPointF pvtPt = QPointF(c->boundingRect().width()/2, c->boundingRect().bottom());
        undoStack()->push(new SetItemRotation(this, c, oldAngle, pvtPt));
    }
    undoStack()->endMacro();

}

void Scene::scaleModeKeyRelease(QKeyEvent* keyEvent)
{
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
        
        QPointF oldScale = c->scale();
        QPointF pvtPt = QPointF(c->boundingRect().width()/2, c->boundingRect().bottom());
        QPointF newScale = oldScale + delta;
        c->setScale(newScale.x(), newScale.y());
        undoStack()->push(new SetItemScale(this, c, oldScale, pvtPt));
    }
    undoStack()->endMacro();
    
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{

    if(selectedItems().count() > 0)
        mHasSelection = true;

    if(mHasSelection && e->modifiers() == Qt::ControlModifier)
        mSelectionPath = selectionArea();
    if(e->buttons() & Qt::LeftButton)
        QGraphicsScene::mousePressEvent(e);

    //FIXME: there has to be a better way to keep the current selection.
    if(mHasSelection && e->modifiers() == Qt::ControlModifier)
        setSelectionArea(mSelectionPath);
    
    mLeftButtonDownPos = e->buttonDownPos(Qt::LeftButton);

    mMoving = false;
    mIsRubberband = false;
    
    mCurItem = itemAt(e->scenePos());

    if(mCurItem) {
        
        switch(mCurItem->type()) {
            case Cell::Type: {
                mCellStartPos = mCurItem->scenePos();
                mDiff = QSizeF(e->scenePos().x() - mCellStartPos.x(), e->scenePos().y() - mCellStartPos.y());
                mSclItem = dynamic_cast<Item*>(mCurItem);
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
                mMoving = true;
                mSclItem = dynamic_cast<Item*>(mCurItem);
                break;
            }
            case QGraphicsSimpleTextItem::Type: {
                //If we've selected the background text pretend we didn't select it.
                mCurItem = 0;
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
            return;
        default:
            break;
    }
    
    if(e->buttons() != Qt::LeftButton)
        return;

    if(selectedItems().count() <= 0 || e->modifiers() == Qt::ControlModifier) {
        
        ChartView* view = qobject_cast<ChartView*>(parent());

        if(!mRubberBand)
            mRubberBand = new QRubberBand(QRubberBand::Rectangle, view);

        mRubberBandStart = view->mapFromScene(e->scenePos());

        mRubberBand->setGeometry(QRect(mRubberBandStart.toPoint(), QSize()));
        mRubberBand->show();
    } else {
        
    //Track object movement on scene.
        foreach(QGraphicsItem* item, selectedItems()) {
            mOldPositions.insert(item, item->pos());
        }
    }

}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
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
            if(mRubberBand) {
                QGraphicsView* view = views().first();
                QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

                mRubberBand->setGeometry(rect.normalized());
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

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
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

    if(mIsRubberband) {
        ChartView* view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

        QPolygonF p = view->mapToScene(rect.normalized());
        QPainterPath path;
        path.addPolygon(p);

        //if user is holding down ctrl add items to existing selection.
        if(mHasSelection && e->modifiers() == Qt::ControlModifier) {
            path.addPath(mSelectionPath);
        }

        setSelectionArea(path);
        mRubberBand->hide();
    }

    if((selectedItems().count() > 0 && mOldPositions.count() > 0) && mMoving) {
        undoStack()->beginMacro("move items");
        foreach(QGraphicsItem* item, selectedItems()) {
            if(mOldPositions.contains(item)) {
                QPointF oldPos = mOldPositions.value(item);
                undoStack()->push(new SetItemCoordinates(this, item, oldPos));
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
    
    delete mRubberBand;
    mRubberBand = 0;

    if(mMoving) {
        //update the size of the scene rect based on where the items are on the scene.
        updateSceneRect();

        initDemoBackground();
        mMoving = false;
    }
    mSclItem = 0;
    mCurItem = 0;
    mHasSelection = false;
}

void Scene::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    if(!mCurItem || mCurItem->type() != Cell::Type)
        return;

    mMoving = false;
/*
 *FIXME: if the user isn't dragging a stitch we should be painting with color.
    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellBgColor(this, mCurCell, mEditBgColor));
*/
}

void Scene::colorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurItem || mCurItem->type() != Cell::Type)
        return;
    
    Cell* curCell = static_cast<Cell*>(mCurItem);
    if(curCell->color() != mEditBgColor)
        undoStack()->push(new SetCellColor(this, curCell, mEditBgColor));
}

void Scene::indicatorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    if(!mCurItem)
        return;
    
    if(mCurItem->type() == Cell::Type || mCurItem->type() == Indicator::Type)
        mMoving = true;

}

void Scene::indicatorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: find a better way other then using ctrl.
    //if right click or ctrl-click remove the indicator.
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

    if(!mCurIndicator && !mHasSelection && !mIsRubberband) {

        QPointF pt = e->buttonDownScenePos(Qt::LeftButton);
        //FIXME: dont hard code the offset for the indicator.
        pt = QPointF(pt.x() - 10, pt.y() - 10);

        undoStack()->push(new AddIndicator(this, pt));

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

void Scene::angleModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurItem)
        return;

    mOldAngle = mCurItem->rotation();
    mPivotPt = QPointF(mCurItem->boundingRect().width()/2, mCurItem->boundingRect().bottom());
    mOrigin = mCurItem->mapToScene(mPivotPt);
}

void Scene::angleModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(!mCurItem)
        return;

    if(selectedItems().count() > 1 || !mCurItem) {
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

    mAngle = mOldAngle + (angle1 - angle2);

    qreal diff = fmod(mAngle, 45.0);
    qreal comp = abs(diff);
    if(comp < 4 /*&& !mSnapTo*/) {
        qreal div = mAngle - diff;
        mAngle = div;
/*FIXME: figure out a way to allow moving back with out snapping.
        mSnapTo = true;
    } else if(comp >= 4 && mSnapTo) {
        mSnapTo = false;
*/
    }
    
    qNormalizeAngle(mAngle);

    mCurItem->setTransformOriginPoint(mPivotPt);
    mCurItem->setRotation(mAngle);

}

void Scene::angleModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurItem)
        return;

    if(mMoving)
        return;

    undoStack()->push(new SetItemRotation(this, mCurItem, mOldAngle, mPivotPt));
    mOldAngle = 0;
}

void Scene::scaleModeMousePress(QGraphicsSceneMouseEvent* e)
{

    Q_UNUSED(e);
    if(!mCurItem && !mSclItem)
        return;

    mOldScale = mSclItem->scale();
    mPivotPt = QPointF(mCurItem->sceneBoundingRect().width()/2, mCurItem->sceneBoundingRect().height());
    qDebug() << mPivotPt;
}

void Scene::scaleModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurItem)
        return;

    if(selectedItems().count() > 1 || !mSclItem) {
        mMoving = true;
        return;
    }

    mMoving = false;

    QPointF delta = e->scenePos() - e->buttonDownScenePos(Qt::LeftButton);

    QSize oldSize = QSize(mSclItem->origWidth * mOldScale.x(), mSclItem->origHeight * mOldScale.y());
    QSize newSize = QSize(oldSize.width() + delta.x(), oldSize.height() + delta.y());
qDebug() << oldSize << newSize;
    if((newSize.width() < 1 && newSize.width() > -1) || (newSize.height() < 1 && newSize.height() > -1))
        return;

    QPointF baseScale = QPointF(newSize.width() / mSclItem->origWidth,
                               newSize.height() / mSclItem->origHeight);

    if(e->modifiers() == Qt::ControlModifier) {
        baseScale.ry() = baseScale.rx();
    }

    mSclItem->setScale(baseScale.x(), baseScale.y());
}

void Scene::scaleModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
    Q_UNUSED(e);
    if(!mSclItem)
        return;

    if(mMoving)
        return;

    undoStack()->push(new SetItemScale(this, mSclItem, mOldScale, mPivotPt));
    
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
    
    mMoving = true;
}

void Scene::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(!mIsRubberband && !mMoving && !mHasSelection) {

        if(mCurItem)
            return;

        if(e->button() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {

            AddCell* addCell = new AddCell(this, e->scenePos());
            undoStack()->push(addCell);
            addCell->cell()->setStitch(mEditStitch);

        }
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
        undoStack()->push(new SetItemCoordinates(this, i, oldPos));
        
    }
    undoStack()->endMacro();
    
}

QPointF Scene::calcGroupPos(QGraphicsItem* group, QPointF newScenePos)
{
    QPointF origin = group->sceneBoundingRect().topLeft() - group->scenePos();
    QPointF delta = newScenePos - origin;
    return delta;
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

        undoStack()->push(new SetItemCoordinates(this, i, oldPos));
    }
    undoStack()->endMacro();
}

void Scene::alignToPath()
{

}

void Scene::distributeToPath()
{

}

void Scene::createRowsChart(int rows, int cols, QString defStitch, QSizeF rowSize)
{
    
    mDefaultSize = rowSize;
    mDefaultStitch = defStitch;
    arrangeGrid(QSize(rows, cols), QSize(1, 1), rowSize.toSize(), false);

    updateSceneRect();
    
    initDemoBackground();
    
}

void Scene::createBlankChart()
{
    initDemoBackground();
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

void Scene::mirror(int direction)
{
    if(selectedItems().count() <= 0)
        return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QRectF rect = selectedItemsBoundingRect(selectedItems());
    QList<QGraphicsItem*> list = selectedItems();

    clearSelection();

    undoStack()->beginMacro("mirror selection");
    if(direction == 1) { //left

        foreach(QGraphicsItem* item, list) {
            if(item->type() == Cell::Type) {
                Cell* c = qgraphicsitem_cast<Cell*>(item);
                QPointF oldPos = c->pos();

                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                Cell* copy = c->copy(addCellCmd->cell());
                
                qreal diff = (rect.left() - c->pos().x()) - c->boundingRect().width();
                copy->setPos(rect.left() + diff, c->pos().y());

                qreal newAngle = 360 - copy->rotation();
                qNormalizeAngle(newAngle);
                copy->setRotation(newAngle);
                copy->setSelected(true);

            }
        }

    } else if(direction == 2) { //right

        foreach(QGraphicsItem* item, list) {
            if(item->type() == Cell::Type) {
                Cell* c = qgraphicsitem_cast<Cell*>(item);
                QPointF oldPos = c->pos();

                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                Cell* copy = c->copy(addCellCmd->cell());

                qreal diff = (rect.right() - c->pos().x()) - c->boundingRect().width();
                copy->setPos(rect.right() + diff, c->pos().y());

                qreal newAngle = 360 - copy->rotation();
                qNormalizeAngle(newAngle);
                copy->setRotation(newAngle);
                copy->setSelected(true);
            }
        }

    } else if(direction == 3) { //up

        foreach(QGraphicsItem* item, list) {
            if(item->type() == Cell::Type) {
                Cell* c = qgraphicsitem_cast<Cell*>(item);
                QPointF oldPos = item->pos();

                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                Cell* copy = c->copy(addCellCmd->cell());

                qreal diff = (rect.top() - c->pos().y()) - c->boundingRect().height();
                copy->setPos(c->pos().x(), rect.top() + diff - copy->boundingRect().height());

                undoStack()->push(new SetItemCoordinates(this, copy, oldPos));
                qreal newAngle = 360 - copy->rotation() + 180;
                qNormalizeAngle(newAngle);
                copy->setTransformOriginPoint(copy->boundingRect().width()/2, copy->boundingRect().height());
                copy->setRotation(newAngle);
                copy->setSelected(true);
            }
        }

    } else if(direction == 4) { //down

        foreach(QGraphicsItem* item, list) {
            if(item->type() == Cell::Type) {
                Cell* c = qgraphicsitem_cast<Cell*>(item);
                QPointF oldPos = item->pos();
                
                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                Cell* copy = c->copy(addCellCmd->cell());
                
                qreal diff = (rect.bottom() - c->pos().y()) - c->boundingRect().height();
                copy->setPos(c->pos().x(), rect.bottom() + diff - copy->boundingRect().height());

                undoStack()->push(new SetItemCoordinates(this, copy, oldPos));

                qreal newAngle = 360 - copy->rotation() + 180;
                qNormalizeAngle(newAngle);
                copy->setTransformOriginPoint(copy->boundingRect().width()/2, copy->boundingRect().height());
                copy->setRotation(newAngle);
                copy->setSelected(true);
            }
        }
    }
    undoStack()->endMacro();

    QApplication::restoreOverrideCursor();
}

void Scene::rotate(qreal degrees)
{
    if(selectedItems().count() <= 0)
        return;

    undoStack()->push(new SetItemsRotation(this, selectedItems(), degrees));
}

void Scene::rotateSelection(qreal degrees, QList<QGraphicsItem*> items, QPointF pivotPoint)
{

    qreal newAngle = degrees;
    qNormalizeAngle(newAngle);

    QGraphicsItemGroup* g = createItemGroup(items);
    g->setTransformOriginPoint(pivotPoint);
    g->setRotation(newAngle);
    destroyItemGroup(g);

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
                stream << c->name() << c->bgColor()
                    << c->rotation() << c->scale() << c->transformOriginPoint() << c->pos()
                    << c->transform();
                break;
            }
            case Indicator::Type: {
                Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                stream << i->scenePos() << i->text();
                break;
            }
            case ItemGroup::Type: {
                ItemGroup* group = qgraphicsitem_cast<ItemGroup*>(item);
                stream << group->childItems().count();
                copyRecursively(stream, group->childItems());
                break;
            }
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
    QList<QGraphicsItem*> items;
    for(int i = 0; i < count; ++i) {
        pasteRecursively(stream, &items);
    }

    foreach(QGraphicsItem* item, items)
        item->setSelected(true);
    
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
            QColor bgColor;
            qreal angle;
            QPointF scale;
            QPointF pos, transPoint;
            QTransform trans;

            stream >> name >> bgColor >> angle >> scale >> transPoint >> pos >> trans;
            pos += offSet;
            AddCell* addCmd = new AddCell(this, pos);
            undoStack()->push(addCmd);
            Cell* c = addCmd->cell();

            c->setStitch(name);
            c->setBgColor(bgColor);

            c->setTransformOriginPoint(transPoint);
            c->setRotation(angle);
            c->setTransform(trans);

            c->setSelected(false);
            group->append(c);
            break;
        }
        case Indicator::Type: {
            Indicator* i = new Indicator();
            //FIXME: add this indicator to the undo stack.
            QPointF pos;
            QString text;

            stream >> pos >> text;
            pos += offSet;
            i->setText(text);
            addItem(i);
            i->setPos(pos);
            
            i->setSelected(false);
            group->append(i);
            break;
        }
        case ItemGroup::Type: {
            int childCount;
            stream >> childCount;
            QList<QGraphicsItem*> items;
            for(int i = 0; i < childCount; ++i) {
                pasteRecursively(stream, &items);
            }

            GroupItems* grpItems = new GroupItems(this, items);
            undoStack()->push(grpItems);
            ItemGroup* g = grpItems->group();

            g->setSelected(false);
            group->append(g);
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
    foreach(QGraphicsItem* item, selectedItems()) {

        switch(item->type()) {
            case Cell::Type: {
                Cell* c = qgraphicsitem_cast<Cell*>(item);
                undoStack()->push(new RemoveCell(this, c));
                break;
            }
            case Indicator::Type: {
            Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                undoStack()->push(new RemoveIndicator(this, i));
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
        foreach(QGraphicsItem *i, items) {
            i->setSelected(false);
            g->addToGroup(i);
        }
    } else {
        foreach(QGraphicsItem* i, items) {
            i->setSelected(false);
            g->addToGroup(i);
        }
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

    mGroups.removeOne(group);
    foreach(QGraphicsItem* item, group->childItems()) {
        group->removeFromGroup(item);
    }

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

    QList<QGraphicsItem*>::const_iterator dItem;
    for (dItem = mDemoItems.begin(); dItem != mDemoItems.constEnd(); ++dItem) {
        itemList.removeOne((*dItem));
    }

    QRectF rect = selectedItemsBoundingRect(itemList);
    return rect;
    
}

void Scene::updateSceneRect()
{
    QRectF ibr = itemsBoundingRect();
    QRectF sbr = sceneRect();
    QRectF final;

    ibr.setTop(ibr.top() + 50);
    ibr.setBottom(ibr.bottom() + 50);
    ibr.setLeft(ibr.left() + 50);
    ibr.setRight(ibr.right() + 50);
    
    final.setBottom((ibr.bottom() >= sbr.bottom()) ? ibr.bottom() : sbr.bottom());
    final.setTop((ibr.top() <= sbr.top()) ? ibr.top() : sbr.top());
    final.setLeft((ibr.left() <= sbr.left()) ? ibr.left() : sbr.left());
    final.setRight((ibr.right() >= sbr.right()) ? ibr.right() : sbr.right());

    setSceneRect(final);
    
}

/*************************************************\
| Rounds Specific functions:                      |
\*************************************************/

bool Scene::showChartCenter()
{
    return mShowChartCenter;
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
    
    initDemoBackground();

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
        c->setStitch(stitch, (row % 2));
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

void Scene::highlightIndicators(bool state)
{
    foreach(Indicator* i, mIndicators) {
        if(!state)
            i->setTextInteractionFlags(Qt::NoTextInteraction);
        i->highlight = state;
        i->update();
    }
}

void Scene::setShowGuidelines(QString guides)
{

    if(mShowGuidelines != guides) {
        mShowGuidelines = guides;
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

void Scene::updateGuidelines()
{
    if(showGuidelines() == tr("None"))
        return;

    if(mCenterSymbol && mCenterSymbol->isVisible()) {
        DEBUG("TODO: Create guidelines from the center symbol");
        
    } else {
        DEBUG("TODO: Create guidelines from the center of the chart");
    }
}

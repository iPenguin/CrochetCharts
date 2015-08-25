#include "selectionband.h"
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QBrush>
#include <cmath>
#include <QApplication>

/************************************************\ 
*  AbstractSelectionBand                         * 
\************************************************/ 
AbstractSelectionBand::AbstractSelectionBand(QWidget* parent)
	: QWidget(parent)
{}

void AbstractSelectionBand::setDefaultStyle(QPainter& p)
{
	QBrush b;
	b = (QApplication::palette().highlight());
	
	p.setBrush(b);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setRenderHint(QPainter::TextAntialiasing, false);
	p.setRenderHint(QPainter::SmoothPixmapTransform, false);
	p.setRenderHint(QPainter::HighQualityAntialiasing, false);
	p.setRenderHint(QPainter::NonCosmeticDefaultPen, true);
	p.setPen(QPen(Qt::black, 1));
	p.setOpacity(80.0/255.0);
}

void AbstractSelectionBand::setPosition(const QPoint& pos)
{
	mPosition = pos;
}

/************************************************\ 
*  RubberBand                                    * 
\************************************************/ 
RubberBand::RubberBand(QWidget* parent)
	: AbstractSelectionBand(parent)
{
	setAttribute(Qt::WA_TransparentForMouseEvents);
	#ifndef Q_WS_WIN
    setAttribute(Qt::WA_NoSystemBackground);
	#endif //Q_WS_WIN
    setAttribute(Qt::WA_WState_ExplicitShowHide);
}

void RubberBand::reset()
{
	setPosition(QPoint(0, 0));
	mMouseNow = QPointF(0, 0);
	resetPath();
}

void RubberBand::moveMouseTo(const QPointF& pos)
{
	mMouseNow = pos;
	updatePath();
}

QPainterPath RubberBand::path()
{
	//move the path so it accounts for the position of the selectionband
	return mPath.translated(getTopLeft());
}

void RubberBand::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
	setDefaultStyle(painter);
	painter.drawPath(mPath);
}

QPoint RubberBand::getTopLeft() const
{
	int width = (mMouseNow.x() - mPosition.x());
	int height = (mMouseNow.y() - mPosition.y());
	
	QRect newGeom(mPosition.x(), mPosition.y(), width, height);
	return newGeom.normalized().topLeft();
}

void RubberBand::updatePath()
{
	//we have to reset the path to remove the old rectangle
	resetPath();
	
	int width = (mMouseNow.x() - mPosition.x());
	int height = (mMouseNow.y() - mPosition.y());
	
	QRect newGeom(mPosition.x(), mPosition.y(), width, height);
	newGeom = newGeom.normalized();
	setGeometry(newGeom);

	mPath.addRect(0.5, 0.5, newGeom.width()-1, newGeom.height()-1);
}
	
void RubberBand::resetPath()
{
	mPath = QPainterPath();
}	

/************************************************\ 
*  LassoBand                                     * 
\************************************************/ 
LassoBand::LassoBand(QWidget* parent)
	: AbstractSelectionBand(parent)
{
	setAttribute(Qt::WA_TransparentForMouseEvents);
	#ifndef Q_WS_WIN
    setAttribute(Qt::WA_NoSystemBackground);
	#endif //Q_WS_WIN
    setAttribute(Qt::WA_WState_ExplicitShowHide);
}
	
void LassoBand::moveMouseTo(const QPointF& pos)
{
	mMouseNow = pos;
	updatePath();
}

void LassoBand::reset()
{
	mPosition = QPoint(0, 0);
	mMouseNow = QPointF(0, 0);
	mCurPainterTranslation = QPoint(0, 0);
	mMaxGeom = QRect(0, 0, 0, 0);
	resetPath();
}
	
QPainterPath LassoBand::path()
{
	//move the path so it accounts for the position of the selectionband
	QPainterPath temppath = mPath;
	temppath.translate(getTopLeft());
	return temppath;
}

void LassoBand::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
	setDefaultStyle(painter);
	//updatePath();
	//painter.setOpacity(0.5);
	
	painter.drawPath(mPath);
}

void LassoBand::updatePath()
{
	//get the difference between the position and the current mouse position
	QPointF diff = mMouseNow - mPosition;
	
	//get the rectangle from that diff to the center
	QRect newr(0, 0, diff.x(), diff.y());
	newr = newr.normalized();
	
	//expand our size rectangle to contain newr
	mMaxGeom = mMaxGeom.unite(newr).normalized();
	
	//set the geometry to that rectangle, translated to our position
	setGeometry(mMaxGeom.translated(mPosition.x(), mPosition.y()).normalized());
	
	//translate the painter to be offset from the topleft of mMaxSize
	QPoint targetOffset = mMaxGeom.topLeft();
	QPoint remainingOffset = mCurPainterTranslation - targetOffset;	
	mPath.translate(remainingOffset);
	mCurPainterTranslation = targetOffset;
	
	//update the path by drawing a line to diff
	mPath.lineTo(diff - mCurPainterTranslation - QPointF(0.5, 0.5));
	
	repaint();
}

void LassoBand::resetPath()
{
	mPath = QPainterPath();
	mPath.moveTo(0, 0);
	mCurPainterTranslation = QPoint(0, 0);
}

QPoint LassoBand::getTopLeft() const
{
	return mMaxGeom.translated(mPosition).topLeft();
}
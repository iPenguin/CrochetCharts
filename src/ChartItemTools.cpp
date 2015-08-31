#include "ChartItemTools.h"
#include <cmath>
#include <QList>
#include <QTransform>
#include <QVector2D>

#define ROTATION_MATRIX_INDEX 	0
#define SCALE_MATRIX_INDEX 		1

qreal constrainAngle180(qreal x) {
    x = fmod(x + 180,360);
    if (x < 0)
        x += 360;
	return x - 180;
}	

qreal constrainAngle360(qreal x) {
    x = fmod(x,360);
    if (x < 0)
        x += 360;
	return x;
}	
	

qreal ChartItemTools::getRotation(QGraphicsItem* item)
{
	return getGraphicsRotation(item)->angle();
}

void ChartItemTools::setRotation(QGraphicsItem* item, qreal rotation)
{
	getGraphicsRotation(item)->setAngle(rotation);
}

void ChartItemTools::addRotation(QGraphicsItem* item, qreal rotation)
{
	QGraphicsRotation* r = getGraphicsRotation(item);
	r->setAngle(r->angle() + rotation);
}

QPointF ChartItemTools::getRotationPivot(QGraphicsItem* item)
{
	return getGraphicsRotation(item)->origin().toPointF();
}

void ChartItemTools::setRotationPivot(QGraphicsItem* item, QPointF pivot, bool reposition)
{
	QGraphicsRotation* r = getGraphicsRotation(item);
	
	if (reposition) {
		//get the new pivot point on the scaled stitch
		QPointF newPivotLocation = mapToRotation(item, pivot);
		QPointF diff = newPivotLocation - pivot;
		//translate by that diff, to offset the translation caused by moving the pivot
		item->moveBy(diff.x(), diff.y());
	}
	
	r->setOrigin(QVector3D(pivot));
}

void ChartItemTools::addRotationPivot(QGraphicsItem* item, QPointF pivot, bool reposition)
{
	QGraphicsRotation* r = getGraphicsRotation(item);
	QPointF curPivot = r->origin().toPointF();
	setRotationPivot(item, curPivot + pivot, reposition);
}

qreal ChartItemTools::getScaleX(QGraphicsItem* item)
{
	return getGraphicsScale(item)->xScale();
}

void ChartItemTools::setScaleX(QGraphicsItem* item, qreal scaleX)
{
	getGraphicsScale(item)->setXScale(scaleX);
}

void ChartItemTools::addScaleX(QGraphicsItem* item, qreal scaleX)
{
	QGraphicsScale* s = getGraphicsScale(item);
	s->setXScale(s->xScale() + scaleX);
}

qreal ChartItemTools::getScaleY(QGraphicsItem* item)
{
	return getGraphicsScale(item)->yScale();
}

void ChartItemTools::setScaleY(QGraphicsItem* item, qreal scaleY)
{
	getGraphicsScale(item)->setYScale(scaleY);
}

void ChartItemTools::addScaleY(QGraphicsItem* item, qreal scaleY)
{
	QGraphicsScale* s = getGraphicsScale(item);
	s->setYScale(s->yScale() + scaleY);
}

QPointF ChartItemTools::getScale(QGraphicsItem* item)
{
	QGraphicsScale* s = getGraphicsScale(item);
	return QPointF(s->xScale(), s->yScale());
}

QPointF ChartItemTools::getScalePivot(QGraphicsItem* item)
{
	return getGraphicsScale(item)->origin().toPointF();
}

void ChartItemTools::setScalePivot(QGraphicsItem* item, QPointF pivot, bool reposition)
{
	item->setScale(1);
	QGraphicsScale* r = getGraphicsScale(item);
	
	if (reposition) {
		//get the new pivot point on the scaled stitch
		QPointF newPivotLocation = mapToRotationAndScale(item, pivot);
		QPointF diff = newPivotLocation - mapToRotation(item, pivot);
		//translate by that diff, to offset the translation caused by moving the pivot
		item->moveBy(diff.x(), diff.y());
	}
	
	r->setOrigin(QVector3D(pivot));
}

void ChartItemTools::addScalePivot(QGraphicsItem* item, QPointF pivot, bool reposition)
{
	QGraphicsScale* r = getGraphicsScale(item);
	QPointF curPivot = r->origin().toPointF();
	setScalePivot(item, curPivot + pivot, reposition);
}

QPointF ChartItemTools::mapToRotation(QGraphicsItem* item, QPointF point)
{
	//create an identity matrix
	QMatrix4x4 mat;
	//rotate it by the graphicsrotation
	getGraphicsRotation(item)->applyTo(&mat);
	//return the point mapped by this matrix
	return mat.map(point);
}

QPointF ChartItemTools::mapToScale(QGraphicsItem* item, QPointF point)
{
	//create an identity matrix
	QMatrix4x4 mat;
	//scale it by the graphicsrotation
	getGraphicsScale(item)->applyTo(&mat);
	//return the point mapped by this matrix
	return mat.map(point);
}

QPointF ChartItemTools::mapToRotationAndScale(QGraphicsItem* item, QPointF point)
{
	//create an identity matrix
	QMatrix4x4 mat;
	//rotate  and scale it
	getGraphicsRotation(item)->applyTo(&mat);
	getGraphicsScale(item)->applyTo(&mat);
	//return the point mapped by this matrix
	return mat.map(point);
}

void ChartItemTools::initIfNotAlreadyInitialised(QGraphicsItem* item)
{
	QList<QGraphicsTransform*> transforms = item->transformations();
	if (transforms.length() == 0) {
		transforms.append(new QGraphicsRotation());
		transforms.append(new QGraphicsScale());
		item->setTransformations(transforms);
	} else if (transforms.length() == 1) {
		transforms.append(new QGraphicsScale());
		item->setTransformations(transforms);
	}
}

QGraphicsRotation* ChartItemTools::getGraphicsRotation(QGraphicsItem* item)
{
	return static_cast<QGraphicsRotation*>(getGraphicsTransformations(item)[ROTATION_MATRIX_INDEX]);
}

QGraphicsScale* ChartItemTools::getGraphicsScale(QGraphicsItem* item)
{
	return static_cast<QGraphicsScale*>(getGraphicsTransformations(item)[SCALE_MATRIX_INDEX]);
}

QList<QGraphicsTransform*> ChartItemTools::getGraphicsTransformations(QGraphicsItem* item)
{
	initIfNotAlreadyInitialised(item);
	QList<QGraphicsTransform*> transforms = item->transformations();
	return transforms;
}

QList<QGraphicsTransform*> ChartItemTools::cloneGraphicsTransformations(QGraphicsItem* item)
{
	QList<QGraphicsTransform*> newitems;
	QGraphicsRotation* oldr = getGraphicsRotation(item);
	QGraphicsScale* olds = getGraphicsScale(item);
	
	QGraphicsRotation* newr = new QGraphicsRotation;
	QGraphicsScale* news = new QGraphicsScale;
	
	newr->setOrigin(oldr->origin());
	newr->setAngle(oldr->angle());
	
	news->setOrigin(olds->origin());
	news->setXScale(olds->xScale());
	news->setYScale(olds->yScale());
	
	newitems.append(newr);
	newitems.append(news);
	return newitems;
}

void ChartItemTools::recalculateTransformations(QGraphicsItem* item)
{
	//plan of action:
	//		1: get the position of the top left corner. this will also be our origin for scale and rotation
	//		2: get the rotation of the item by mapping two corners and calculating the atan2
	//		3: get the scale of the item by mapping three corners and comparing the ratio of the distances
	//		4: reset all transformations of the chartitem
	//		5: apply the new transformations
	
	//calculate the position of the item now
	QPointF oldOrigin = item->mapToScene(0, 0);
	
	//get the positions of three corners
	QPointF topLeftLocal = item->boundingRect().topLeft();
	QPointF bottomLeftLocal = item->boundingRect().bottomLeft();
	QPointF topRightLocal = item->boundingRect().topRight();
	
	//get these positions mapped
	QPointF topLeftMapped = item->mapToScene(topLeftLocal);
	QPointF bottomLeftMapped = item->mapToScene(bottomLeftLocal);
	QPointF topRightMapped = item->mapToScene(topRightLocal);
	
	//get the differences in positions both mapped and local
	QVector2D xDiffMapped = QVector2D(topRightMapped - topLeftMapped);
	QVector2D yDiffMapped = QVector2D(bottomLeftMapped - topLeftMapped);
	QVector2D xDiffLocal = QVector2D(topRightLocal - topLeftLocal);
	QVector2D yDiffLocal = QVector2D(bottomLeftLocal - topLeftLocal);
	
	//calculate the rotation using an arctangens in degrees
	qreal xrotation = std::atan2(xDiffMapped.y(), xDiffMapped.x()) * 180 / M_PI;
	qreal yrotation = std::atan2(yDiffMapped.y(), yDiffMapped.x()) * 180 / M_PI;
	
	//get the lengths of all distances so we can calculate the size ratio
	qreal xDistanceLocal = xDiffLocal.length();
	qreal yDistanceLocal = yDiffLocal.length();
	qreal xDistanceMapped = xDiffMapped.length();
	qreal yDistanceMapped = yDiffMapped.length();
	
	//from this we can calculate the scales
	qreal scaleX = xDistanceMapped / xDistanceLocal;
	qreal scaleY = yDistanceMapped / yDistanceLocal;
	
	//we need to correct the signs of these scales. The is either 90 degrees or 270, but because
	//they are floats its best to do a lesserthan with enough epsilon
	if (constrainAngle360(xrotation - yrotation) <= 91) {
		scaleX = -scaleX;
		xrotation -= 180;
	}
	
	//now we reset the item
	item->setRotation(0);
	item->setScale(1);
	item->setTransformOriginPoint(0, 0);
	item->resetTransform();
	item->setTransformations(QList<QGraphicsTransform*>());
	
	//and apply the new transformations, first we set the scale
	setScalePivot(item, topLeftLocal);
	setScaleX(item, scaleX);
	setScaleY(item, scaleY);

	//and then the rotation
	setRotationPivot(item, mapToScale(item, topLeftLocal));
	setRotation(item, xrotation);
	
	//get the position of the item after the changes
	QPointF nowOrigin = item->mapToScene(0, 0);
	
	//move the item back to the original point
	item->moveBy(oldOrigin.x() - nowOrigin.x(), oldOrigin.y() - nowOrigin.y());
	
	item->update();
}
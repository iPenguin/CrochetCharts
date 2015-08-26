#include "ChartItemTools.h"
#include <QList>
#include <QTransform>

#define ROTATION_MATRIX_INDEX 	0
#define SCALE_MATRIX_INDEX 		1

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

void ChartItemTools::setRotationPivot(QGraphicsItem* item, QPointF pivot)
{
	QGraphicsRotation* r = getGraphicsRotation(item);
	
	//get the new pivot point on the scaled stitch
	QPointF newPivotLocation = mapToRotation(item, pivot);
	//QPointF currentPivotLocation = mapToScale(item, r->origin().toPointF());
	QPointF diff = newPivotLocation - pivot;
	//translate by that diff, to offset the translation caused by moving the pivot
	item->moveBy(diff.x(), diff.y());
	
	r->setOrigin(QVector3D(pivot));
}

void ChartItemTools::addRotationPivot(QGraphicsItem* item, QPointF pivot)
{
	QGraphicsRotation* r = getGraphicsRotation(item);
	QPointF curPivot = r->origin().toPointF();
	setRotationPivot(item, curPivot + pivot);
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

QPointF ChartItemTools::getScalePivot(QGraphicsItem* item)
{
	return getGraphicsScale(item)->origin().toPointF();
}

void ChartItemTools::setScalePivot(QGraphicsItem* item, QPointF pivot)
{
	item->setScale(1);
	QGraphicsScale* r = getGraphicsScale(item);
	
	//get the new pivot point on the scaled stitch
	QPointF newPivotLocation = mapToRotationAndScale(item, pivot);
	//QPointF currentPivotLocation = mapToScale(item, r->origin().toPointF());
	QPointF diff = newPivotLocation - mapToRotation(item, pivot);
	//translate by that diff, to offset the translation caused by moving the pivot
	item->moveBy(diff.x(), diff.y());
	
	r->setOrigin(QVector3D(pivot));
}

void ChartItemTools::addScalePivot(QGraphicsItem* item, QPointF pivot)
{
	QGraphicsScale* r = getGraphicsScale(item);
	QPointF curPivot = r->origin().toPointF();
	setScalePivot(item, curPivot + pivot);
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
	//TODO delete this items when destroying the graphicsitem
	QList<QGraphicsTransform*> transforms = item->transformations();
	if (transforms.length() == 0) {
		transforms.append(new QGraphicsRotation());
		transforms.append(new QGraphicsScale());
		item->setTransformations(transforms);
	} else if (transforms.length() == 1) {
		transforms.append(new QGraphicsScale());
		item->setTransformations(transforms);
	}
	return transforms;
}
#ifndef SELECTIONBAND_H
#define SELECTIONBAND_H

#include <QWidget>
#include <QPainterPath>

class AbstractSelectionBand : public QWidget
{
public:
	AbstractSelectionBand(QWidget* parent);
	
	virtual void moveMouseTo(const QPointF& pos) = 0;
	virtual void setPosition(const QPoint& pos);
	virtual void reset() = 0;
	
	virtual QPainterPath path() = 0;
	
protected:
	void setDefaultStyle(QPainter& p);
	QPoint mPosition;
};


class RubberBand : public AbstractSelectionBand
{
public:
	RubberBand(QWidget* parent);
	
	virtual void moveMouseTo(const QPointF& pos);
	virtual void reset();
	
	virtual QPainterPath path();
	void paintEvent(QPaintEvent * event);
	
private:
	void updatePath();
	void resetPath();
	QPoint getTopLeft() const;
	QPointF mMouseNow;
	QPainterPath mPath;
};


class LassoBand : public AbstractSelectionBand
{
public:
	LassoBand(QWidget* parent);
	
	virtual void moveMouseTo(const QPointF& pos);
	virtual void reset();
	
	virtual QPainterPath path();
	void paintEvent(QPaintEvent * event);

private:
	void updatePath();
	void resetPath();
	QPoint getTopLeft() const;
	
	QPoint mCurPainterTranslation;
	QPointF mMouseNow;
	QRect mMaxGeom;
	QPainterPath mPath;
};


class LineBand : public AbstractSelectionBand
{
public:
	LineBand(QWidget* parent);
	
	virtual void moveMouseTo(const QPointF& pos);
	virtual void reset();
	
	virtual QPainterPath path();
	void paintEvent(QPaintEvent * event);

private:
	void updatePath();
	void resetPath();
	QPoint getTopLeft() const;
	
	QPoint mCurPainterTranslation;
	QPointF mMouseNow;
	QRect mMaxGeom;
	QPainterPath mIntersectionPath;
	QPainterPath mVisiblePath;
};

#endif // SELECTIONBAND_H

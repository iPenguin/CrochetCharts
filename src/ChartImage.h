#ifndef CHARTIMAGE_H
#define CHARTIMAGE_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class ChartImage : public QGraphicsObject
{
	Q_OBJECT
public:
	ChartImage(const QString& filename, QGraphicsItem* parent = 0);
	~ChartImage();

    enum {Type = UserType + 20 };

    QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    int type() const { return Type; }
	
	unsigned int layer() { return mLayer; }
	void setLayer(unsigned int layer) { mLayer = layer; }
	
	const QString& filename() { return mFilename; }
	
private:
	unsigned int mLayer;
	QPixmap* mPixmap;
	QString mFilename;

};

#endif // CHARTIMAGE_H

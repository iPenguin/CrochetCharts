#include "ChartImage.h"
#include "debug.h"
#include <QMessageBox>

ChartImage::ChartImage(const QString& filename, QGraphicsItem* parent):
	QGraphicsObject(parent),
	mLayer(0),
	mFilename(filename)
{
	mPixmap = new QPixmap(filename);
	
    //if the image is invalid
	if (mPixmap->isDetached() || mPixmap->isNull()) {
		//show an error
		QMessageBox message;
		message.setText("The chosen file (" + filename + ") is not a valid image.");
		message.exec();
	}
		
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

ChartImage::~ChartImage()
{
	delete mPixmap;
}

QRectF ChartImage::boundingRect() const
{
	
	if (mPixmap) {
		return mPixmap->rect();
	} else {
		WARN("No pixmap in ChartImage.");
		return QRectF();
	}
}

void ChartImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (mPixmap) {
		painter->drawPixmap(option->rect.x(), option->rect.y(), *mPixmap);
	}
	if(option->state & QStyle::State_Selected) {
		painter->setPen(Qt::DashLine);
		painter->drawRect(option->rect);
		painter->setPen(Qt::SolidLine);
	}
}
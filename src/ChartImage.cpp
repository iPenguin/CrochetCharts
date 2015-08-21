#include "ChartImage.h"
#include "debug.h"
#include <QMessageBox>

ChartImage::ChartImage(const QString& filename, QGraphicsItem* parent):
	QGraphicsObject(parent),
	mLayer(0),
	mFilename(filename)
{
	setZLayer("Background");
	mPixmap = new QPixmap(filename);
	
    //if the image is invalid
	if (mPixmap->isNull()) {
		//show an error
		QMessageBox message;
		message.setText("The chosen file (" + filename + ") is not a valid image.");
		message.exec();
	}
	
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

ChartImage::ChartImage(QDataStream& stream, QGraphicsItem* parent):
	QGraphicsObject(parent),
	mLayer(0),
	mFilename("Loaded from chart")
{
	//load the pixmap from the stream
	mPixmap = new QPixmap();
	stream >> *mPixmap;
	
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

void ChartImage::setFile(const QString& filename)
{
	QPixmap* newPixmap = new QPixmap(filename);
	
	//if the new pixmap is not a valid image, cancel the operation
	if (newPixmap->isNull()) {
		delete newPixmap;
		return;
	}
	
	//otherwise, delete the old pixmap and replace it
	delete mPixmap;
	mPixmap = newPixmap;
	mFilename = filename;
}

void ChartImage::setZLayer(const QString& zlayer)
{
	mZLayer = zlayer;
	
	if (mZLayer == "Foreground") {
		setZValue(11);
	} else if (mZLayer == "Background") {
		setZValue(-2);
	} else {
		WARN("Unknown z value: ");
		WARN(mZLayer);
	}
}
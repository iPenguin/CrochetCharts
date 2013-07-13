#include "colorlistwidget.h"

#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

#include <QDebug>

ColorListWidget::ColorListWidget(QWidget *parent) :
    QListWidget(parent),
    mDragStart(QPointF(0,0))
{
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

}

void ColorListWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        mDragStart = event->pos();

    QListWidget::mousePressEvent(event);
/*
    if(event->button() == Qt::RightButton) {

        emit bgColorSelected(currentIndex());
        event->accept();
    }
*/
}

void ColorListWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
        return;

    if ((e->pos() - mDragStart).manhattanLength()
        < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QColor c = QColor(currentIndex().data(Qt::ToolTipRole).toString());
    QVariant v = c;

    mimeData->setText(c.name());
    mimeData->setColorData(v);
    drag->setMimeData(mimeData);

    /*Qt::DropAction dropAction =*/ drag->exec(Qt::CopyAction | Qt::MoveAction);

    QListWidget::mouseMoveEvent(e);
}

QPixmap ColorListWidget::drawColorBox(QColor color, QSize size)
{
    QPixmap pix = QPixmap(size);
    QPainter p;
    p.begin(&pix);

    if(color.isValid()) {
        p.fillRect(QRect(QPoint(0, 0), size), QColor(color));
    } else {
        p.fillRect(QRect(QPoint(0,0), size), QColor(Qt::white));
        QPen pen;
        pen.setColor(QColor(Qt::black));
        pen.setWidth(3);
        p.setPen(pen);
        p.drawLine(0,0, size.width(),size.height());
        p.drawLine(0,size.height(), size.width(),0);
    }

    p.drawRect(0, 0, size.width() - 1, size.height() - 1);
    p.end();

    return pix;
}

void ColorListWidget::dragEnterEvent(QDragEnterEvent *e)
{
    //Don't accept generic abstract model items.
    if(e->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        e->dropAction();
        return;
    }

    QListWidget::dragEnterEvent(e);
}

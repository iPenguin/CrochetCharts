#ifndef CELL_H
#define CELL_H

#include <QtSvg/QGraphicsSvgItem>

class Cell : public QGraphicsSvgItem
{
    Q_OBJECT
public:
    explicit Cell(const QString fileName, QGraphicsItem *parent = 0);

    QRectF boundingRect () const;
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    int type () const;


signals:

public slots:


private:

};

#endif // CELL_H

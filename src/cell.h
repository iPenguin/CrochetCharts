/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CELL_H
#define CELL_H

#include <QtSvg/QGraphicsSvgItem>

class Cell : public QGraphicsSvgItem
{
    Q_OBJECT

public:

    enum {Type = UserType + 1 };
    
    explicit Cell(const QString fileName, QGraphicsItem *parent = 0);
    Cell(QGraphicsItem *parent = 0);

    QRectF boundingRect () const;
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    int type () const;

    void setRotation(double rotation) { mRotation = rotation; }
    double rotation() const { return mRotation; }

    void setAngle(double angle) { mAngle = angle; }
    double angle() const { return mRotation; }

signals:

public slots:


private:
    double mRotation;
    double mAngle;
};

#endif // CELL_H

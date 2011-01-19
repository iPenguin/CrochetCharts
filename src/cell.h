/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CELL_H
#define CELL_H

#include <QtSvg/QGraphicsSvgItem>
#include "stitch.h"

class Cell : public QGraphicsSvgItem
{
    Q_OBJECT
    friend class SaveFile;
public:

    enum {Type = UserType + 1 };
    
    explicit Cell(QGraphicsItem* parent = 0);
    ~Cell();

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    int type () const;

    void setRotation(double rotation) { mRotation = rotation; }
    double rotation() const { return mRotation; }

    void setAngle(double angle) { mAngle = angle; }
    double angle() const { return mRotation; }

    void setStitch(Stitch* s);
    Stitch* stitch() const { return mStitch; }
    
signals:

public slots:

private:
    double mRotation;
    double mAngle;

    Stitch* mStitch;
};

#endif // CELL_H

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

    void setRotation(qreal rotation);
    qreal rotation() const { return mRotation; }

    void setAngle(double angle) { mAngle = angle; }
    double angle() const { return mAngle; }

    void setStitch(Stitch* s);
    void setStitch(QString s);
    Stitch* stitch() const { return mStitch; }

    QString name();
    
signals:

public slots:

private:
    qreal mRotation;
    double mAngle;

    Stitch* mStitch;
};

#endif // CELL_H

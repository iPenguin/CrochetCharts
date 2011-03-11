/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
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

    enum { Type = UserType + 1 };
    
    explicit Cell(QGraphicsItem* parent = 0);
    ~Cell();

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    int type () const { return Cell::Type; }

    void setAngle(double angle) { mAngle = angle; }
    double angle() const { return mAngle; }

    void setColor(QColor c = QColor(Qt::white));
    QColor color() const { return mColor; }
    
    void setStitch(Stitch* s, bool useAltRenderer = false);
    void setStitch(QString s, bool useAltRenderer = false);
    Stitch* stitch() const { return mStitch; }

    QString name();

    QGraphicsItem::GraphicsItemFlags flags() const;
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
public slots:

private:
    double mAngle;

    QColor mColor;
    Stitch* mStitch;
};

#endif // CELL_H

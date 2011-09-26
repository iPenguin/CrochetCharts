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
    friend class SaveThread;
public:

    enum { Type = UserType + 1 };
    
    explicit Cell(QGraphicsItem* parent = 0);
    ~Cell();

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    int type () const { return Cell::Type; }

    void setColor(QColor c = QColor(Qt::white));
    QColor color() const { return mColor; }
    
    void setStitch(Stitch* s, bool useAltRenderer = false);
    void setStitch(QString s, bool useAltRenderer = false);
    Stitch* stitch() const { return mStitch; }

    /**
     * The stitch name.
     */
    QString name();

    void useAlternateRenderer(bool useAlt);
    
    QPointF scale() { return mScale; }
    void setScale(qreal sx, qreal sy);

    QPointF actualScale() {return mActualScale; }
    void setActualScale(qreal sx, qreal sy);

signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
public slots:

private:
    QColor mColor;
    Stitch* mStitch;

    QPointF mScale;
    QPointF mActualScale;
};

#endif // CELL_H

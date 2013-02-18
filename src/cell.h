/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CELL_H
#define CELL_H

#include <QtSvg/QGraphicsSvgItem>
#include "stitch.h"
#include <QPointer>
#include "item.h"

class Cell : public QGraphicsSvgItem, public Item
{
    Q_OBJECT
    friend class SaveFile;
    friend class FileLoad_v1;
    friend class FileLoad_v2;
public:

    enum { Type = UserType + 1 };
    
    explicit Cell(QGraphicsItem* parent = 0);
    ~Cell();
    
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    int type () const { return Cell::Type; }

    void setHighlight(bool state) { mHighlight = state; update(); }
    Cell* copy(Cell* cell = 0);
    
    void setBgColor(QColor c = QColor(Qt::white));
    QColor bgColor() const { return mBgColor; }
    
    void setColor(QColor c = QColor(Qt::white));
    QColor color() const { return mColor; }

    void setStitch(Stitch* s, bool useAltRenderer = false);
    void setStitch(QString uid, bool useAltRenderer = false);
    Stitch* stitch() const { return mStitch; }

    /**
     * The stitch uid.
     */
    QString uid();

    void useAlternateRenderer(bool useAlt);

    QPointF scale() {return mScale; }
    void setScale(qreal sx, qreal sy);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
private:
    QColor mBgColor;
    QColor mColor;
    QPointer<Stitch> mStitch;

    QPointF mScale;
    bool mHighlight;
};

#endif // CELL_H

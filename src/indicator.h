/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef INDICATOR_H
#define INDICATOR_H

//#include <QGraphicsItem>
#include <QGraphicsTextItem>

class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;

class Indicator : public QGraphicsTextItem //public QGraphicsItem,
{
    Q_OBJECT
    friend class SaveFile;
public:
    enum {Type = UserType + 15 };

    Indicator();
    ~Indicator();

    int type() const { return Type; }
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString text() { return mText; }
    void setText(QString t) { mText = t; }

    QColor bgColor() { return mBgColor; }
    void setBgColor(QColor c) { mBgColor = c; }

    QColor textColor() { return mTextColor; }
    void setTextColor(QColor c) { mTextColor = c; }


signals:
    void lostFocus(Indicator *item);
    void selectedChange(QGraphicsItem *item);
    
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void focusOutEvent(QFocusEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    QString mText;

    QColor mBgColor;
    QColor mTextColor;

};
#endif //INDICATOR_H

/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef INDICATOR_H
#define INDICATOR_H

#include <QGraphicsTextItem>

#include <QLineEdit>

class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QFocusEvent;

class Indicator : public QGraphicsTextItem
{

    friend class SaveFile;
public:
    enum {Type = UserType + 15 };

    Indicator(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    ~Indicator();

    QRectF boundingRect();
    int type() const { return Type; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString text() { return toPlainText(); }
    void setText(QString t) { setPlainText(t); }

    QColor bgColor() { return mBgColor; }
    void setBgColor(QColor c) { mBgColor = c; }

    QColor textColor() { return mTextColor; }
    void setTextColor(QColor c) { mTextColor = c; }

protected:
    void focusOutEvent(QFocusEvent *event);
    
private:

    QColor mBgColor;
    QColor mTextColor;

};
#endif //INDICATOR_H

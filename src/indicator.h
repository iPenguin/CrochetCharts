/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#ifndef INDICATOR_H
#define INDICATOR_H

#include <QGraphicsTextItem>

#include <QLineEdit>

class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QFocusEvent;
class QKeyEvent;

class Indicator : public QGraphicsTextItem
{
    Q_OBJECT
    friend class SaveFile;
public:
    enum {Type = UserType + 15 };

    Indicator(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    ~Indicator();

    QRectF boundingRect() const;
    int type() const { return Type; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    QString text() { return toHtml(); }
    void setText(QString t) { setHtml(t); }

    QColor bgColor() { return mBgColor; }
    void setBgColor(QColor c) { mBgColor = c; }

    QColor textColor() { return mTextColor; }
    void setTextColor(QColor c) { mTextColor = c; }

    QString style() { return mStyle; }
    void setStyle(QString style) { mStyle = style; update();}

    QPainterPath shape() const;

    bool highlight;
	
	unsigned int layer() { return mLayer; }
	void setLayer(unsigned int layer) { mLayer = layer; }

signals:
    void lostFocus(Indicator *item);
    void gotFocus(Indicator *item);

protected:
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	//the layer of the indicator
	unsigned int mLayer;
	
    QColor mBgColor;
    QColor mTextColor;	

    QString mStyle;

};
#endif //INDICATOR_H

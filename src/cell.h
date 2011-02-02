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

    void setBgColor(QColor c = QColor(Qt::white));
    QColor bgColor() const { return mBgColor; }

    void setFgColor(QColor c = QColor(Qt::black)) { mFgColor = c; }
    QColor fgColor() const { return mFgColor; }
    
    void setStitch(Stitch* s);
    void setStitch(QString s);
    Stitch* stitch() const { return mStitch; }

    QString name();
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
public slots:

private:
    qreal mRotation;
    double mAngle;

    QColor mBgColor;
    QColor mFgColor;
    Stitch* mStitch;
};

#endif // CELL_H

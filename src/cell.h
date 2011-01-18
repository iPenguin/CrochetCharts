/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CELL_H
#define CELL_H

#include <QtSvg/QGraphicsSvgItem>

class QXmlStreamWriter;

/**
 * Cell is a base class it should NOT be used directly.
 * Some of the functions are incomplete without being subclassed.
 * save() should be called like this:
 *
 * SubCell::save(stream) {
 *  stream->writeStartElement("cell");
 *  ...
 *  Cell::save(stream);
 *  ...
 *  stream->writeEndElement();
 * }
 */
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

    void save(QXmlStreamWriter *stream);
signals:

public slots:

private:
    double mRotation;
    double mAngle;
};

#endif // CELL_H

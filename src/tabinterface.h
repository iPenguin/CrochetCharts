#ifndef TABINTERFACE_H
#define TABINTERFACE_H

#include <QObject>

class QString;
class QMenu;
class QRect;
class QPoint;
class QPainter;

class TabInterface
{

public:
    TabInterface(QObject *parent);
    virtual ~TabInterface() {}

    virtual QString tabType() = 0;

    virtual bool hasZoomInterface() = 0;
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;

    virtual bool hasMenu() = 0;
    virtual QMenu menu() = 0;

    virtual QRect mousePress(const QString &brush, QPainter &painter, const QPoint &pos) = 0;
    virtual QRect mouseMove(const QString &brush, QPainter &painter, const QPoint &oldPos, const QPoint &newPos) = 0;
    virtual QRect mouseRelease(const QString &brush, QPainter &painter, const QPoint &pos) = 0;

};

#endif // TABINTERFACE_H

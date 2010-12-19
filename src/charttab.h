#ifndef CHARTTAB_H
#define CHARTTAB_H

#include <QWidget>

class QGraphicsView;
class CrochetScene;

class ChartTab : public QWidget
{
    Q_OBJECT
public:
    explicit ChartTab(QWidget *parent = 0);
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

signals:

public slots:


private:
    QGraphicsView *mView;
    CrochetScene *mScene;
};

#endif // CHARTTAB_H

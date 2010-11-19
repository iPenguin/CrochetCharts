#ifndef CHARTTAB_H
#define CHARTTAB_H

#include <QWidget>

class QGraphicsView;
class QGraphicsScene;

class ChartTab : public QWidget
{
    Q_OBJECT
public:
    explicit ChartTab(QWidget *parent = 0);

signals:

public slots:


private:
    QGraphicsView *mView;
    QGraphicsScene *mScene;
};

#endif // CHARTTAB_H

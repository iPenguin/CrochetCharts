#include "charttab.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>

ChartTab::ChartTab(QWidget *parent) :
    QWidget(parent)
{

    QVBoxLayout *l = new QVBoxLayout(this);
    mView = new QGraphicsView(this);
    mScene = new QGraphicsScene(this);
    mView->setScene(mScene);
    l->addWidget(mView);

}



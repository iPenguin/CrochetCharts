#include "crochetscene.h"

#include "crochetcell.h"

#include <QFontMetrics>
#include <QGraphicsSimpleTextItem>

#include <math.h>

#include "settings.h"

CrochetScene::CrochetScene(QObject *parent)
    : QGraphicsScene(parent)
{
    mStitchWidth = 64;
    this->initDemoBackground();
}

void CrochetScene::initDemoBackground()
{
    if(Settings::inst()->isDemoVersion()) {

        double fontSize = 32.0;
        QFont demoFont = QFont();
        demoFont.setPointSize(fontSize);
        QString demoString = "Stitch Works Software - Demo Version  -  ";

        QFontMetrics fm = QFontMetrics(demoFont);
        double stringWidth = fm.width(demoString);

        QGraphicsSimpleTextItem *demoText;
        QRectF rect = this->sceneRect();
        double demoRows = rect.height() / fontSize;
        demoRows = demoRows /2.0;

        double demoCols = rect.width() / stringWidth;

        for(int c = 0; c < ceil(demoCols); ++c) {
            for(int i = 0; i < ceil(demoRows); ++i) {
                demoText = this->addSimpleText(demoString, demoFont);
                demoText->setBrush(QBrush(QColor(Qt::lightGray)));
                QPointF point = QPointF(rect.left() + c*stringWidth , rect.top() + i*(2*fontSize));
                demoText->setPos(point);
                demoText->setZValue(-1);
            }
        }

        //restore original rect. letting the demo text overflow off the scene.
        this->setSceneRect(rect);
    }
}

void CrochetScene::createRow(int columns)
{
    //FIXME: less then 8 stitches gives funny rows.
    double widthInDegrees = 360.0 / columns;
    double circumference = (columns -2) * mStitchWidth;
    double diameter = circumference / M_PI;
    double radius = diameter /2;

    Cell *c;

    for(int i = 0; i < columns; ++i) {
        double degrees = (widthInDegrees*i) - (widthInDegrees/2);
        QPointF finish = this->calcPoint(radius, degrees, QPointF(0,0));

        c = new CrochetCell(":/stitches/chain.svg");
        this->addItem(c);
        c->setPos(finish);
        c->setToolTip(QString::number(i+1));
        c->rotate(degrees + (widthInDegrees/2));
    }
}

QPointF CrochetScene::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    double x = (double)(radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    double y = (double)(radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

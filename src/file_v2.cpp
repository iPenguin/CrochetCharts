#include "file_v2.h"

#include "debug.h"

#include "appinfo.h"

#include <QFileInfo>
#include <QDir>
#include <QFile>

#include "stitchlibrary.h"
#include "mainwindow.h"
#include "scene.h"

#include "crochettab.h"

FileLoad_v2::FileLoad_v2(FileFactory *parent)
    : File(parent)
{

}

FileFactory::FileError FileLoad_v2::load(QXmlStreamReader *stream)
{

    while (!stream->atEnd() && !stream->hasError()) {

        stream->readNext();
        if (stream->isStartElement()) {
            QString name = stream->name().toString();

            if(name == "colors") {
                loadColors(stream);

            } else if(name == "chart") {
                loadChart(stream);

            } else if(name == "stitch_set") {
                mParent->mInternalStitchSet->loadXmlStitchSet(stream, true);

            }
        }
    }

    StitchLibrary::inst()->addStitchSet(mParent->mInternalStitchSet);

    return FileFactory::No_Error;
}

void FileLoad_v2::loadColors(QXmlStreamReader* stream)
{

    MainWindow* mw = qobject_cast<MainWindow*>(mParent->mParent);

    mw->patternColors().clear();

    while(!(stream->isEndElement() && stream->name() == "colors")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if (tag == "color") {
            QMap<QString, qint64> properties;
            properties.insert("count", 0); //count = 0 because we haven't added any cells yet.
            properties.insert("added", (qint64) stream->attributes().value("added").toString().toLongLong());
            mw->mPatternColors.insert(stream->readElementText(),properties);
        }
    }

}

void FileLoad_v2::loadChart(QXmlStreamReader* stream)
{
    MainWindow* mw = qobject_cast<MainWindow*>(mParent->mParent);
    CrochetTab* tab = 0;
    QString tabName = "", defaultSt = "";

    while(!(stream->isEndElement() && stream->name() == "chart")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "name") {
            tabName = stream->readElementText();

        } else if(tag == "style") {
            int style = stream->readElementText().toInt();
            tab = mw->createTab((Scene::ChartStyle)style);

            mParent->mTabWidget->addTab(tab, "");
            mParent->mTabWidget->widget(mParent->mTabWidget->indexOf(tab))->hide();
        } else if(tag == "defaultSt") {
            defaultSt = stream->readElementText();
            tab->scene()->mDefaultStitch = defaultSt;

        } else if(tag == "chartCenter") {
            qreal x = stream->attributes().value("x").toString().toDouble();
            qreal y = stream->attributes().value("y").toString().toDouble();

            stream->readElementText();
            tab->blockSignals(true);
            tab->setShowChartCenter(true);
            tab->scene()->mCenterSymbol->setPos(x, y);
            tab->blockSignals(false);

        } else if(tag == "grid") {
            loadGrid(stream, tab->scene());

        } else if(tag == "rowSpacing") {
            qreal width = stream->attributes().value("width").toString().toDouble();
            qreal height = stream->attributes().value("height").toString().toDouble();
            tab->scene()->mDefaultSize.setHeight(height);
            tab->scene()->mDefaultSize.setWidth(width);

        } else if(tag == "cell") {
            loadCell(tab, stream);

        } else if(tag == "indicator") {
            loadIndicator(tab, stream);

        } else if(tag == "group") {
            stream->readElementText().toInt();
            //create an empty group for future use.
            QList<QGraphicsItem*> items;
            tab->scene()->group(items);
        }
    }

    tab->scene()->initDemoBackground();
    tab->updateRows();
    int index = mParent->mTabWidget->indexOf(tab);
    mParent->mTabWidget->setTabText(index, tabName);
    mParent->mTabWidget->widget(mParent->mTabWidget->indexOf(tab))->show();
    tab->scene()->updateSceneRect();

    if(tab->scene()->hasChartCenter()) {
        tab->view()->centerOn(tab->scene()->mCenterSymbol->sceneBoundingRect().center());
    } else {
        tab->view()->centerOn(tab->scene()->itemsBoundingRect().center());
    }
}

void FileLoad_v2::loadGrid(QXmlStreamReader* stream, Scene* scene)
{
    while(!(stream->isEndElement() && stream->name() == "grid")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "row") {
            int cols = stream->readElementText().toInt();
            QList<Cell*> row;
            for(int i = 0; i < cols; ++i) {
                row.append(0);
            }
            scene->grid.append(row);
        }
    }
}

void FileLoad_v2::loadIndicator(CrochetTab* tab, QXmlStreamReader* stream)
{
    Indicator* i = new Indicator();

    qreal x = 0, y = 0;
    QString textColor, bgColor;
    QString text;

    while(!(stream->isEndElement() && stream->name() == "indicator")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "x") {
            x = stream->readElementText().toDouble();
        } else if(tag == "y") {
            y = stream->readElementText().toDouble();
        } else if(tag == "text") {
            text = stream->readElementText();
        } else if(tag == "textColor") {
            textColor = stream->readElementText();
        } else if(tag == "bgColor") {
            bgColor = stream->readElementText();
        }
    }

    tab->scene()->addItem(i);
    i->setPos(x,y);
    i->setText(text);
    i->setTextColor(textColor);
    i->setBgColor(bgColor);
}

void FileLoad_v2::loadCell(CrochetTab* tab, QXmlStreamReader* stream)
{

    Cell* c = new Cell();
    Stitch* s = 0;
    int row = -1, column = -1;
    int group = -1;
    QString bgColor, color;
    QPointF position(0.0,0.0);
    QPointF pivotPoint;
    qreal angle = 0.0;
    QPointF scale = QPointF(1.0,1.0);
    QTransform transform;
    qreal   m11 = 1, m12 = 0, m13 = 0,
            m21 = 0, m22 = 1, m23 = 0,
            m31 = 0, m32 = 0, m33 = 1;

    while(!(stream->isEndElement() && stream->name() == "cell")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "stitch") {
            QString st = stream->readElementText();
            s = StitchLibrary::inst()->findStitch(st);

        } else if(tag == "grid") {
            row = stream->attributes().value("row").toString().toDouble();
            column = stream->attributes().value("column").toString().toDouble();
            stream->readElementText();

        } else if(tag == "color") {
            color = stream->readElementText();

        } else if(tag == "bgColor") {
            bgColor = stream->readElementText();

        } else if(tag == "position") {
            position.rx() = stream->attributes().value("x").toString().toDouble();
            position.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "angle") {
            angle = stream->readElementText().toDouble();

        } else if(tag == "scale") {
            scale.rx() = stream->attributes().value("x").toString().toDouble();
            scale.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "pivotPoint") {
            pivotPoint.rx() = stream->attributes().value("x").toString().toDouble();
            pivotPoint.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "group") {
            group = stream->readElementText().toInt();

        } else if(tag == "transformation") {
            m11 = stream->attributes().value("m11").toString().toDouble();
            m12 = stream->attributes().value("m12").toString().toDouble();
            m13 = stream->attributes().value("m13").toString().toDouble();
            m21 = stream->attributes().value("m21").toString().toDouble();
            m22 = stream->attributes().value("m22").toString().toDouble();
            m23 = stream->attributes().value("m23").toString().toDouble();
            m31 = stream->attributes().value("m31").toString().toDouble();
            m32 = stream->attributes().value("m32").toString().toDouble();
            m33 = stream->attributes().value("m33").toString().toDouble();
            transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
            stream->readElementText();
        }
    }

    tab->scene()->addItem(c);

    if(row > -1 && column > -1) {
        c->setStitch(s, (row % 2));
        tab->scene()->grid[row].replace(column, c);
        c->setZValue(100);
    } else {
        c->setStitch(s);
        c->setZValue(10);
    }

    c->setTransform(transform);
    c->setPos(position);
    c->setBgColor(QColor(bgColor));
    c->setColor(QColor(color));
    c->setTransformOriginPoint(pivotPoint);
    c->setRotation(angle);

    if(group != -1)
        tab->scene()->mGroups[group]->addToGroup(c);
}

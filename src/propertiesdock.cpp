#include "propertiesdock.h"
#include "ui_propertiesdock.h"

#include "settings.h"
#include "cell.h"
#include "crochettab.h"
#include "stitchlibrary.h"
#include "stitch.h"
#include "colorlistwidget.h"
#include <qcolordialog.h>

PropertiesDock::PropertiesDock(QTabWidget *tabWidget, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PropertiesDock),
    mTabWidget(tabWidget),
    mScene(0)
{
    ui->setupUi(this);
    setVisible(false);
    setFloating(true);
    setObjectName("propertiesDock");

    ui->chartGroup->hide();
    ui->itemGroup->hide();
qDebug() << "propertiesdock";
    connect(mTabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));
}

PropertiesDock::~PropertiesDock()
{
    delete ui;
}

void PropertiesDock::selectionChanged()
{
    CrochetTab *tab = qobject_cast<CrochetTab*>(mTabWidget->currentWidget());
    if(!tab) {
        qWarning() << "Couldn't find a tab!";
    }
    QList<QGraphicsItem*> items = tab->scene()->selectedItems();

    if(items.count() <= 0)
        return;
qDebug() << "selection changed";
    bool firstPass = true;

    foreach(QGraphicsItem *i, items) {
        Cell *cur = qgraphicsitem_cast<Cell*>(i);
        if(!cur)
            continue;

        if(firstPass) {
            mCellProps.stitch = cur->name();
            mCellProps.color = cur->color();
            mCellProps.bgColor = cur->bgColor();
            mCellProps.scale = cur->scale();
            mCellProps.angle = cur->rotation();
            firstPass = false;
        }

        mCellProps.stitch = stitchValue(mCellProps.stitch, cur->name());
        mCellProps.color = colorValue(mCellProps.color, cur->color());
        mCellProps.bgColor = colorValue(mCellProps.bgColor, cur->bgColor());
        mCellProps.scale = scaleValue(mCellProps.scale, cur->scale());
        mCellProps.angle = angleValue(mCellProps.angle, cur->rotation());
    }
qDebug() << "selection changed end";
}

PropertiesDock::UiSelection PropertiesDock::chooseUi()
{
    UiSelection uiStyle = PropertiesDock::SceneUi;

    int count = mScene->selectedItems().count();

    if(count == 0) {

        uiStyle = PropertiesDock::SceneUi;
    } else if(count >= 1) {
        bool theSame = true;

        int firstType = mScene->selectedItems().first()->type();
        for(int i = 1; i < count; ++i) {
            if(firstType != mScene->selectedItems().at(i)->type()) {
                theSame = false;
                break;
            }
        }

        if(!theSame) {
            uiStyle = PropertiesDock::MixedUi;
        } else if(firstType == Cell::Type) {
            uiStyle = PropertiesDock::CellUi;
        } else if(firstType == Indicator::Type) {
            uiStyle = PropertiesDock::IndicatorUi;
        }
    }

    return uiStyle;
}

void PropertiesDock::updatePropertiesUi()
{
    qDebug() << "update properties ui";

    UiSelection selection = chooseUi();

    //Choose the options to show based on the selection.
    foreach(QObject *obj, ui->itemGroup->children()) {
        if(obj->objectName().startsWith("gen_")) {
            QWidget *w = qobject_cast<QWidget*>(obj);
            w->setVisible(true);

        } else if (obj->objectName().startsWith("st_")) {
            QWidget *w = qobject_cast< QWidget* >(obj);
            w->setVisible(selection == PropertiesDock::CellUi ? true : false);

        } else if(obj->objectName().startsWith("ind_")) {
            QWidget *w = qobject_cast< QWidget* >(obj);
            w->setVisible(selection == PropertiesDock::IndicatorUi ? true : false);
        }
    }

    qDebug() << selection;
    if(selection == PropertiesDock::SceneUi)
        ui->chartGroup->show();

}

void PropertiesDock::tabChanged(int tabNumber)
{
    if(tabNumber <= -1) {
        return;
    }

    mScene = qobject_cast<CrochetTab*>(mTabWidget->widget(tabNumber))->scene();
    connect(mScene, SIGNAL(selectionChanged()), SLOT(selectionChanged()));
}

void PropertiesDock::cellUpdateFgColor()
{

    QColorDialog cdlg;
    QColor color = cdlg.getColor();
    QVariant v = color;

    emit propertyUpdated("fgColor", v);
}

void PropertiesDock::cellUpdateBgColor()
{
    QColorDialog cdlg;
    QColor color = cdlg.getColor();
    QVariant v = color;

    emit propertyUpdated("bgColor", v);
}

QString PropertiesDock::stitchValue(QString first, QString second)
{
    QString stitch;
    if(first == second)
        stitch = first;
    else
        stitch = "";

    return stitch;
}

QColor PropertiesDock::colorValue(QColor first, QColor second)
{
    QColor color;
    if(first == second)
        color = first;
    else
        color = QColor("");

    return color;
}

QPointF PropertiesDock::scaleValue(QPointF first, QPointF second)
{
    qreal x = 1.0,
          y = 1.0;

    if(first.x() == second.x())
        x = first.x();
    else
        x = 0.0;

    if(first.y() == second.y())
        y = first.y();
    else
        y = 0.0;

    return QPointF(x,y);
}

qreal PropertiesDock::angleValue(qreal first, qreal second)
{
    qreal angle = 0.0;
    if(first == second)
        angle = first;
    else
        angle = -1;

    return angle;
}

void PropertiesDock::setupStitchCombo()
{

    ui->st_stitch->blockSignals(true);
    //populate the combo box.

    foreach(QString stitch, StitchLibrary::inst()->stitchList()) {
        Stitch *s = StitchLibrary::inst()->findStitch(stitch);
        ui->st_stitch->addItem(QIcon(s->file()), stitch);
    }

    //Smart selection of stitch for combo box.
    //if the stitches aren't the same use "" else use the name of the stitch.
    QString st = "";
    Cell *prev = 0;
    foreach(QGraphicsItem* i, mScene->selectedItems()) {
        Cell *c = qgraphicsitem_cast<Cell*>(i);
        if(prev) {
            if(c->name() != prev->name()) {
                st = "";
                break;
            }
        } else {
            st = c->name();
        }

        prev = c;
    }

    ui->st_stitch->setCurrentIndex(ui->st_stitch->findText(st));

    ui->st_stitch->blockSignals(false);

}
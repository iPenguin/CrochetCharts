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
#include "propertiesdock.h"
#include "ui_propertiesdock.h"

#include "settings.h"

#include "cell.h"
#include "itemgroup.h"
#include "indicator.h"
#include <QGraphicsEllipseItem>

#include "crochettab.h"
#include "stitchlibrary.h"
#include "stitch.h"
#include "colorlistwidget.h"
#include <qcolordialog.h>

#include "propertiesdata.h"

PropertiesDock::PropertiesDock(QTabWidget *tabWidget, QWidget *parent) :
    QDockWidget(parent),
    closing(false),
    ui(new Ui::PropertiesDock),
    mTabWidget(tabWidget),
    mScene(0)
{
    ui->setupUi(this);
    setVisible(false);
    setFloating(true);
    setObjectName("propertiesDock");

    ui->rows->setValue(Settings::inst()->value("rowCount").toInt());
    ui->columns->setValue(Settings::inst()->value("stitchCount").toInt());
    ui->cellHeight->setValue(Settings::inst()->value("cellHeight").toInt());
    ui->cellWidth->setValue(Settings::inst()->value("cellWidth").toInt());

    int styleIdx = ui->ind_indicatorStyle->findText(Settings::inst()->value("chartRowIndicator").toString());
    ui->ind_indicatorStyle->setCurrentIndex(styleIdx);
    clearUi();
    connect(mTabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));

    connect(ui->gen_angle, SIGNAL(valueChanged(double)), SLOT(cellUpdateAngle(double)));
    connect(ui->gen_scaleX, SIGNAL(valueChanged(double)), SLOT(cellUpdateScaleX(double)));
    connect(ui->gen_scaleY, SIGNAL(valueChanged(double)), SLOT(cellUpdateScaleY(double)));

    connect(ui->gen_xPos, SIGNAL(valueChanged(double)), SLOT(cellUpdatePositionX(double)));
    connect(ui->gen_yPos, SIGNAL(valueChanged(double)), SLOT(cellUpdatePositionY(double)));

	connect(ui->snapAngle, SIGNAL(toggled(bool)), SLOT(chartUpdateAlignAngle(bool)));
    connect(ui->showChartCenter, SIGNAL(toggled(bool)), SLOT(chartUpdateChartCenter(bool)));
    connect(ui->guidelinesType, SIGNAL(currentIndexChanged(QString)), SLOT(chartUpdateGuidelines()));

    connect(ui->st_stitch, SIGNAL(currentIndexChanged(QString)), SLOT(cellUpdateStitch(QString)));

    connect(ui->gen_deleteItems, SIGNAL(clicked()), SLOT(cellDeleteItems()));

    connect(ui->guidelinesType, SIGNAL(currentIndexChanged(int)), SLOT(updateGuidelinesUi()));
    updateGuidelinesUi();

    connect(ui->rows, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));
    connect(ui->cellWidth, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));
    connect(ui->columns, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));
    connect(ui->cellHeight, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));

    connect(ui->st_stBgColorBttn, SIGNAL(clicked(bool)), SLOT(cellUpdateBgColor()));
    connect(ui->st_stColorBttn, SIGNAL(clicked(bool)), SLOT(cellUpdateFgColor()));

    connect(ui->ind_indicatorStyle, SIGNAL(currentIndexChanged(int)), SLOT(indicatorUpdate()));
	connect(ui->ind_fontComboBox, SIGNAL(currentFontChanged(const QFont&)), SLOT(indicatorUpdate()));
	connect(ui->ind_size, SIGNAL(valueChanged(int)), SLOT(indicatorUpdate()));

    setupStitchCombo();

    ui->indX_IndicatorEditText->setVisible(false);
    ui->indX_indicatorTextEdit->setVisible(false);
}

PropertiesDock::~PropertiesDock()
{
    delete ui;
}

void PropertiesDock::loadProperties(Guidelines guidelines)
{
    mGuidelines = guidelines;

    //QUESTION: is this the best way to prevent overwriting the properties when "loading" them.
    ui->guidelinesType->blockSignals(true);
    ui->guidelinesType->setCurrentIndex(ui->guidelinesType->findText(mGuidelines.type()));
    ui->guidelinesType->blockSignals(false);

    ui->rows->blockSignals(true);
    ui->rows->setValue(mGuidelines.rows());
    ui->rows->blockSignals(false);

    ui->columns->blockSignals(true);
    ui->columns->setValue(mGuidelines.columns());
    ui->columns->blockSignals(false);

    ui->cellHeight->blockSignals(true);
    ui->cellHeight->setValue(mGuidelines.cellHeight());
    ui->cellHeight->blockSignals(false);

    ui->cellWidth->blockSignals(true);
    ui->cellWidth->setValue(mGuidelines.cellWidth());
    ui->cellWidth->blockSignals(false);

}

void PropertiesDock::tabChanged(int tabNumber)
{

    if(tabNumber == -1) {
        clearUi();
        return;
    }

    mScene = qobject_cast<CrochetTab*>(mTabWidget->widget(tabNumber))->scene();
    connect(mScene, SIGNAL(selectionChanged()), SLOT(updateDialogUi()));
    updateDialogUi();

}

void PropertiesDock::clearUi()
{
    ui->chartGroup->hide();
    ui->itemGroup->hide();
}

void PropertiesDock::setupStitchCombo()
{

    //populate the combo box.
    foreach(QString stitch, StitchLibrary::inst()->stitchList()) {
        Stitch *s = StitchLibrary::inst()->findStitch(stitch);
        ui->st_stitch->addItem(QIcon(s->file()), stitch);
    }

}

bool PropertiesDock::updateGuidelines()
{
    bool changed = false;

    if(mGuidelines.type() != ui->guidelinesType->currentText()) {
        mGuidelines.setType(ui->guidelinesType->currentText());
        changed = true;
    }

    if(mGuidelines.rows() != ui->rows->value()) {
        mGuidelines.setRows(ui->rows->value());
        changed = true;
    }

    if(mGuidelines.columns() != ui->columns->value()) {
        mGuidelines.setColumns(ui->columns->value());
        changed = true;
    }

    if(mGuidelines.cellHeight() != ui->cellHeight->value()) {
        mGuidelines.setCellHeight(ui->cellHeight->value());
        changed = true;
    }

    if(mGuidelines.cellWidth() != ui->cellWidth->value()) {
        mGuidelines.setCellWidth(ui->cellWidth->value());
        changed = true;
    }

    return changed;
}

PropertiesData PropertiesDock::selectionProperties()
{
    PropertiesData props;
    bool firstPass = true;

    bool angleMixed = false, xScaleMixed = false, yScaleMixed = false,
         xPositionMixed = false, yPositionMixed = false, stitchMixed = false,
         colorMixed = false, bgColorMixed = false;
         
    foreach(QGraphicsItem *i, mScene->selectedItems()) {
        
        Cell *c = 0;
        ItemGroup *g = 0;
        Indicator *ind = 0;
        QGraphicsEllipseItem *cc = 0; //ChartCenter
        
        if(i->type() == Cell::Type) {
            c = qgraphicsitem_cast<Cell*>(i);
        } else if(i->type() == ItemGroup::Type) {
            g = qgraphicsitem_cast<ItemGroup*>(i);
        } else if(i->type() == Indicator::Type) {
            ind = qgraphicsitem_cast<Indicator*>(i);
        } else if(i->type() == QGraphicsEllipseItem::Type) {
            cc =  qgraphicsitem_cast<QGraphicsEllipseItem*>(i);
        }
        
        if(firstPass) {
            
            props.angle = c->rotation();
            props.scale.setX(c->transform().m11());
            props.scale.setY(c->transform().m22());
            props.position.setX(c->pos().x());
            props.position.setY(c->pos().y());
            props.stitch = c->name();
            props.color = c->color();
            props.bgColor = c->bgColor();
            firstPass = false;
        }

        if(!c)
            continue;
        
        if(props.angle != c->rotation())
            angleMixed = true;
        if(props.scale.x() != c->transform().m11())
            xScaleMixed = true;
        if(props.scale.y() != c->transform().m22())
            yScaleMixed = true;
        if(props.position.x() != c->pos().x())
            xPositionMixed = true;
        if(props.position.y() != c->pos().y())
            yPositionMixed = true;
        if(props.stitch != c->name())
            stitchMixed = true;
        if(props.color != c->color())
            colorMixed = true;
        if(props.bgColor != c->bgColor())
            bgColorMixed = true;

    }

    if(angleMixed)
        props.angle = 0.0;
    if(xScaleMixed)
        props.scale.setX(1.0);
    if(yScaleMixed)
        props.scale.setY(1.0);
    if(xPositionMixed)
        props.position.setX(0.0);
    if(yPositionMixed)
        props.position.setY(0.0);
    if(stitchMixed)
        props.stitch = "";
    if(colorMixed)
        props.color = QColor();
    if(bgColorMixed)
        props.bgColor = QColor();

    return props;
}

void PropertiesDock::updateDialogUi()
{

    clearUi();
    if(closing)
        return;

    int count = mScene->selectedItems().count();

    if(count == 0) {

        showUi(PropertiesDock::SceneUi);
        return;
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
            showUi(PropertiesDock::MixedUi, count);
        } else if(firstType == Cell::Type) {
            showUi(PropertiesDock::CellUi, count);
        } else if(firstType == Indicator::Type) {
            showUi(PropertiesDock::IndicatorUi, count);
        } else if(firstType == QGraphicsEllipseItem::Type) {
			showUi(PropertiesDock::CenterUi, count);
		}
    }
}

void PropertiesDock::showUi(PropertiesDock::UiSelection selection, int count)
{

    Q_UNUSED(count);

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

    if(selection == PropertiesDock::SceneUi || selection == PropertiesDock::CenterUi) {
        showCanvas();

    } else if(selection == PropertiesDock::CellUi) {
        showSingleCell();
        
    } else if(selection == PropertiesDock::ItemGroupUi){
        showItemGroup();
    } else if(selection == PropertiesDock::MixedUi) {
        showMixedObjects();

        //TODO: loop through all the items, check all the

    } else if(selection == PropertiesDock::IndicatorUi) {
        showSingleIndicator();

    }/* else if (selection == PropertiesDock::CenterUi) {
        WARN("TODO: make center ui work");
    }*/

}

void PropertiesDock::showSingleCell()
{

    PropertiesData p = selectionProperties();

    ui->itemGroup->show();

    ui->gen_angle->blockSignals(true);
    ui->gen_angle->setValue(p.angle);
    ui->gen_angle->blockSignals(false);

    ui->gen_xPos->blockSignals(true);
    ui->gen_xPos->setValue(p.position.x());
    ui->gen_xPos->blockSignals(false);

    ui->gen_yPos->blockSignals(true);
    ui->gen_yPos->setValue(p.position.y());
    ui->gen_yPos->blockSignals(false);

    ui->gen_scaleX->blockSignals(true);
    ui->gen_scaleX->setValue(p.scale.x());
    ui->gen_scaleX->blockSignals(false);

    ui->gen_scaleY->blockSignals(true);
    ui->gen_scaleY->setValue(p.scale.y());
    ui->gen_scaleY->blockSignals(false);

    ui->st_stColorBttn->setIcon(ColorListWidget::drawColorBox(p.color, QSize(32,32)));
    ui->st_stColorBttn->setText(p.color.name());
    ui->st_stBgColorBttn->setIcon(ColorListWidget::drawColorBox(p.bgColor, QSize(32,32)));
    ui->st_stBgColorBttn->setText(p.bgColor.name());

    ui->st_stitch->blockSignals(true);
    ui->st_stitch->setCurrentIndex(ui->st_stitch->findText(p.stitch));
    ui->st_stitch->blockSignals(false);

}

void PropertiesDock::showMultiCell()
{
    //qDebug() << "multi cell";
}

void PropertiesDock::showSingleIndicator()
{
    ui->itemGroup->show();

    Indicator *i = qgraphicsitem_cast<Indicator*>(mScene->selectedItems().first());
    //ui->ind_indicatorTextEdit->setText(i->text());
    ui->ind_indicatorStyle->blockSignals(true);
    ui->ind_indicatorStyle->setCurrentIndex(ui->ind_indicatorStyle->findText(i->style()));
    ui->ind_indicatorStyle->blockSignals(false);
	ui->ind_fontComboBox->blockSignals(true);
	ui->ind_fontComboBox->setCurrentFont(i->font());
	ui->ind_fontComboBox->blockSignals(false);
	ui->ind_size->blockSignals(true);
	ui->ind_size->setValue(i->font().pointSize());
	ui->ind_size->blockSignals(false);
}

void PropertiesDock::showMultiIndicator()
{

}

void PropertiesDock::showMixedObjects()
{
    //qDebug() << "mixed objs";
    ui->itemGroup->show();
}

void PropertiesDock::showItemGroup()
{
    ui->itemGroup->show();
    
}

void PropertiesDock::showCanvas()
{
    ui->chartGroup->show();

    ui->showChartCenter->setChecked(mScene->showChartCenter());

    QString type = mScene->guidelines().type();

    if(type.isEmpty())
        type = "None";

    ui->guidelinesType->setCurrentIndex(ui->guidelinesType->findText(type));
    updateGuidelinesUi();
}

void PropertiesDock::updateGuidelinesUi()
{
    if(ui->guidelinesType->currentText() == "None") {
        ui->rows->setEnabled(false);
        ui->columns->setEnabled(false);
        ui->cellWidth->setEnabled(false);
        ui->cellHeight->setEnabled(false);
		ui->snapAngle->setEnabled(false);
		ui->snapAngleLbl->setEnabled(false);
    } else {
        ui->rows->setEnabled(true);
        ui->columns->setEnabled(true);
        ui->cellWidth->setEnabled(true);
        ui->cellHeight->setEnabled(true);

        if( ui->guidelinesType->currentText() == "Rows") {
            ui->rowsLbl->setText(tr("Rows:"));
            ui->cellWidthLbl->show();
            ui->cellWidth->show();
			ui->snapAngle->setEnabled(false);
			ui->snapAngleLbl->setEnabled(false);
        } else { //rounds
            ui->rowsLbl->setText(tr("Rounds:"));
            ui->cellWidthLbl->hide();
            ui->cellWidth->hide();
			ui->snapAngle->setEnabled(true);
			ui->snapAngleLbl->setEnabled(true);
        }
    }

}

void PropertiesDock::indicatorUpdate()
{	
	const QFont& font = ui->ind_fontComboBox->currentFont();
    QString style = ui->ind_indicatorStyle->currentText();
	int size = ui->ind_size->value();
	
    IndicatorProperties ip;
    ip.setFont(font);
	ip.setSize(size);
    ip.setStyle(style);

    QVariant value;
    value.setValue(ip);

    emit propertiesUpdated("Indicator", value);
}

/**
 *Chart functions:
 */

void PropertiesDock::chartUpdateAlignAngle(bool state)
{
	emit propertiesUpdated("AlignAngle", QVariant(state));
}

void PropertiesDock::chartUpdateChartCenter(bool state)
{
    emit propertiesUpdated("ChartCenter", QVariant(state));
}

void PropertiesDock::chartUpdateGuidelines()
{

    if (updateGuidelines()) {
        QVariant value;
        value.setValue(mGuidelines);
        emit propertiesUpdated("Guidelines", value);
    }
}

void PropertiesDock::propertyUpdated()
{
    updateDialogUi();
}

/**
 *Cell functions:
 */

void PropertiesDock::cellUpdateAngle(double angle)
{
    emit propertiesUpdated("Angle", QVariant(angle));
}

void PropertiesDock::cellUpdatePositionX(double positionX)
{
    emit propertiesUpdated("PositionX", QVariant(positionX));
}

void PropertiesDock::cellUpdatePositionY(double positionY)
{
    emit propertiesUpdated("PositionY", QVariant(positionY));
}

void PropertiesDock::cellUpdateScaleX(double scale)
{
    emit propertiesUpdated("ScaleX", QVariant(scale));
}

void PropertiesDock::cellUpdateScaleY(double scale)
{
    emit propertiesUpdated("ScaleY", QVariant(scale));
}

void PropertiesDock::cellUpdateStitch(QString stitch)
{
    emit propertiesUpdated("Stitch", QVariant(stitch));
}

void PropertiesDock::cellDeleteItems()
{
    emit propertiesUpdated("Delete", QVariant(true));
}

void PropertiesDock::cellUpdateFgColor()
{

    QColorDialog cdlg;
    QColor color = cdlg.getColor();
    QVariant v = color;
    emit propertiesUpdated("fgColor", v);
}

void PropertiesDock::cellUpdateBgColor()
{
    QColorDialog cdlg;
    QColor color = cdlg.getColor();
    QVariant v = color;
    emit propertiesUpdated("bgColor", v);
}

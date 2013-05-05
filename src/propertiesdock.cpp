#include "propertiesdock.h"
#include "ui_propertiesdock.h"

#include "settings.h"
#include "cell.h"
#include "crochettab.h"
#include "stitchlibrary.h"
#include "stitch.h"

PropertiesDock::PropertiesDock(QTabWidget* tabWidget, QWidget *parent) :
    QDockWidget(parent),
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

    int styleIdx = ui->indicatorStyle->findText(Settings::inst()->value("chartRowIndicator").toString());
    ui->indicatorStyle->setCurrentIndex(styleIdx);
    clearUi();
    connect(mTabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));

    connect(ui->angle, SIGNAL(valueChanged(double)), SLOT(cellUpdateAngle(double)));
    connect(ui->scaleX, SIGNAL(valueChanged(double)), SLOT(cellUpdateScaleX(double)));
    connect(ui->scaleY, SIGNAL(valueChanged(double)), SLOT(cellUpdateScaleY(double)));

    connect(ui->showChartCenter, SIGNAL(toggled(bool)), SLOT(chartUpdateChartCenter(bool)));
    connect(ui->guidelinesType, SIGNAL(currentIndexChanged(QString)), SLOT(chartUpdateGuidelines()));

    connect(ui->stitch, SIGNAL(currentIndexChanged(QString)), SLOT(cellUpdateStitch(QString)));

    connect(ui->deleteItems, SIGNAL(clicked()), SLOT(cellDeleteItems()));

    connect(ui->guidelinesType, SIGNAL(currentIndexChanged(int)), SLOT(updateGuidelinesUi()));
    updateGuidelinesUi();

    connect(ui->rows, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));
    connect(ui->cellWidth, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));
    connect(ui->columns, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));
    connect(ui->cellHeight, SIGNAL(valueChanged(int)), SLOT(chartUpdateGuidelines()));

    connect(ui->indicatorStyle, SIGNAL(currentIndexChanged(int)), SLOT(indicatorUpdate()));
}

PropertiesDock::~PropertiesDock()
{
    delete ui;
}

void PropertiesDock::loadProperties(Guidelines guidelines)
{
    mGuidelines = guidelines;

    //QUESTION: is this the best way to prevent overwritting the properties when "loading" them.
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
    ui->stitchGroup->hide();
    ui->indicatorGroup->hide();
}

void PropertiesDock::setupStitchCombo()
{

    ui->stitch->blockSignals(true);
    //populate the combo box.
    foreach(QString stitch, StitchLibrary::inst()->stitchList()) {
        Stitch *s = StitchLibrary::inst()->findStitch(stitch);
        ui->stitch->addItem(QIcon(s->file()), stitch);
    }

    //Smart selection of stitch for combobox.
    //if the sittches aren't the same use "" else use the name of the stitch.
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

    ui->stitch->setCurrentIndex(ui->stitch->findText(st));

    ui->stitch->blockSignals(false);

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

void PropertiesDock::updateDialogUi()
{

    clearUi();

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
            showUi(PropertiesDock::MixedUi);
        } else if(firstType == Cell::Type) {
            showUi(PropertiesDock::CellUi);
        } else if(firstType == Indicator::Type) {
            showUi(PropertiesDock::IndicatorUi);
        }

    }

}

void PropertiesDock::showUi(PropertiesDock::UiSelection selection)
{

    if(selection == PropertiesDock::SceneUi) {
        ui->chartGroup->show();

        ui->showChartCenter->setChecked(mScene->showChartCenter());

        QString guidelines = mScene->guidelines().type();
        if(guidelines.isEmpty())
            guidelines = "None";

        ui->guidelinesType->setCurrentIndex(ui->guidelinesType->findText(guidelines));
        updateGuidelinesUi();

    } else if(selection == PropertiesDock::CellUi) {

        Cell *c = qgraphicsitem_cast<Cell*>(mScene->selectedItems().first());
        ui->stitchGroup->show();

        ui->angle->blockSignals(true);
        ui->angle->setValue(c->rotation());
        ui->angle->blockSignals(false);

        ui->scaleX->blockSignals(true);
        ui->scaleX->setValue(c->scale().x());
        ui->scaleX->blockSignals(false);

        ui->scaleY->blockSignals(true);
        ui->scaleY->setValue(c->scale().y());
        ui->scaleY->blockSignals(false);

        setupStitchCombo();

    } else if(selection == PropertiesDock::MixedUi) {

        //TODO: loop through all the items, check all the


    } else if(selection == PropertiesDock::IndicatorUi) {
        ui->indicatorGroup->show();
        Indicator *i = qgraphicsitem_cast<Indicator*>(mScene->selectedItems().first());

        ui->indicatorStyle->setCurrentIndex(ui->indicatorStyle->findText(i->style()));

    } else if (selection == PropertiesDock::CenterUi) {
        WARN("TODO: make center ui work");
    }

}

void PropertiesDock::updateGuidelinesUi()
{
    if(ui->guidelinesType->currentText() == "None") {
        ui->rows->setEnabled(false);
        ui->columns->setEnabled(false);
        ui->cellWidth->setEnabled(false);
        ui->cellHeight->setEnabled(false);

    } else {
        ui->rows->setEnabled(true);
        ui->columns->setEnabled(true);
        ui->cellWidth->setEnabled(true);
        ui->cellHeight->setEnabled(true);

        if( ui->guidelinesType->currentText() == "Rows") {
            ui->rowsLbl->setText(tr("Rows:"));
            ui->cellWidthLbl->show();
            ui->cellWidth->show();
        } else { //rounds
            ui->rowsLbl->setText(tr("Rounds:"));
            ui->cellWidthLbl->hide();
            ui->cellWidth->hide();
        }
    }

}

void PropertiesDock::indicatorUpdate()
{
    QString html = "ui";
    QString style = ui->indicatorStyle->currentText();

    IndicatorProperties ip;
    ip.setHtml(html);
    ip.setStyle(style);

    QVariant value;
    value.setValue(ip);

    emit propertiesUpdated("Indicator", value);
}

/**
 *Chart functions:
 */

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


/**
 *Cell functions:
 */

void PropertiesDock::cellUpdateAngle(double angle)
{
    emit propertiesUpdated("Angle", QVariant(angle));
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

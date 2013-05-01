#include "propertiesdock.h"
#include "ui_propertiesdock.h"

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

    clearUi();
    connect(mTabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));

    connect(ui->angle, SIGNAL(valueChanged(double)), SLOT(cellUpdateAngle(double)));
    connect(ui->scaleX, SIGNAL(valueChanged(double)), SLOT(cellUpdateScaleX(double)));
    connect(ui->scaleY, SIGNAL(valueChanged(double)), SLOT(cellUpdateScaleY(double)));

    connect(ui->showChartCenter, SIGNAL(toggled(bool)), SLOT(chartUpdateChartCenter(bool)));
    connect(ui->showGuidelines, SIGNAL(currentIndexChanged(QString)), SLOT(chartUpdateGuidelines(QString)));

    connect(ui->stitch, SIGNAL(currentIndexChanged(QString)), SLOT(cellUpdateStitch(QString)));

    connect(ui->deleteItems, SIGNAL(clicked()), SLOT(cellDeleteItems()));

    connect(ui->showGuidelines, SIGNAL(currentIndexChanged(int)), SLOT(updateGuidelinesUi()));
    updateGuidelinesUi();

    connect(ui->rows, SIGNAL(valueChanged(int)), SLOT(updateRows(int)));
    connect(ui->cellWidth, SIGNAL(valueChanged(int)), SLOT(updateCellWidth(int)));
    connect(ui->columns, SIGNAL(valueChanged(int)), SLOT(updateColumns(int)));
    connect(ui->cellHeight, SIGNAL(valueChanged(int)), SLOT(updateCellHeight(int)));

}

PropertiesDock::~PropertiesDock()
{
    delete ui;
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

void PropertiesDock::updateGuidelines()
{
    mGuidelines.type = ui->showGuidelines->currentText();
    mGuidelines.rows = ui->rows->value();
    mGuidelines.columns = ui->columns->value();
    mGuidelines.cellHeight = ui->cellHeight->value();
    mGuidelines.cellWidth = ui->cellWidth->value();
}

void PropertiesDock::updateDialogUi()
{
    
    clearUi();
    
    int count = mScene->selectedItems().count();

    if(count == 0) {
    
        showUi(PropertiesDock::SceneUi);
        return;
    } else if(count == 1) {

        if(mScene->selectedItems().first()->type() == Cell::Type) {
            showUi(PropertiesDock::CellUi);
        } else {
            WARN("another type");
        }
    } else if(count > 1) {
        WARN("TODO: check if all items the same, if so show that dialog");
        bool theSame = true;
        
        int firstType = mScene->selectedItems().first()->type();
        for(int i = 1; i < count; ++i) {
            if(firstType != mScene->selectedItems().at(i)->type()) {
                theSame = false;
                break;
            }
        }
        qDebug() << "the same" << theSame;
        if(theSame) {
            showUi(PropertiesDock::CellUi);
        } else {
            showUi(PropertiesDock::MixedUi);
        }
    }
    
}

void PropertiesDock::showUi(PropertiesDock::UiSelection selection)
{
    
    if(selection == PropertiesDock::SceneUi) {
        ui->chartGroup->show();
        
        ui->showChartCenter->setChecked(mScene->showChartCenter());

        QString guidelines = mScene->guidelines().type;
        ui->showGuidelines->setCurrentIndex(ui->showGuidelines->findText(guidelines));
        updateGuidelinesUi();

    } else if(selection == PropertiesDock::CellUi) {

        Cell* c = qgraphicsitem_cast<Cell*>(mScene->selectedItems().first());
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
        
    } else if (selection == PropertiesDock::CenterUi) {
        WARN("TODO: make center ui work");
    }
    
}

void PropertiesDock::updateGuidelinesUi()
{
    if(ui->showGuidelines->currentText() == "None") {
        ui->rows->setEnabled(false);
        ui->columns->setEnabled(false);
        ui->cellWidth->setEnabled(false);
        ui->cellHeight->setEnabled(false);

    } else {
        ui->rows->setEnabled(true);
        ui->columns->setEnabled(true);
        ui->cellWidth->setEnabled(true);
        ui->cellHeight->setEnabled(true);

        if( ui->showGuidelines->currentText() == "Rows") {
            ui->rowsLbl->setText(tr("Rows:"));
            ui->cellWidthLbl->setText((tr("Row width:")));
        } else { //rounds
            ui->rowsLbl->setText(tr("Rounds:"));
            ui->cellWidthLbl->setText((tr("Round width:")));
        }
    }
}

void PropertiesDock::updateRows(int rows)
{
    updateGuidelines();
    emit propertiesUpdated("Rows", QVariant::fromValue(mGuidelines));
}

void PropertiesDock::updateColumns(int columns)
{
    updateGuidelines();
    emit propertiesUpdated("Columns", QVariant(columns));
}

void PropertiesDock::updateCellWidth(int width)
{
    updateGuidelines();
    emit propertiesUpdated("CellWidth", QVariant(width));
}

void PropertiesDock::updateCellHeight(int height)
{
    updateGuidelines();
    emit propertiesUpdated("CellHeight", QVariant(height));
}

/**
 *Chart functions:
 */

void PropertiesDock::chartUpdateChartCenter(bool state)
{
    emit propertiesUpdated("ChartCenter", QVariant(state));
}

void PropertiesDock::chartUpdateGuidelines(QString guides)
{

    emit propertiesUpdated("Guidelines", QVariant(guides));
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

#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"

#include "cell.h"
#include "crochettab.h"


PropertiesDialog::PropertiesDialog(QTabWidget* tabWidget, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PropertiesDialog),
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
}

PropertiesDialog::~PropertiesDialog()
{
    delete ui;
}

void PropertiesDialog::tabChanged(int tabNumber)
{

    if(tabNumber == -1) {
        clearUi();
        return;
    }
        
    mScene = qobject_cast<CrochetTab*>(mTabWidget->widget(tabNumber))->scene();
    connect(mScene, SIGNAL(selectionChanged()), SLOT(updateDialogUi()));
    updateDialogUi();

}

void PropertiesDialog::clearUi()
{
    ui->chartGroup->hide();
    ui->stitchGroup->hide();
    ui->selectionGroup->hide();
}

void PropertiesDialog::updateDialogUi()
{
    
    clearUi();
    
    int count = mScene->selectedItems().count();

    if(count == 0) {
    
        showUi(PropertiesDialog::SceneUi);
        return;
    } else if(count == 1) {

        if(mScene->selectedItems().first()->type() == Cell::Type) {
            showUi(PropertiesDialog::CellUi);
        } else {
            WARN("another type");
        }
    } else if(count > 1) {
        WARN("TODO: check if all items the same, if so show that dialog");
        bool theSame = true;
        
        for(int i = 1; i < count; ++i) {
            if(mScene->selectedItems().at(i-1)->type() != mScene->selectedItems().at(i-1)->type()) {
                theSame = false;
                break;
            }
        }

        if(theSame) {
            showUi(PropertiesDialog::CellUi);
        } else {
            showUi(PropertiesDialog::MixedUi);
        }
    }
    
}

void PropertiesDialog::showUi(PropertiesDialog::UiSelection selection)
{
    
    if(selection == PropertiesDialog::SceneUi) {
        ui->chartGroup->show();
        
        ui->showChartCenter->setChecked(mScene->showChartCenter());

        QString guidelines = mScene->guidelines();
        ui->showGuidelines->setCurrentIndex(ui->showGuidelines->findText(guidelines));
        
    } else if(selection == PropertiesDialog::CellUi) {
        Cell* c = qgraphicsitem_cast<Cell*>(mScene->selectedItems().first());
        ui->stitchGroup->show();
        ui->angle->setValue(c->rotation());
        ui->scaleX->setValue(c->scale().x());
        ui->scaleY->setValue(c->scale().y());

    } else if(selection == PropertiesDialog::MixedUi) {

        //TODO: loop through all the items, check all the

        ui->selectionGroup->show();
        
    } else if (selection == PropertiesDialog::CenterUi) {
        WARN("TODO: make center ui work");
    }
    
}

/**
 *Chart functions:
 */

void PropertiesDialog::chartUpdateChartCenter(bool state)
{
    mScene->setShowChartCenter(state);
}

void PropertiesDialog::chartUpdateGuidelines(QString guides)
{
    mScene->setShowGuidelines(guides);
}


/**
 *Cell functions:
 */

void PropertiesDialog::cellUpdateAngle(double angle)
{
    foreach(QGraphicsItem* i, mScene->selectedItems()) {
        i->setRotation(angle);
    }
}

void PropertiesDialog::cellUpdateScaleX(double scale)
{
    foreach(QGraphicsItem* i, mScene->selectedItems()) {
        Cell* c = qgraphicsitem_cast<Cell*>(i);
        c->setScale(scale, c->scale().y());
    }
}

void PropertiesDialog::cellUpdateScaleY(double scale)
{
    foreach(QGraphicsItem* i, mScene->selectedItems()) {
        Cell* c = qgraphicsitem_cast<Cell*>(i);
        c->setScale(c->scale().x(), scale);
    }
}

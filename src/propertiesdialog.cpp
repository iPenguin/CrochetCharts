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
    
    if(mScene)
        disconnect(mScene, 0, this, 0);
        
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
    
    if(mScene->selectedItems().count() <= 0) {
        showUi(PropertiesDialog::SceneUi);
        return;
    } else if(mScene->selectedItems().count() == 1) {

        
        showUi(PropertiesDialog::CellUi);
        
    } else if(mScene->selectedItems().count() > 1) {

        showUi(PropertiesDialog::MixedUi);
    }

}

void PropertiesDialog::showUi(PropertiesDialog::UiSelection selection)
{

    if(selection == PropertiesDialog::SceneUi) {
        ui->chartGroup->show();
        ui->showChartCenter->setChecked(mScene->showChartCenter());
        ui->showGuidelines->setChecked(mScene->showGuidelines());

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

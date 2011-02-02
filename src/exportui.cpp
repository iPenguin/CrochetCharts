/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "exportui.h"
#include "ui_export.h"

ExportUi::ExportUi(QTabWidget *tab, QWidget *parent)
    : QDialog(parent), exportType(""), eui(new Ui::ExportDialog), mTabWidget(tab)
{
    eui->setupUi(this);

    this->updateExportOptions(eui->fileType->currentText());
    connect(eui->fileType, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateExportOptions(QString)));
    generateSelectionList(true);

    connect(eui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(eui->buttonBox, SIGNAL(accepted()), this, SLOT(setValues()));
}

void ExportUi::updateExportOptions(QString expType)
{
    exportType = expType;
    if(exportType == "pdf") {
        eui->optionsGroupBox->setVisible(true);

        eui->selectionLbl->setVisible(true);
        eui->chartSelection->setVisible(true);
        generateSelectionList(true);
        
        eui->resolution->setVisible(true);
        eui->resolutionLbl->setVisible(true);

        eui->height->setVisible(false);
        eui->heightLbl->setVisible(false);
        eui->width->setVisible(false);
        eui->widthLbl->setVisible(false);

    } else if(exportType == "svg") {
        //eui->optionsGroupBox->setVisible(false);
        eui->selectionLbl->setVisible(true);
        eui->chartSelection->setVisible(true);
        generateSelectionList(false);
        
        eui->resolution->setVisible(false);
        eui->resolutionLbl->setVisible(false);

        eui->height->setVisible(false);
        eui->heightLbl->setVisible(false);
        eui->width->setVisible(false);
        eui->widthLbl->setVisible(false);

    } else { // jpg, jpeg, png, gif, tiff, bmp, etc.
        eui->optionsGroupBox->setVisible(true);

        eui->selectionLbl->setVisible(true);
        eui->chartSelection->setVisible(true);
        generateSelectionList(false);
        
        eui->resolution->setVisible(true);
        eui->resolutionLbl->setVisible(true);

        eui->height->setVisible(true);
        eui->heightLbl->setVisible(true);
        eui->width->setVisible(true);
        eui->widthLbl->setVisible(true);
    }
}

void ExportUi::generateSelectionList(bool showAll)
{
    eui->chartSelection->clear();
    
    QStringList options;
    if(showAll)
        options << tr("All Charts");
    
    QString curChart;
    int count = mTabWidget->count();
    for(int i = 0; i < count; ++i) {
        options << mTabWidget->tabText(i);
        if(i == mTabWidget->currentIndex())
            curChart = mTabWidget->tabText(i);
    }

    eui->chartSelection->addItems(options);
    eui->chartSelection->setCurrentIndex(eui->chartSelection->findText(curChart));
}

void ExportUi::setValues()
{
    exportType = eui->fileType->currentText();
    selection = eui->chartSelection->currentText();
    resolution = eui->resolution->text().toInt();
    width = eui->width->text().toInt();
    height = eui->height->text().toInt();

    accept();
}

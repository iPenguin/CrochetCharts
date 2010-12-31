#include "exportui.h"
#include "ui_export.h"

ExportUi::ExportUi(QWidget *parent) :
    QDialog(parent), eui(new Ui::ExportDialog)
{
    eui->setupUi(this);

    this->updateExportOptions(eui->fileType->currentText());
    connect(eui->fileType, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateExportOptions(QString)));
}

void ExportUi::updateExportOptions(QString exportType)
{

    if(exportType == "pdf") {
        eui->optionsGroupBox->setVisible(true);

        eui->selectionLbl->setVisible(true);
        eui->chartSelection->setVisible(true);
        eui->resolution->setVisible(true);
        eui->resolutionLbl->setVisible(true);

        eui->height->setVisible(false);
        eui->heightLbl->setVisible(false);
        eui->width->setVisible(false);
        eui->widthLbl->setVisible(false);

    } else if(exportType == "svg") {
        //eui->optionsGroupBox->setVisible(false);
        eui->selectionLbl->setVisible(false);
        eui->chartSelection->setVisible(false);
        eui->resolution->setVisible(false);
        eui->resolutionLbl->setVisible(false);

        eui->height->setVisible(false);
        eui->heightLbl->setVisible(false);
        eui->width->setVisible(false);
        eui->widthLbl->setVisible(false);

    } else { // jpg, jpeg, png, gif, tiff, bmp, etc.
        eui->optionsGroupBox->setVisible(true);

        eui->selectionLbl->setVisible(false);
        eui->chartSelection->setVisible(false);
        eui->resolution->setVisible(true);
        eui->resolutionLbl->setVisible(true);

        eui->height->setVisible(true);
        eui->heightLbl->setVisible(true);
        eui->width->setVisible(true);
        eui->widthLbl->setVisible(true);
    }

}

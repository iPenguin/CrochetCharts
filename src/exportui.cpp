/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "exportui.h"
#include "src/ui_export.h"

#include "settings.h"

#include <QMessageBox>
#include <QFileDialog>

#include <QPrinter> //for pdf
#include <QSvgGenerator> //for svg

ExportUi::ExportUi(QTabWidget *tab, QMap<QString, int> *stitches,
                   QMap<QString, QMap<QString, qint64> > *colors, QWidget *parent)
    : QDialog(parent), exportType(""), scene(new QGraphicsScene(this)), ui(new Ui::ExportDialog), mTabWidget(tab),
    mStitches(stitches), mColors(colors)
{
    ui->setupUi(this);
    
    ui->legendView->scale(.6, .6);
    
    setupColorLegendOptions();
    setupStitchLegendOptions();
    
    updateExportOptions(ui->fileType->currentText());
    connect(ui->fileType, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateExportOptions(QString)));
    generateSelectionList(true);

    connect(ui->chartSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(setSelection(QString)));
    
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(setValues()));

    ui->legendView->setScene(scene);
}

void ExportUi::setupColorLegendOptions()
{   
    cl = new ColorLegend(mColors);
    ui->colorLegendOptions->hide();

    ui->colorHexValue->setChecked(Settings::inst()->value("showHexValues").toBool());
    ui->colorColumns->setValue(Settings::inst()->value("colorColumnCount").toInt());
    ui->colorPrefix->setText(Settings::inst()->value("colorPrefix").toString());
    ui->colorBorder->setChecked(Settings::inst()->value("showColorBorder").toBool());
    ui->colorTitle->setChecked(Settings::inst()->value("showColorTitle").toBool());
    int index = ui->colorSortBy->findText(Settings::inst()->value("colorSortBy").toString());
    ui->colorSortBy->setCurrentIndex(index);
    
    connect(ui->colorBorder, SIGNAL(toggled(bool)), SLOT(updateColorLegend()));
    connect(ui->colorHexValue, SIGNAL(toggled(bool)), SLOT(updateColorLegend()));
    connect(ui->colorColumns, SIGNAL(valueChanged(int)), SLOT(updateColorLegend()));
    connect(ui->colorPrefix, SIGNAL(textChanged(QString)), SLOT(updateColorLegend()));
    connect(ui->colorBorder, SIGNAL(toggled(bool)), SLOT(updateColorLegend()));
    connect(ui->colorTitle, SIGNAL(toggled(bool)), SLOT(updateColorLegend()));
    connect(ui->colorSortBy, SIGNAL(currentIndexChanged(int)), SLOT(updateColorLegend()));
    
}

void ExportUi::setupStitchLegendOptions()
{
    sl = new StitchLegend(mStitches);
    ui->stitchLegendOptions->hide();

    ui->stitchBorder->setChecked(Settings::inst()->value("showStitchBorder").toBool());
    ui->stitchColumns->setValue(Settings::inst()->value("stitchColumnCount").toInt());
    ui->stitchTitle->setChecked(Settings::inst()->value("showStitchTitle").toBool());
    ui->showStitchDescription->setChecked(Settings::inst()->value("showStitchDescription").toBool());
    ui->showStitchWrongSide->setChecked(Settings::inst()->value("showWrongSideDescription").toBool());
    ui->showStitchBlocks->setChecked(Settings::inst()->value("showStitchBlock").toBool());

    connect(ui->stitchBorder, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
    connect(ui->stitchColumns, SIGNAL(valueChanged(int)), SLOT(updateStitchLegend()));
    connect(ui->stitchTitle, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
    connect(ui->showStitchDescription, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
    connect(ui->showStitchWrongSide, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
    connect(ui->showStitchBlocks, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
}

void ExportUi::updateColorLegend()
{
    cl->showHexValues = ui->colorHexValue->isChecked();
    cl->columnCount = ui->colorColumns->value();
    cl->prefix = ui->colorPrefix->text();
    cl->showBorder = ui->colorBorder->isChecked();
    cl->showTitle = ui->colorTitle->isChecked();
    cl->sortBy = ui->colorSortBy->currentText();
    
    scene->update();
}

void ExportUi::updateStitchLegend()
{
    sl->showBorder = ui->stitchBorder->isChecked();
    sl->columnCount = ui->stitchColumns->value();
    sl->showTitle = ui->stitchTitle->isChecked();
    sl->showDescription = ui->showStitchDescription->isChecked();
    sl->showWrongSide = ui->showStitchWrongSide->isChecked();
    sl->showBlocks = ui->showStitchBlocks->isChecked();
    
    scene->update();
}

void ExportUi::updateExportOptions(QString expType)
{
    exportType = expType;
    if(exportType == "pdf") {
        ui->optionsGroupBox->setVisible(true);

        ui->selectionLbl->setVisible(true);
        ui->chartSelection->setVisible(true);
        generateSelectionList(true);
        
        ui->resolution->setVisible(true);
        ui->resolutionLbl->setVisible(true);

        ui->height->setVisible(false);
        ui->heightLbl->setVisible(false);
        ui->width->setVisible(false);
        ui->widthLbl->setVisible(false);

    } else if(exportType == "svg") {
        //eui->optionsGroupBox->setVisible(false);
        ui->selectionLbl->setVisible(true);
        ui->chartSelection->setVisible(true);
        generateSelectionList(false);
        
        ui->resolution->setVisible(false);
        ui->resolutionLbl->setVisible(false);

        ui->height->setVisible(false);
        ui->heightLbl->setVisible(false);
        ui->width->setVisible(false);
        ui->widthLbl->setVisible(false);

    } else { // jpg, jpeg, png, gif, tiff, bmp, etc.
        ui->optionsGroupBox->setVisible(true);

        ui->selectionLbl->setVisible(true);
        ui->chartSelection->setVisible(true);
        generateSelectionList(false);
        
        ui->resolution->setVisible(true);
        ui->resolutionLbl->setVisible(true);

        ui->height->setVisible(true);
        ui->heightLbl->setVisible(true);
        ui->width->setVisible(true);
        ui->widthLbl->setVisible(true);
    }
}

void ExportUi::generateSelectionList(bool showAll)
{
    ui->chartSelection->clear();
    
    QStringList options;
    if(showAll)
        options << tr("All Charts");
    options << tr("Stitch Legend") << tr("Color Legend");
    
    QString curChart;
    int count = mTabWidget->count();
    for(int i = 0; i < count; ++i) {
        options << mTabWidget->tabText(i);
        if(i == mTabWidget->currentIndex())
            curChart = mTabWidget->tabText(i);
    }

    ui->chartSelection->addItems(options);
    ui->chartSelection->setCurrentIndex(ui->chartSelection->findText(curChart));
}

void ExportUi::setValues()
{
    exportType = ui->fileType->currentText();
    selection = ui->chartSelection->currentText();
    resolution = ui->resolution->text().toInt();
    width = ui->width->text().toInt();
    height = ui->height->text().toInt();
    
    accept();
}

int ExportUi::exec()
{
    int retValue = QDialog::exec();
    
    if(retValue != QDialog::Accepted)
        return retValue;

    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return QDialog::Rejected;
    }
    
    QString filter;
    if(exportType == "pdf")
        filter = tr("Portable Document Format (pdf)(*.pdf)");
    else if(exportType == "svg")
        filter = tr("Scaled Vector Graphics (svg)(*.svg *.svgz)");
    else if(exportType == "jpeg")
        filter = tr("Joint Photographic Experts Group (jpeg)(*.jpeg *.jpg)");
    else if(exportType == "png")
        filter = tr("Portable Network Graphics (png)(*.png)");
    else if(exportType == "gif")
        filter = tr("Graphics Interchange Format (gif)(*.gif)");
    else if(exportType == "tiff")
        filter = tr("Tagged Image File Format (tiff)(*.tiff *.tif)");
    else if(exportType == "bmp")
        filter = tr("Bitmap (bmp)(*.bmp)");
    else
        filter = tr("");
    
    QString fileLoc = Settings::inst()->value("fileLocation").toString();
    fileName = QFileDialog::getSaveFileName(this, tr("Export Pattern As..."), fileLoc, filter);

    if(fileName.isEmpty())
        return QDialog::Rejected;

    if(selection == tr("Stitch Legend") || selection == tr("Color Legend")) {

        if(exportType == "pdf")
            exportLegendPdf();
        else if (exportType == "svg")
            exportLegendSvg();
        else
            exportLegendImg();
    }
    
    return retValue;
}

void ExportUi::setSelection(QString selection)
{
    if(selection.isEmpty())
        return;

    if(selection == tr("Stitch Legend")) {
        ui->stitchLegendOptions->show();
        ui->colorLegendOptions->hide();
        if(scene->items().contains(cl))
            scene->removeItem(cl);
        if(!scene->items().contains(sl))
            scene->addItem(sl);
    } else if(selection == tr("Color Legend")) {
        ui->stitchLegendOptions->hide();
        ui->colorLegendOptions->show();
        
        if(scene->items().contains(sl))
            scene->removeItem(sl);
        if(!scene->items().contains(cl))
            scene->addItem(cl);
    } else {
        ui->stitchLegendOptions->hide();
        ui->colorLegendOptions->hide();
    }
}

void ExportUi::exportLegendPdf()
{
    QPainter *p = new QPainter();
    
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);
    printer->setResolution(resolution);
    
    p->begin(printer);
    scene->render(p);
    p->end();
}

void ExportUi::exportLegendSvg()
{
    QPainter *p = new QPainter();
    
    QSvgGenerator gen;

    gen.setResolution(resolution);
    gen.setTitle(QFileInfo(fileName).baseName());
    gen.setDescription(tr("This file was generated by %1").arg(qApp->applicationName()) );
    gen.setViewBox(scene->sceneRect());
    gen.setFileName(fileName);
    gen.setSize(scene->sceneRect().size().toSize());
    
    p->begin(&gen);
    scene->render(p);
    p->end();
}

void ExportUi::exportLegendImg()
{
    if(mColors->count() < 1) {
        QMessageBox msgbox(this);
        msgbox.setText(tr("There are no colors to put into the key."));
        msgbox.setInformativeText(tr("A color key will not be generated."));
        msgbox.setIcon(QMessageBox::Information);
        msgbox.setStandardButtons(QMessageBox::Ok);
        
        msgbox.exec();
        return;
    }
    
    QPainter p(this);
    QPixmap pix = QPixmap(scene->sceneRect().size().toSize());

    p.begin(&pix);
    scene->render(&p);
    p.end();
    pix.save(fileName);
}

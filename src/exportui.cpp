/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "exportui.h"
#include "ui_export.h"

#include <QDebug>

#include "settings.h"
#include "math.h"

#include <QMessageBox>
#include <QFileDialog>

#include <QPrinter> //for pdf
#include <QSvgGenerator> //for svg

#include "crochettab.h"
#include "scene.h" // for to connect the scene to the view.

ExportUi::ExportUi(QTabWidget* tab, QMap<QString, int>* stitches,
                   QMap<QString, QMap<QString, qint64> >* colors, QWidget* parent)
    : QDialog(parent),
      exportType(""),
      scene(new QGraphicsScene(this)),
      ui(new Ui::ExportDialog),
      mTabWidget(tab),
      mStitches(stitches),
      mColors(colors)
{
    ui->setupUi(this);
    
    ui->view->scale(.6, .6);

    //TODO: use later...
    ui->showStitchWrongSide->hide();
    ui->showStitchWrongSideLbl->hide();
    ui->colorSortBy->hide();
    ui->colorSortByLbl->hide();
    //FIXME: chart options...
    ui->chartOptions->hide();
    
    setupChartOptions();
    setupColorLegendOptions();
    setupStitchLegendOptions();
    
    updateExportOptions(ui->fileType->currentText());
    connect(ui->fileType, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateExportOptions(QString)));
    generateSelectionList(true);

    connect(ui->chartSelection, SIGNAL(currentIndexChanged(QString)), this, SLOT(setSelection(QString)));
    
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(exportData()));

    connect(ui->width, SIGNAL(valueChanged(int)), SLOT(updateHightFromWidth(int)));
    connect(ui->height, SIGNAL(valueChanged(int)), SLOT(updateWidthFromHeight(int)));

}

void ExportUi::setupChartOptions()
{

}

void ExportUi::setupColorLegendOptions()
{   
    cl = new ColorLegend(mColors);
    ui->colorLegendOptions->hide();

    ui->colorHexValue->setChecked(Settings::inst()->value("showColorHexValues").toBool());
    ui->colorColumns->setValue(Settings::inst()->value("colorLegendColumnCount").toInt());
    ui->colorPrefix->setText(Settings::inst()->value("colorPrefix").toString());
    ui->colorBorder->setChecked(Settings::inst()->value("showColorBorder").toBool());
    ui->colorTitle->setChecked(Settings::inst()->value("showColorTitle").toBool());
    int index = ui->colorSortBy->findText(Settings::inst()->value("colorLegendSortBy").toString());
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
    ui->stitchColumns->setValue(Settings::inst()->value("stitchLegendColumnCount").toInt());
    ui->stitchTitle->setChecked(Settings::inst()->value("showStitchTitle").toBool());
    ui->showStitchDescription->setChecked(Settings::inst()->value("showStitchDescription").toBool());
    ui->showStitchWrongSide->setChecked(Settings::inst()->value("showStitchWrongSide").toBool());

    connect(ui->stitchBorder, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
    connect(ui->stitchColumns, SIGNAL(valueChanged(int)), SLOT(updateStitchLegend()));
    connect(ui->stitchTitle, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
    connect(ui->showStitchDescription, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
    connect(ui->showStitchWrongSide, SIGNAL(toggled(bool)), SLOT(updateStitchLegend()));
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
        ui->resolution->setValue(300);
        ui->height->setVisible(false);
        ui->heightLbl->setVisible(false);
        ui->width->setVisible(false);
        ui->widthLbl->setVisible(false);

        ui->pageToChartSize->setVisible(true);
        ui->pageSizeLbl->setVisible(true);

    } else if(exportType == "svg") {
        //eui->optionsGroupBox->setVisible(false);
        ui->selectionLbl->setVisible(true);
        ui->chartSelection->setVisible(true);
        generateSelectionList(false);
        
        ui->resolution->setVisible(false);
        ui->resolutionLbl->setVisible(false);
        ui->resolution->setValue(300);
        
        ui->height->setVisible(false);
        ui->heightLbl->setVisible(false);
        ui->width->setVisible(false);
        ui->widthLbl->setVisible(false);

        ui->pageToChartSize->setVisible(false);
        ui->pageSizeLbl->setVisible(false);
        
    } else { // jpg, jpeg, png, gif, tiff, bmp, etc.
        ui->optionsGroupBox->setVisible(true);

        ui->selectionLbl->setVisible(true);
        ui->chartSelection->setVisible(true);
        generateSelectionList(false);
        
        ui->resolution->setVisible(true);
        ui->resolutionLbl->setVisible(true);
        ui->resolution->setValue(96);
        
        ui->height->setVisible(true);
        ui->heightLbl->setVisible(true);
        ui->width->setVisible(true);
        ui->widthLbl->setVisible(true);

        ui->pageToChartSize->setVisible(false);
        ui->pageSizeLbl->setVisible(false);
    }
}

void ExportUi::generateSelectionList(bool showAll)
{
    QString curSelection = ui->chartSelection->currentText();
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

    int index = ui->chartSelection->findText(curSelection);
    if(index > 0)
        ui->chartSelection->setCurrentIndex(index);
    else
        ui->chartSelection->setCurrentIndex(ui->chartSelection->findText(curChart));

    curSelection = ui->chartSelection->currentText();
    if(curSelection == tr("Stitch Legend") || curSelection == tr("Color Legend")) {
        ui->view->setScene(scene);
    } else {
        CrochetTab* tab = 0;
        for(int i = 0; i < count; ++i) {
            if(curSelection == mTabWidget->tabText(i)) {
                tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
                break;
            }
        }

        if(!tab)
            return;
        ui->view->setScene(tab->scene());
    }
}

void ExportUi::exportData()
{
    exportType = ui->fileType->currentText();
    selection = ui->chartSelection->currentText();
    resolution = ui->resolution->text().remove(" dpi").toInt();
    width = ui->width->text().toInt();
    height = ui->height->text().toInt();
    pageToChartSize = ui->pageToChartSize->isChecked();
    
    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return;
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
        return;
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //we don't want the dotted lines in the image.
    for(int i = 0; i < mTabWidget->count(); ++i) {
        CrochetTab* t = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
        if(t) t->clearSelection();
    }

    if(selection == tr("Stitch Legend") || selection == tr("Color Legend")) {
        if(exportType == "pdf")
            exportLegendPdf();
        else if (exportType == "svg")
            exportLegendSvg();
        else
            exportLegendImg();
        
    } else { //charts
        if(exportType == "pdf")
            exportPdf();
        else if(exportType == "svg")
            exportSvg();
        else
            exportImg();
    }
    
    QApplication::restoreOverrideCursor();
}

int ExportUi::exec()
{
    int retValue = QDialog::exec();
    

    return retValue;
}

void ExportUi::setSelection(QString selection)
{
    if(selection.isEmpty())
        return;

    if(selection == tr("Stitch Legend")) {
        ui->view->setScene(scene);
        ui->stitchLegendOptions->show();
        ui->colorLegendOptions->hide();
        ui->chartOptions->hide();
        
        if(scene->items().contains(cl))
            scene->removeItem(cl);
        if(!scene->items().contains(sl))
            scene->addItem(sl);
    } else if(selection == tr("Color Legend")) {
        ui->view->setScene(scene);
        ui->stitchLegendOptions->hide();
        ui->colorLegendOptions->show();
        ui->chartOptions->hide();
        
        if(scene->items().contains(sl))
            scene->removeItem(sl);
        if(!scene->items().contains(cl))
            scene->addItem(cl);
    } else {
        CrochetTab* tab = 0;
        for(int i = 0; i < mTabWidget->count(); ++i) {
            if(selection == mTabWidget->tabText(i) || selection == tr("All Charts")) {
                tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
                break;
            }
        }
        
        if(!tab)
            return;
        
        ui->view->setScene(tab->scene());
        
        ui->stitchLegendOptions->hide();
        ui->colorLegendOptions->hide();
//FIXME: Add Chart Options.
        ui->chartOptions->hide();
        ui->view->centerOn(tab->scene()->sceneRect().center());
    }
    
    updateChartSizeRatio(selection);
}

void ExportUi::exportLegendPdf()
{
    QPainter* p = new QPainter();
    
    QPrinter* printer = new QPrinter(QPrinter::HighResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);
    printer->setResolution(resolution);

    QSizeF size = scene->sceneRect().size();
    if(pageToChartSize)
        printer->setPaperSize(size, QPrinter::Point);
    
    p->begin(printer);
    scene->render(p);
    p->end();
}

void ExportUi::exportLegendSvg()
{
    QPainter* p = new QPainter();
    
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
    
    QPainter* p = new QPainter(this);
    QPixmap pix = QPixmap(scene->sceneRect().size().toSize());

    p->begin(&pix);
    scene->render(p);
    p->end();
    pix.save(fileName);
}

void ExportUi::exportPdf()
{
    int tabCount = mTabWidget->count();
    QPainter* p = new QPainter();
    
    QPrinter* printer = new QPrinter(QPrinter::HighResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);
    printer->setResolution(resolution);

    QSizeF size = ui->view->scene()->sceneRect().size();
    if(pageToChartSize)
        printer->setPaperSize(size, QPrinter::Point);
    
    p->begin(printer);
    
    bool firstPass = true;
    for(int i = 0; i < tabCount; ++i) {
        if(!firstPass)
            printer->newPage();
        
        if(selection == tr("All Charts") || selection == mTabWidget->tabText(i)) {
            CrochetTab* tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
            tab->renderChart(p);
            firstPass = false;
            if(selection != tr("All Charts"))
                break;
        }
    }
    p->end();
}

void ExportUi::exportSvg()
{
    int tabCount = mTabWidget->count();
    
    QRectF rect = ui->view->scene()->sceneRect();

    QSvgGenerator gen;
    gen.setFileName(fileName);
    gen.setSize(rect.size().toSize());
    gen.setViewBox(rect);
        
    gen.setResolution(resolution);
    gen.setTitle(QFileInfo(fileName).baseName());
    gen.setDescription(tr("This file was generated by %1").arg(qApp->applicationName()) );

    QPainter p;
    p.begin(&gen);
    
    for(int i = 0; i < tabCount; ++i) {
        if(selection == mTabWidget->tabText(i)) {
            CrochetTab* tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
            tab->renderChart(&p, QRectF(QPointF(0,0),QSizeF((qreal)width, (qreal)height)));
        }
    }
    p.end();

}

void ExportUi::exportImg()
{
    int tabCount = mTabWidget->count();
    QPainter* p = new QPainter();
    
    double dpm = resolution * (39.3700787);
    QImage img = QImage(QSize(width, height), QImage::Format_ARGB32);
    img.setDotsPerMeterX(dpm);
    img.setDotsPerMeterY(dpm);
    
    p->begin(&img);
    p->fillRect(0, 0, width, height, QColor(Qt::white));
    
    for(int i = 0; i < tabCount; ++i) {
        if(selection == mTabWidget->tabText(i)) {
            CrochetTab* tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
            tab->renderChart(p, QRectF(QPointF(0,0),QSizeF((qreal)width, (qreal)height)));
        }
    }
    p->end();
    
    img.save(fileName);
}

void ExportUi::updateChartSizeRatio(QString selection)
{

    if(selection == tr("Stitch Legend") || selection == tr("Color Legend")) {
        ui->width->setValue(ui->view->sceneRect().width());
        ui->height->setValue(ui->view->sceneRect().height());
    } else {

        CrochetTab* tab = 0;
        for(int i = 0; i < mTabWidget->count(); ++i) {
            if(selection == mTabWidget->tabText(i)) {
                tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
                break;
            }
        }
        if(!tab)
            return;
        
        QRectF rect = tab->scene()->itemsBoundingRect();

        ui->height->setValue(rect.height());
        ui->width->setValue(rect.width());
    }
}

qreal ExportUi::sceneRatio()
{
    qreal ratio = 1.0;
    ratio = ui->view->sceneRect().height() / ui->view->sceneRect().width();
    return ratio;
}

void ExportUi::updateWidthFromHeight(int height)
{
    int width = ceil(height / sceneRatio());
    ui->width->blockSignals(true);
    ui->width->setValue(width);
    ui->width->blockSignals(false);
}

void ExportUi::updateHightFromWidth(int width)
{
    int height = ceil(width * sceneRatio());
    ui->height->blockSignals(true);
    ui->height->setValue(height);
    ui->height->blockSignals(false);
}

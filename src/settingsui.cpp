/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "settingsui.h"
#include "ui_settings.h"

#include <QFileDialog>

#include "settings.h"
#include <QPainter>
#include <QColorDialog>

SettingsUi::SettingsUi(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0); //in case the form gets saved on the wrong tab.

#ifdef Q_WS_MAC
    this->setWindowTitle(tr("Preferences"));
#else
    this->setWindowTitle(tr("Options"));
#endif //Q_WS_MAC

    connect(ui->folderSelector, SIGNAL(clicked()), this, SLOT(selectFolder()));

    connect(ui->primaryColorBttn, SIGNAL(clicked()), this, SLOT(updatePrimaryColor()));
    connect(ui->alternateColorBttn, SIGNAL(clicked()), this, SLOT(updateAlternateColor()));
    
    loadApplicationSettings();
    loadRoundChartSettings();
    loadInstructionSettings();
    loadLegendSettings();

}

SettingsUi::~SettingsUi()
{
    delete ui;
    ui = 0;
}

int SettingsUi::exec()
{
    
    int retValue = QDialog::exec();
    
    //TODO: save values to disk based on retValue;
    if(retValue != QDialog::Accepted)
        return retValue;
    
    saveApplicationSettings();
    saveRoundChartSettings();
    saveInstructionSettings();
    saveLegendSettings();
    
    return retValue;
}



void SettingsUi::loadApplicationSettings()
{
    //TODO: use auto completer to help fill in the default file location field.
    
    ui->defaultFileLocation->setText(
        Settings::inst()->value("fileLocation").toString());
    
    ui->checkForUpdates->setChecked(
        Settings::inst()->value("checkForUpdates").toBool());
}

void SettingsUi::saveApplicationSettings()
{
    Settings::inst()->setValue("fileLocation", QVariant(ui->defaultFileLocation->text()));
    Settings::inst()->setValue("checkForUpdates", QVariant(ui->checkForUpdates->isChecked()));
    
}



void SettingsUi::loadRoundChartSettings()
{

    ui->defaultRows->setValue(
        Settings::inst()->value("defaultRows").toInt());
    ui->defaultStitches->setValue(
        Settings::inst()->value("defaultStitches").toInt());

    QStringList stitches;
    stitches << "ch"; //FIXME: don't hard code the choice of default stitches.
    ui->defaultStitch->addItems(stitches);
    int index = ui->defaultStitch->findText(Settings::inst()->value("defaultStitch").toString());
    ui->defaultStitch->setCurrentIndex(index);

    ui->alternateRowColors->setChecked(Settings::inst()->value("alternateRowColors").toBool());
    
    QString priColor = Settings::inst()->value("stitchPrimaryColor").toString();
    QString altColor = Settings::inst()->value("stitchAlternateColor").toString();

    ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(priColor), QSize(32, 32))));
    ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(altColor), QSize(32, 32))));

    mPrimaryColor = priColor;
    mAlternateColor = altColor;
    
}

void SettingsUi::saveRoundChartSettings()
{
    Settings::inst()->setValue("defaultRows", QVariant(ui->defaultRows->value()));
    Settings::inst()->setValue("defaultStitches", QVariant(ui->defaultStitches->value()));
    Settings::inst()->setValue("defaultStitch", QVariant(ui->defaultStitch->currentText()));

    Settings::inst()->setValue("alternateRowColors", QVariant(ui->alternateRowColors->isChecked()));
    Settings::inst()->setValue("stitchPrimaryColor", QVariant(mPrimaryColor.name()));
    Settings::inst()->setValue("stitchAlternateColor", QVariant(mAlternateColor.name()));
    
}



void SettingsUi::loadInstructionSettings()
{

}

void SettingsUi::saveInstructionSettings()
{
    
}



void SettingsUi::loadLegendSettings()
{
    ui->stitchColumnCount->setValue(Settings::inst()->value("stitchColumnCount").toInt());
    ui->showStitchDescription->setChecked(Settings::inst()->value("showStitchDescription").toBool());
    ui->showWrongSideDescription->setChecked(Settings::inst()->value("showWrongSideDescription").toBool());
    
    ui->colorPrefix->setText(Settings::inst()->value("colorPrefix").toString());
    ui->colorColumnCount->setValue(Settings::inst()->value("colorColumnCount").toInt());
    ui->showHexValues->setChecked(Settings::inst()->value("showHexValues").toBool());
}

void SettingsUi::saveLegendSettings()
{
    Settings::inst()->setValue("stitchColumnCount", QVariant(ui->stitchColumnCount->value()));
    Settings::inst()->setValue("showStitchDescription", QVariant(ui->showStitchDescription->isChecked()));
    Settings::inst()->setValue("showWrongSideDescription", QVariant(ui->showWrongSideDescription->isChecked()));
    
    Settings::inst()->setValue("colorPrefix", QVariant(ui->colorPrefix->text()));
    Settings::inst()->setValue("colorColumnCount", QVariant(ui->colorColumnCount->value()));
    Settings::inst()->setValue("showHexValues", QVariant(ui->showHexValues->isChecked()));
}



void SettingsUi::selectFolder()
{
    QString defLoc = Settings::inst()->value("fileLocation").toString();

    QString folder = QFileDialog::getExistingDirectory(this, tr("Select Default Folder"), defLoc);

    if(folder.isEmpty())
        return;

    ui->defaultFileLocation->setText(folder);
}

QPixmap SettingsUi::drawColorBox(QColor color, QSize size)
{
    QPixmap pix = QPixmap(size);
    QPainter p;
    p.begin(&pix);
    p.fillRect(QRect(QPoint(0, 0), size), QColor(color));
    p.drawRect(0, 0, size.width() - 1, size.height() - 1);
    p.end();
    
    return pix;
}

void SettingsUi::updatePrimaryColor()
{
    QColor color = QColorDialog::getColor(mPrimaryColor, this, tr("Primary Row Color"));

    if (color.isValid()) {
        ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
        mPrimaryColor = color;
    }
}


void SettingsUi::updateAlternateColor()
{
    QColor color = QColorDialog::getColor(mAlternateColor, this, tr("Alternate Row Color"));
    
    if (color.isValid()) {
        ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
        mAlternateColor = color;
    }
}

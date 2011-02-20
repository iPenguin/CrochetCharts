/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "settingsui.h"
#include "ui_settings.h"

#include <QDebug>
#include <QFileDialog>

#include "settings.h"
#include <QPainter>
#include <QColorDialog>
#include "stitchlibrary.h"


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

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    
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

void SettingsUi::buttonClicked(QAbstractButton* button)
{
    if(ui->buttonBox->buttonRole(button) ==  QDialogButtonBox::ResetRole) {
        resetApplicationSettings();
        resetRoundChartSettings();
        resetInstructionSettings();
        resetLegendSettings();
    }
        
}

int SettingsUi::exec()
{
    int retValue = QDialog::exec();
    
    if(retValue != QDialog::Accepted)
        return retValue;
    
    saveApplicationSettings();
    saveRoundChartSettings();
    saveInstructionSettings();
    saveLegendSettings();
    
    return retValue;
}

void SettingsUi::load(QWidget* w)
{
    QVariant value = Settings::inst()->value(w->objectName());
    
    if(w->inherits("QLineEdit")) {
        qobject_cast<QLineEdit*>(w)->setText(value.toString());
    } else if (w->inherits("QCheckBox")) {
        qobject_cast<QCheckBox*>(w)->setChecked(value.toBool());
    } else if (w->inherits("QSpinBox")) {
        qobject_cast<QSpinBox*>(w)->setValue(value.toInt());
    } else if (w->inherits("QComboBox")) {
        QComboBox *cb = qobject_cast<QComboBox*>(w);
        int index = cb->findText(value.toString());
        cb->setCurrentIndex(index);
    } else {
        qWarning() << "Trying to load unknown settings type";
    }
        
}

void SettingsUi::loadDefualt(QWidget* w)
{
    QVariant value = Settings::inst()->defaultValue(w->objectName());
    
    if(w->inherits("QLineEdit")) {
        qobject_cast<QLineEdit*>(w)->setText(value.toString());
    } else if (w->inherits("QCheckBox")) {
        qobject_cast<QCheckBox*>(w)->setChecked(value.toBool());
    } else if (w->inherits("QSpinBox")) {
        qobject_cast<QSpinBox*>(w)->setValue(value.toInt());
    } else if (w->inherits("QComboBox")) {
        QComboBox *cb = qobject_cast<QComboBox*>(w);
        int index = cb->findText(value.toString());
        cb->setCurrentIndex(index);
    } else {
        qWarning() << "Trying to load unknown settings type";
    }
}

void SettingsUi::save(QWidget* w)
{
    QVariant value;
    if(w->inherits("QLineEdit")) {
        value = QVariant(qobject_cast<QLineEdit*>(w)->text());
    } else if (w->inherits("QCheckBox")) {
        value = QVariant(qobject_cast<QCheckBox*>(w)->isChecked());
    } else if (w->inherits("QSpinBox")) {
        value = QVariant(qobject_cast<QSpinBox*>(w)->value());
    } else if (w->inherits("QComboBox")) {
        value = QVariant(qobject_cast<QComboBox*>(w)->currentText());
    } else {
        qWarning() << "Trying to save unknown settings type";
    }
    
    if(value.isValid())
        Settings::inst()->setValue(w->objectName(), value);
}

void SettingsUi::loadApplicationSettings()
{
    //TODO: use auto completer to help fill in the default file location field.

    ui->defaultFileLocation->setObjectName("fileLocation");
    ui->checkForUpdates->setObjectName("checkForUpdates");

    load(ui->defaultFileLocation);
    load(ui->checkForUpdates);
}

void SettingsUi::saveApplicationSettings()
{
    save(ui->defaultFileLocation);
    save(ui->checkForUpdates);    
}

void SettingsUi::resetApplicationSettings()
{
    loadDefualt(ui->defaultFileLocation);
    loadDefualt(ui->checkForUpdates);
}


void SettingsUi::loadRoundChartSettings()
{

    ui->defaultRows->setObjectName("defaultRows");
    ui->defaultStitches->setObjectName("defaultStitches");
    ui->generateRepeats->setObjectName("generateRepeats");

    load(ui->defaultRows);
    load(ui->defaultStitches);
    load(ui->generateRepeats);

    ui->defaultStitch->addItems(StitchLibrary::inst()->stitchList());
    ui->defaultStitch->setObjectName("defaultStitch");
    load(ui->defaultStitch);

    ui->alternateRowColors->setObjectName("alternateRowColors");
    load(ui->alternateRowColors);
    
    QString priColor = Settings::inst()->value("stitchPrimaryColor").toString();
    QString altColor = Settings::inst()->value("stitchAlternateColor").toString();

    ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(priColor), QSize(32, 32))));
    ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(altColor), QSize(32, 32))));

    mPrimaryColor = priColor;
    mAlternateColor = altColor;

    ui->defaultPlaceholder->addItems(StitchLibrary::inst()->stitchList());
    ui->defaultPlaceholder->setObjectName("defaultPlaceholder");
    load(ui->defaultPlaceholder);
    
}

void SettingsUi::saveRoundChartSettings()
{
    save(ui->defaultRows);
    save(ui->defaultStitches);
    save(ui->defaultStitch);
    save(ui->defaultPlaceholder);

    save(ui->generateRepeats);
    save(ui->alternateRowColors);
    
    Settings::inst()->setValue("stitchPrimaryColor", QVariant(mPrimaryColor.name()));
    Settings::inst()->setValue("stitchAlternateColor", QVariant(mAlternateColor.name()));
    
}

void SettingsUi::resetRoundChartSettings()
{

    loadDefualt(ui->defaultRows);
    loadDefualt(ui->defaultStitches);
    loadDefualt(ui->defaultStitch);
    loadDefualt(ui->defaultPlaceholder);
    loadDefualt(ui->alternateRowColors);
    loadDefualt(ui->generateRepeats);

    QString priColor = Settings::inst()->defaultValue("stitchPrimaryColor").toString();
    QString altColor = Settings::inst()->defaultValue("stitchAlternateColor").toString();
    
    ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(priColor), QSize(32, 32))));
    ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(altColor), QSize(32, 32))));
    
    mPrimaryColor = priColor;
    mAlternateColor = altColor;
    
}


void SettingsUi::loadInstructionSettings()
{

}

void SettingsUi::saveInstructionSettings()
{
    
}

void SettingsUi::resetInstructionSettings()
{

}


void SettingsUi::loadLegendSettings()
{
    ui->stitchColumnCount->setObjectName("stitchColumnCount");
    ui->showStitchDescription->setObjectName("showStitchDescription");
    ui->showWrongSideDescription->setObjectName("showWrongSideDescription");
    
    load(ui->stitchColumnCount);
    load(ui->showStitchDescription);
    load(ui->showWrongSideDescription);
    
    ui->colorPrefix->setObjectName("colorPrefix");
    ui->colorColumnCount->setObjectName("colorColumnCount");
    ui->showHexValues->setObjectName("showHexValues");

    load(ui->colorPrefix);
    load(ui->colorColumnCount);
    load(ui->showHexValues);
    
}

void SettingsUi::saveLegendSettings()
{

    save(ui->stitchColumnCount);
    save(ui->showStitchDescription);
    save(ui->showWrongSideDescription);
    save(ui->colorPrefix);
    save(ui->colorColumnCount);
    save(ui->showHexValues);

}

void SettingsUi::resetLegendSettings()
{
    loadDefualt(ui->stitchColumnCount);
    loadDefualt(ui->showStitchDescription);
    loadDefualt(ui->showWrongSideDescription);
    loadDefualt(ui->colorPrefix);
    loadDefualt(ui->colorColumnCount);
    loadDefualt(ui->showHexValues);
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

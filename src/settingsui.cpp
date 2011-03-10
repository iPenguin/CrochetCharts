/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "settingsui.h"
#include "src/ui_settings.h"

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

    setupDialogWidgets();

    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        foreach(QObject *obj, ui->tabWidget->widget(i)->children()) {
            if(isSettingsWidget(obj))
                load(obj);
        }
    }
}

SettingsUi::~SettingsUi()
{
    delete ui;
    ui = 0;
}

void SettingsUi::buttonClicked(QAbstractButton* button)
{
    if(ui->buttonBox->buttonRole(button) ==  QDialogButtonBox::ResetRole) {
        for(int i = 0; i < ui->tabWidget->count(); ++i) {
            foreach(QObject *obj, ui->tabWidget->widget(i)->children()) {
                if(isSettingsWidget(obj))
                    loadDefualt(obj);
            }
        }
        resetDialogWidgets();
    }
}

int SettingsUi::exec()
{
    int retValue = QDialog::exec();
    
    if(retValue != QDialog::Accepted)
        return retValue;

    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        foreach(QObject *obj, ui->tabWidget->widget(i)->children()) {
            if(isSettingsWidget(obj))
                save(obj);
        }
    }
    saveDialogWidgets();
    
    return retValue;
}

void SettingsUi::load(QObject* w)
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

void SettingsUi::loadDefualt(QObject* w)
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

void SettingsUi::save(QObject* w)
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

bool SettingsUi::isSettingsWidget(QObject *obj)
{
    if(obj->inherits("QLineEdit"))
        return true;
    if(obj->inherits("QCheckBox"))
        return true;
    if(obj->inherits("QSpinBox"))
        return true;
    if(obj->inherits("QComboBox"))
        return true;

    return false;
}


void SettingsUi::setupDialogWidgets()
{
    //Application
    //TODO: use auto completer to help fill in the default file location field.

    //Charts
    ui->defaultStitch->addItems(StitchLibrary::inst()->stitchList());

    QString priColor = Settings::inst()->value("stitchPrimaryColor").toString();
    QString altColor = Settings::inst()->value("stitchAlternateColor").toString();
    
    ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(priColor), QSize(32, 32))));
    ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(altColor), QSize(32, 32))));
    
    mPrimaryColor = priColor;
    mAlternateColor = altColor;

    ui->placeholder->addItems(StitchLibrary::inst()->stitchList());
    
    //Instructions

    //Legends
    QStringList list;
    list << tr("Age") << tr("Color") << tr("Quantity");
    ui->colorLegendSortBy->addItems(list);
    
}

void SettingsUi::saveDialogWidgets()
{
    //Application

    //Charts
    Settings::inst()->setValue("stitchPrimaryColor", QVariant(mPrimaryColor.name()));
    Settings::inst()->setValue("stitchAlternateColor", QVariant(mAlternateColor.name()));

    //Instructions

    //Legends
}

void SettingsUi::resetDialogWidgets()
{
    //Application
    
    //Charts
    QString priColor = Settings::inst()->defaultValue("stitchPrimaryColor").toString();
    QString altColor = Settings::inst()->defaultValue("stitchAlternateColor").toString();
    
    ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(priColor), QSize(32, 32))));
    ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(altColor), QSize(32, 32))));
    
    mPrimaryColor = priColor;
    mAlternateColor = altColor;
    
    //Instructions
    
    //Legends
}

void SettingsUi::selectFolder()
{
    QString defLoc = Settings::inst()->value("fileLocation").toString();

    QString folder = QFileDialog::getExistingDirectory(this, tr("Select Default Folder"), defLoc);

    if(folder.isEmpty())
        return;

    ui->fileLocation->setText(folder);
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

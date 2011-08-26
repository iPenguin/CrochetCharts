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


SettingsUi::SettingsUi(QWidget* parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    
    //in case the form gets saved on the wrong tab.
    ui->tabWidget->setCurrentIndex(0);

#ifdef Q_WS_MAC
    this->setWindowTitle(tr("Preferences"));
#else
    this->setWindowTitle(tr("Options"));
#endif //Q_WS_MAC

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(buttonClicked(QAbstractButton*)));


    //TODO: add later for expanded use of software.
    ui->showStitchWrongSide->hide();
    ui->showStitchWrongSideLbl->hide();
    ui->colorLegendSortBy->hide();
    ui->colorSortByLbl->hide();
    
    setupDialogWidgets();

    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        foreach(QObject* obj, ui->tabWidget->widget(i)->children()) {
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
            foreach(QObject* obj, ui->tabWidget->widget(i)->children()) {
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
        foreach(QObject* obj, ui->tabWidget->widget(i)->children()) {
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
        QComboBox* cb = qobject_cast<QComboBox*>(w);
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
        QComboBox* cb = qobject_cast<QComboBox*>(w);
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

bool SettingsUi::isSettingsWidget(QObject* obj)
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

void SettingsUi::setColor()
{
    QPushButton* b = static_cast<QPushButton*>(sender());
    QColor color = QColorDialog::getColor(mPrimaryColor, this, tr("Select Color"));

    if (color.isValid()) {
        if(b->objectName() == "primaryColorBttn") {
            ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
            mPrimaryColor = color;
        } else if(b->objectName() == "alternateColorBttn") {
            ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
            mAlternateColor = color;
        } else if(b->objectName() == "dotColorBttn") {
            ui->dotColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
            mDotColor = color;
        }
    }
}

void SettingsUi::setupDialogWidgets()
{
    //Application
    //TODO: use auto completer to help fill in the default file location field.
    connect(ui->folderSelector, SIGNAL(clicked()), SLOT(selectFolder()));
    
    //Charts
    connect(ui->primaryColorBttn, SIGNAL(clicked()), SLOT(setColor()));
    connect(ui->alternateColorBttn, SIGNAL(clicked()), SLOT(setColor()));
    connect(ui->dotColorBttn, SIGNAL(clicked()), SLOT(setColor()));
    
    ui->defaultStitch->addItems(StitchLibrary::inst()->stitchList());
    
    QString priColor = Settings::inst()->value("stitchPrimaryColor").toString();
    ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(priColor), QSize(32, 32))));
    mPrimaryColor = priColor;
    
    QString altColor = Settings::inst()->value("stitchAlternateColor").toString();
    ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(altColor), QSize(32, 32))));
    mAlternateColor = altColor;

    QString dotColor = Settings::inst()->value("chartIndicatorColor").toString();
    ui->dotColorBttn->setIcon(QIcon(drawColorBox(QColor(dotColor), QSize(32, 32))));
    mDotColor = dotColor;
    
    ui->placeholder->addItems(StitchLibrary::inst()->stitchList());
        
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
    Settings::inst()->setValue("chartIndicatorColor", QVariant(mDotColor.name()));

    StitchLibrary::inst()->reloadAllStitchIcons();
    
    //Instructions
    Settings::inst()->setValue("syntaxColor", QVariant(mKeywordColor.name()));
    
    //Legends
}

void SettingsUi::resetDialogWidgets()
{
    //Application
    
    //Charts
    QString color = Settings::inst()->defaultValue("stitchPrimaryColor").toString();
    ui->primaryColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
    mPrimaryColor = color;
    
    color = Settings::inst()->defaultValue("stitchAlternateColor").toString();
    ui->alternateColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
    mAlternateColor = color;

    color = Settings::inst()->defaultValue("chartIndicatorColor").toString();
    ui->dotColorBttn->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
    mDotColor = color;
        
    //Legends
}

/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "licensewizard.h"

#include <QApplication>

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QTextEdit>

#include <QVBoxLayout>

#include <QMessageBox>
#include <QRegExpValidator>

#include <QFile>

#include <QPrintDialog>
#include <QPrinter>

#include "appinfo.h" // email RegEx & licensePage
#include "licensevalidator.h"

#include "settings.h"

#include <QDebug>


/*****************************************************\
 * License Wizard
 *****************************************************/
LicenseWizard::LicenseWizard(bool regOnly, QWidget *parent)
    : QWizard(parent)
{

    setPage(Page_Intro, new IntroPage(regOnly));
    setPage(Page_Evaluate, new EvaluatePage);
    setPage(Page_Register, new RegisterPage);
    setPage(Page_Conclusion, new ConclusionPage);

    setStartId(Page_Intro);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif
    setOption(HaveHelpButton, true);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo.png"));

    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));

    setWindowTitle(tr("License Wizard"));
}

void LicenseWizard::showHelp()
{
    static QString lastHelpMessage;

    QString message;

    switch (currentId()) {
        case Page_Intro:
            message = tr("You may evaluate the software before you purchase a serial number and register it.");
            break;
        case Page_Evaluate:
            message = tr("Your name and a valid email address will be used to generate a demo license for your use in evaluating this software.");
            break;
        case Page_Register:
            message = tr("If you do not provide a valid email and serial number you will not be able to register this software."
                "The information you provide here will be used to generate a license to run the software.");
            break;
        case Page_Conclusion:
            message = tr("You must accept the terms and conditions of the license to if you wish to use the software.");
            break;
        default:
            message = tr("There is no help available for this page. Please contact %1 by using the form at %2")
                        .arg(AppInfo::inst()->appOrg).arg(AppInfo::inst()->appOrgContact);
    }

    QMessageBox::information(this, tr("License Wizard Help"), message);
    lastHelpMessage = message;
}

/*****************************************************\
 * Intro Page
 *****************************************************/
IntroPage::IntroPage(bool regOnly, QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/wizard_watermark.png"));

    topLabel = new QLabel(tr("This wizard will walk you through the registration of <i>%1</i>. "
                "This licensing wizard requires you to be connected to the internet. "
                ).arg(qApp->applicationName()));
    topLabel->setWordWrap(true);

    registerRadioButton = new QRadioButton(tr("&Register your copy"));
    evaluateRadioButton = new QRadioButton(tr("&Evaluate this software"));
    registerRadioButton->setChecked(true);

    evaluateRadioButton->setEnabled(!regOnly);
    evaluateRadioButton->setVisible(!regOnly);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(registerRadioButton);
    layout->addWidget(evaluateRadioButton);
    setLayout(layout);
}

int IntroPage::nextId() const
{
    if (evaluateRadioButton->isChecked()) {
        return LicenseWizard::Page_Evaluate;
    } else {
        return LicenseWizard::Page_Register;
    }
}

/*****************************************************\
 * Evaluate Page
 *****************************************************/
EvaluatePage::EvaluatePage(QWidget *parent)
    : QWizardPage(parent), mLicHttp(0), mDownloadFile(false)
{
    if(!mLicHttp) {
        mLicHttp = new LicenseHttp(this);
        connect(mLicHttp, SIGNAL(licenseCompleted(QString,bool)), SLOT(getLicense(QString,bool)));
    }
    
    mAllowNextPage = false;

    setTitle(tr("Evaluate <i>%1</i>").arg(qApp->applicationName()));
    setSubTitle(tr("Please fill all fields. Make sure to provide a valid "
                   "email address (e.g., john.smith@example.com)."));

    firstNameLabel = new QLabel(tr("&First Name:"));
    firstNameLineEdit = new QLineEdit(this);
    firstNameLabel->setBuddy(firstNameLineEdit);

    lastNameLabel = new QLabel(tr("&Last Name:"));
    lastNameLineEdit = new QLineEdit(this);
    lastNameLabel->setBuddy(lastNameLineEdit);

    emailLabel = new QLabel(tr("&Email address:"));
    emailLineEdit = new QLineEdit(this);
    emailLineEdit->setValidator(new QRegExpValidator(AppInfo::inst()->emailRegExp, this));
    emailLabel->setBuddy(emailLineEdit);

    licenseNumberLineEdit = new QLineEdit(this);
    licenseNumberLineEdit->setVisible(false);

    registerField("evaluate.first_name*", firstNameLineEdit);
    registerField("evaluate.last_name*", lastNameLineEdit);
    registerField("evaluate.email*", emailLineEdit);
    registerField("evaluate.license", licenseNumberLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(firstNameLabel, 0, 0);
    layout->addWidget(firstNameLineEdit, 0, 1);
    layout->addWidget(lastNameLabel, 1, 0);
    layout->addWidget(lastNameLineEdit, 1, 1);
    layout->addWidget(emailLabel, 2, 0);
    layout->addWidget(emailLineEdit, 2, 1);
    setLayout(layout);
}

int EvaluatePage::nextId() const
{
    return LicenseWizard::Page_Conclusion;
}

/**
 * validatePage()
 * Some Notes: The first time this function is called it spawns a thread that checks the server
 * for a license and returns before the thread has completed so we don't allow movement to the
 * next page.
 * Once the thread returns it calls getLicense and wizard()->next(); where the validatePage function
 * is called again. To prevent a second thread from trying to get the license I've used mDownloadFile.
 */
bool EvaluatePage::validatePage()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString path = Settings::inst()->value("licensePage").toString();
    path = QString(path).arg("").arg(emailLineEdit->text()).arg(firstNameLineEdit->text()).arg(lastNameLineEdit->text());
    QUrl url(path);

    if(mDownloadFile)
        mDownloadFile = false;
    else {
        mLicHttp->downloadFile(url);
        mDownloadFile = true;
    }
    
    QApplication::restoreOverrideCursor();
    return mAllowNextPage;
}

void EvaluatePage::getLicense(QString license, bool errors)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if(errors) {
        mAllowNextPage = false;
        QMessageBox::information(this, "Error", tr("%1 was unable to register with the server."
                            "If this problem persists please contact %1 by using the form at %2")
                            .arg(AppInfo::inst()->appOrg).arg(AppInfo::inst()->appOrgContact));
    }

    setField("evaluate.license", QVariant(license));
    mAllowNextPage = true;
    wizard()->next();
    QApplication::restoreOverrideCursor();
}

/*****************************************************\
 * Register Page
 *****************************************************/
RegisterPage::RegisterPage(QWidget *parent)
    : QWizardPage(parent), mLicHttp(0), mDownloadFile(false)
{
    if(!mLicHttp) {
        mLicHttp = new LicenseHttp(this);
        connect(mLicHttp, SIGNAL(licenseCompleted(QString,bool)), SLOT(getLicense(QString,bool)));
    }
    
    mAllowNextPage = false;

    setTitle(tr("Register Your Copy of <i>%1</i>&trade;").arg(qApp->applicationName()));
    setSubTitle(tr("Please fill in all fields. The Serial Number should use the form XXXX-XXX-XXXX and it should include the dashes."));

    firstNameLabel = new QLabel(tr("&First Name:"));
    firstNameLineEdit = new QLineEdit(this);
    firstNameLabel->setBuddy(firstNameLineEdit);

    lastNameLabel = new QLabel(tr("&Last Name:"));
    lastNameLineEdit = new QLineEdit(this);
    lastNameLabel->setBuddy(lastNameLineEdit);

    emailLabel = new QLabel(tr("&Email:"));
    emailLineEdit = new QLineEdit(this);
    emailLabel->setBuddy(emailLineEdit);
    emailLineEdit->setValidator(new QRegExpValidator(AppInfo::inst()->emailRegExp, this));

    serialNumberLabel = new QLabel(tr("&Serial Number:"));
    serialNumberLineEdit = new QLineEdit(this);
    serialNumberLabel->setBuddy(serialNumberLineEdit);

    serialNumberLineEdit->setValidator(new LicenseValidator(this));

    licenseNumberLineEdit = new QLineEdit(this);
    licenseNumberLineEdit->setVisible(false);

    registerField("register.first_name*", firstNameLineEdit);
    registerField("register.last_name*", lastNameLineEdit);
    registerField("register.email*", emailLineEdit);
    registerField("register.serialNumber*", serialNumberLineEdit);
    registerField("register.license", licenseNumberLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(firstNameLabel, 0, 0);
    layout->addWidget(firstNameLineEdit, 0, 1);
    layout->addWidget(lastNameLabel, 1, 0);
    layout->addWidget(lastNameLineEdit, 1, 1);
    layout->addWidget(emailLabel, 2, 0);
    layout->addWidget(emailLineEdit, 2, 1);
    layout->addWidget(serialNumberLabel, 3, 0);
    layout->addWidget(serialNumberLineEdit, 3, 1);
    setLayout(layout);
}

void RegisterPage::initializePage()
{
    QString fName = Settings::inst()->value("firstName").toString();
    QString lName = Settings::inst()->value("lastName").toString();
    QString email = Settings::inst()->value("email").toString();

    firstNameLineEdit->setText(fName);
    lastNameLineEdit->setText(lName);
    emailLineEdit->setText(email);
}

/**
 * validatePage()
 * Some Notes: The first time this function is called it spawns a thread that checks the server
 * for a license and returns before the thread has completed so we don't allow movement to the
 * next page.
 * Once the thread returns it calls getLicense and wizard()->next(); where the validatePage function
 * is called again. To prevent a second thread from trying to get the license I've used mDownloadFile.
 */
bool RegisterPage::validatePage()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString path = Settings::inst()->value("licensePage").toString();
    path = QString(path).arg(serialNumberLineEdit->text()).arg(emailLineEdit->text()).arg(firstNameLineEdit->text()).arg(lastNameLineEdit->text());
    QUrl url(path);

    if(mDownloadFile)
        mDownloadFile = false;
    else {
        mLicHttp->downloadFile(url);
        mDownloadFile = true;
    }
    
    QApplication::restoreOverrideCursor();
    return mAllowNextPage;
}

void RegisterPage::getLicense(QString license, bool errors)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if(errors) {
        mAllowNextPage = false;
        //FIXME: consolidate this with the code from the eval page.
        QMessageBox::information(this, "Error", tr("%1 was unable to register with the server."
        "If this problem persists please contact %1 by using the form at %2")
        .arg(AppInfo::inst()->appOrg).arg(AppInfo::inst()->appOrgContact));
    }

    setField("register.license", QVariant(license));
    mAllowNextPage = true;
    this->wizard()->next();
    QApplication::restoreOverrideCursor();
}

int RegisterPage::nextId() const
{
    return LicenseWizard::Page_Conclusion;
}

/*****************************************************\
 *Conclusion Page
 *****************************************************/
ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Complete Your Registration"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/wizard_watermark.png"));
    setFinalPage(true);
    
    licenseEdit = new QTextEdit;

    agreeCheckBox = new QCheckBox(tr("I agree to the terms of the license"));

    registerField("conclusion.agree*", agreeCheckBox);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(tr("An internet connection is no longer required to run this software.")));
    layout->addWidget(licenseEdit);
    layout->addWidget(agreeCheckBox);
    setLayout(layout);
}

int ConclusionPage::nextId() const
{
    return -1;
}

void ConclusionPage::initializePage()
{
    QFile f(":/resources/license.txt");
    if(!f.open(QIODevice::ReadOnly))
        return;
    QString licenseText = f.readAll();
    
    licenseEdit->setText(licenseText);
    licenseEdit->setReadOnly(true);

    if (wizard()->hasVisitedPage(LicenseWizard::Page_Register)) {
        sn      = field("register.serialNumber").toString();
        license = field("register.license").toString();
        fname   = field("register.first_name").toString();
        lname   = field("register.last_name").toString();
        email   = field("register.email").toString();
    } else if (wizard()->hasVisitedPage(LicenseWizard::Page_Evaluate)) {
        license = field("evaluate.license").toString();
        fname   = field("evaluate.first_name").toString();
        lname   = field("evaluate.last_name").toString();
        email   = field("evaluate.email").toString();
    }
    //else do an upgrade.
    
}

bool ConclusionPage::validatePage()
{
    bool isValid = QWizardPage::validatePage();

    if(isValid) {
        Settings::inst()->setValue("firstName", QVariant(fname));
        Settings::inst()->setValue("lastName", QVariant(lname));
        Settings::inst()->setValue("email", QVariant(email));
        Settings::inst()->setValue("serialNumber", QVariant(sn));
        Settings::inst()->setValue("license", QVariant(license));
    }

    return isValid;
}

void ConclusionPage::setVisible(bool visible)
{
    QWizardPage::setVisible(visible);

    if (visible) {
        wizard()->setButtonText(QWizard::CustomButton1, tr("&Print"));
        wizard()->setOption(QWizard::HaveCustomButton1, true);
        connect(wizard(), SIGNAL(customButtonClicked(int)),
                this, SLOT(printButtonClicked()));
    } else {
        wizard()->setOption(QWizard::HaveCustomButton1, false);
        disconnect(wizard(), SIGNAL(customButtonClicked(int)),
                   this, SLOT(printButtonClicked()));
    }
}

void ConclusionPage::printButtonClicked()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec())
        licenseEdit->print(&printer);
}

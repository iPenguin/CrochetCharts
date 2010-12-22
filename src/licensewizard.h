#ifndef LICENSEWIZARD_H
#define LICENSEWIZARD_H

#include <QWizard>

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QTextEdit;

class LicenseWizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Intro, Page_Evaluate, Page_Register, Page_Conclusion };

    LicenseWizard(QWidget *parent = 0);

private slots:
    void showHelp();
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *topLabel;
    QRadioButton *registerRadioButton;
    QRadioButton *evaluateRadioButton;
};

class EvaluatePage : public QWizardPage
{
    Q_OBJECT

public:
    EvaluatePage(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *nameLabel;
    QLabel *emailLabel;
    QLineEdit *nameLineEdit;
    QLineEdit *emailLineEdit;
};

class RegisterPage : public QWizardPage
{
    Q_OBJECT

public:
    RegisterPage(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *nameLabel;
    QLabel *serialNumberLabel;
    QLabel *emailLabel;
    QLineEdit *nameLineEdit;
    QLineEdit *serialNumberLineEdit;
    QLineEdit *emailLineEdit;
};

class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = 0);

    void initializePage();
    int nextId() const;
    void setVisible(bool visible);
    void cleanupPage();

private slots:
    void printButtonClicked();

private:
    QTextEdit *licenseEdit;
    QCheckBox *agreeCheckBox;
};

#endif // LICENSEWIZARD_H

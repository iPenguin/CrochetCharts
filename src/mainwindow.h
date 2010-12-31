#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
    class StitchLibraryDialog;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileExport();

    void menuViewAboutToShow();
    void viewShowStitches();

    void documentNewChart();

    void toolsOptions();
    void toolsRegisterSoftware();
    void toolsStitchLibrary();

    void helpAbout();

private:
    void readSettings();
    void trialVersionMessage();

    Ui::MainWindow *ui;
    Ui::StitchLibraryDialog *sld;

    void setupMenus();
};

#endif // MAINWINDOW_H

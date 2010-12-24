#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
    class ExportDialog;
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

    void helpAbout();

    void toolsOptions();

private:
    void readSettings();

    Ui::MainWindow *ui;
    Ui::ExportDialog *expDialog;

    void setupMenus();
};

#endif // MAINWINDOW_H

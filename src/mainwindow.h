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

private slots:
    void fileExport();

    void documentNewChart();

    void about();

private:
    Ui::MainWindow *ui;
    Ui::ExportDialog *expDialog;

    void setupMenus();
};

#endif // MAINWINDOW_H

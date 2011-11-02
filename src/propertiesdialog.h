#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDockWidget>
#include <QTabWidget>
#include "scene.h"

#include "debug.h"

namespace Ui {
    class PropertiesDialog;
}

class PropertiesDialog : public QDockWidget
{
    Q_OBJECT

public:

    enum UiSelection {
        SceneUi = 10,
        CellUi = 15,
        CenterUi = 20,
        
        MixedUi = 100
    };
    
    PropertiesDialog(QTabWidget* tabWidget, QWidget *parent = 0);
    ~PropertiesDialog();

    
private slots:
    void tabChanged(int tabNumber);
    void updateDialogUi();

private:
    void showUi(UiSelection selection);

    void clearUi();
    
private:
    Ui::PropertiesDialog *ui;

    QTabWidget* mTabWidget;
    Scene* mScene;
};

#endif // PROPERTIESDIALOG_H

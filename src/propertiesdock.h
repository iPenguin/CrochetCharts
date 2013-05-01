#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include <QDockWidget>
#include <QTabWidget>
#include "scene.h"

#include "debug.h"

namespace Ui {
    class PropertiesDock;
}

class PropertiesDock : public QDockWidget
{
    Q_OBJECT

public:

    enum UiSelection {
        SceneUi = 10,
        CellUi = 15,
        CenterUi = 20,
        
        MixedUi = 100
    };
    
    PropertiesDock(QTabWidget* tabWidget, QWidget *parent = 0);
    ~PropertiesDock();

signals:
    void propertiesUpdated(QString property, QVariant newValue);
    
private slots:
    void tabChanged(int tabNumber);
    void updateDialogUi();

    void chartUpdateChartCenter(bool state);
    void chartUpdateGuidelines(QString guides);

    void cellUpdateAngle(double angle);
    void cellUpdateScaleX(double scale);
    void cellUpdateScaleY(double scale);
    void cellUpdateStitch(QString stitch);
    void cellDeleteItems();

    void updateGuidelinesUi();

    void updateRows(int rows);
    void updateColumns(int columns);
    void updateCellWidth(int width);
    void updateCellHeight(int height);

private:
    void showUi(UiSelection selection);
    void clearUi();

    void setupStitchCombo();

    void updateGuidelines();

private:
    Ui::PropertiesDock *ui;

    QTabWidget* mTabWidget;
    Scene* mScene;
    Grid mGuidelines;
};

#endif // PROPERTIESDOCK_H

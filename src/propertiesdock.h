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
        IndicatorUi = 25,
        MixedUi = 100
    };

    PropertiesDock(QTabWidget* tabWidget, QWidget *parent = 0);
    ~PropertiesDock();

    void loadProperties(Guidelines guidelines);

    bool closing;

signals:
    void propertiesUpdated(QString property, QVariant newValue);

private slots:
    void tabChanged(int tabNumber);
    void updateDialogUi();

    void chartUpdateChartCenter(bool state);
    void chartUpdateGuidelines();

    void cellUpdateAngle(double angle);
    void cellUpdateScaleX(double scale);
    void cellUpdateScaleY(double scale);
    void cellUpdateStitch(QString stitch);
    void cellDeleteItems();

    void updateGuidelinesUi();
    void indicatorUpdate();

private:
    void showUi(UiSelection selection);
    void clearUi();

    void setupStitchCombo();

    bool updateGuidelines();

private:
    Ui::PropertiesDock *ui;

    QTabWidget* mTabWidget;
    Scene* mScene;
    Guidelines mGuidelines;
};

#endif // PROPERTIESDOCK_H

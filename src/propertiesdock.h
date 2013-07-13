#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include <QDockWidget>
#include <QTabWidget>
#include "scene.h"

#include "debug.h"

namespace Ui {
    class PropertiesDock;
}

class StitchProperties;

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

public slots:
    void propertyUpdated();

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

    void cellUpdateFgColor();
    void cellUpdateBgColor();

    void updateGuidelinesUi();
    void indicatorUpdate();

private:
    /**
     * showUi - show the user interface based on the selection.
     * Options are show based on their prefix.
     * gen_ = general, st_ = stitch, ind_ = indicator.
     */
    void showUi(PropertiesDock::UiSelection selection, int count = 0);

    void showSingleCell();
    void showMultiCell();
    void showSingleIndicator();
    void showMultiIndicator();
    void showMixedObjects();
    void showCanvas();

    void clearUi();

    void setupStitchCombo();

    bool updateGuidelines();

    /**
     * @brief selectionProperties
     * @return return the properties for the selection of stitches.
     */
    StitchProperties selectionProperties();

private:
    Ui::PropertiesDock *ui;

    QTabWidget* mTabWidget;
    Scene* mScene;
    Guidelines mGuidelines;
};

#endif // PROPERTIESDOCK_H

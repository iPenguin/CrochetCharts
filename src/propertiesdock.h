#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include <QDockWidget>
#include <QTabWidget>
#include "scene.h"

#include "debug.h"

namespace Ui {
    class PropertiesDock;
}

struct CellProperties {

    QString stitch;
    QColor color;
    QColor bgColor;
    QPointF scale;
    qreal angle;
};

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

signals:
    void propertyUpdated(QString property, QVariant value);

public slots:
    void updatePropertiesUi();
    void selectionChanged();

private:
    void cellUpdateFgColor();
    void cellUpdateBgColor();

private slots:
    void tabChanged(int tabNumber);

//private cell property functions.
private:
    /**
     * stitchValue - returns the value that should be used for display.
     * if there isn't a common value it returns "".
     */
    QString stitchValue(QString first, QString second);

    /**
     * colorValue - returns the value that should be used for display.
     * if there isn't a common value it returns an invalid color.
     */
    QColor colorValue(QColor first, QColor second);

    /**
     * scaleValue - returns the value that should be used for display.
     * if there isn't a common value it returns QPointF(0,0).
     */
    QPointF scaleValue(QPointF first, QPointF second);

    /**
     * angleValue - returns the value that should be used for display.
     * if there isn't a common value it returns -1;
     */
    qreal angleValue(qreal first, qreal second);

    void setupStitchCombo();

    UiSelection chooseUi();

private:
    Ui::PropertiesDock *ui;

    QTabWidget* mTabWidget;
    QPointer<Scene> mScene;
    Guidelines mGuidelines;
    CellProperties mCellProps;
};

#endif // PROPERTIESDOCK_H

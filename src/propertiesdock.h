/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#ifndef PROPERTIESDOCK_H
#define PROPERTIESDOCK_H

#include <QDockWidget>
#include <QTabWidget>
#include "scene.h"

#include "debug.h"

namespace Ui {
    class PropertiesDock;
}

class PropertiesData;

class PropertiesDock : public QDockWidget
{
    Q_OBJECT

public:

    enum UiSelection {
        SceneUi = 10,
        CellUi = 15,
        ItemGroupUi = 16,
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

	void chartUpdateAlignAngle(bool state);
    void chartUpdateChartCenter(bool state);
    void chartUpdateGuidelines();

    void cellUpdateAngle(double angle);
    void cellUpdatePositionX(double positionX);
    void cellUpdatePositionY(double positionY);
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
    void showItemGroup();

    void clearUi();

    void setupStitchCombo();

    bool updateGuidelines();

    /**
     * @brief selectionProperties
     * @return return the properties for the selection of stitches.
     */
    PropertiesData selectionProperties();

private:
    Ui::PropertiesDock *ui;

    QTabWidget* mTabWidget;
    Scene* mScene;
    Guidelines mGuidelines;
};

#endif // PROPERTIESDOCK_H

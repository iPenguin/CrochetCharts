/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
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
#ifndef ALIGNDOCK_H
#define ALIGNDOCK_H

#include <QDockWidget>

namespace Ui {
    class AlignDock;
}

class AlignDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit AlignDock(QWidget *parent = 0);
    ~AlignDock();

signals:
    /**
     * alignment:
     * 1 = left, 2 = center v, 3 = right
     * 4 = top,  5 = center h, 6 = bottom
     */
    void align(int alignment);

    /**
     * distribution:
     * 1 = left, 2 = center h, 3 = right
     * 4 = top,  5 = center v, 6 = bottom
     */
    void distribute(int distribution);

private slots:
    void generateAlignment();
    void generateDistribution();
    
private:
    Ui::AlignDock *ui;

    
};

#endif // ALIGNDOCK_H

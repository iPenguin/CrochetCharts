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
#ifndef TESTSTITCHSET_H
#define TESTSTITCHSET_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../src/stitchset.h"

class TestStitchSet : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void setupStitchSet();
    void findStitch();
    void findStitch_data();
    void saveLoadDataSet();
    void cleanupTestCase();

private:
    StitchSet* mSet;
};

#endif // TESTSTITCHSET_H

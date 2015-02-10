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
#include "testsettings.h"
#include "teststitch.h"
#include "teststitchset.h"
#include "testcell.h"
#include "testtextview.h"
#include "teststitchlibrary.h"

int main(int argc, char** argv) 
{

    QApplication app(argc, argv);
    int retval(0);

    QObject* test;
    qDebug() << "All output files are in the build directory";

    //dep: license (initDemo)
    test = new TestSettings();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestStitch();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestStitchSet();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestStitchLibrary();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestCell();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestTextView();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;
    
    return (retval ? 1 : 0);
}

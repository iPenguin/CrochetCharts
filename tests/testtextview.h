/*************************************************\
| Copyright (c) 2013 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TESTTEXTVIEW_H
#define TESTTEXTVIEW_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "scene.h"

#include "textview.h"

class TestTextView : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testGenerateText();
    void testGenerateTextRow();
    void testGenerateTextRow_data();
    void testCopyInstructions();
    void testCopyInstructions_data();
    void cleanupTestCase();
    
private:
    Scene *mScene;
    TextView *mTv;
    
    QList< QList<Cell *> > createGrid();
    QList< QStringList > mTextGrid;
};

#endif // TESTTEXTVIEW_H

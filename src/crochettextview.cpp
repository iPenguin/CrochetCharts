/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochettextview.h"

#include <QDebug>

CrochetTextView::CrochetTextView(QWidget *parent, CrochetScene* scene)
    : QPlainTextEdit(parent), mScene(scene)
{

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateScene()));
    //connect(this, SIGNAL(selectionChanged()), this, SLOT(updateScene()));
   //Update on every key stroke:
   //connect(this, SIGNAL(textChanged()), this, SLOT(updateScene()));
   
}

CrochetTextView::~CrochetTextView()
{
    
}

void CrochetTextView::generateRows()
{
    
}

QString CrochetTextView::generateRow(int i)
{
    

    return QString();
}


void CrochetTextView::updateScene()
{
    
    qDebug() << "update Scene";
    
    
    
}
/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETTEXTVIEW_H
#define CROCHETTEXTVIEW_H

#include "crochetscene.h"

#include <QPlainTextEdit>

class CrochetTextView : public QPlainTextEdit
{
    Q_OBJECT
    
public:
    CrochetTextView(QWidget *parent = 0, CrochetScene* scene = 0);
    ~CrochetTextView();

    void setScene(CrochetScene* scene) { mScene = scene; }

    void generateRows();
    QString generateRow(int i);
    
protected:

private slots:
    void updateScene();
    
private:
        
    CrochetScene* mScene;
};
#endif //CROCHETTEXTVIEW_H
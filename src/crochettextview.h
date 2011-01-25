/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETTEXTVIEW_H
#define CROCHETTEXTVIEW_H

#include "crochetscene.h"
#include "crochethighlighter.h"

#include <QPlainTextEdit>
#include <QCompleter>

class CrochetTextView : public QPlainTextEdit
{
    Q_OBJECT
    
public:
    CrochetTextView(QWidget *parent = 0, CrochetScene* scene = 0);
    ~CrochetTextView();

    void setScene(CrochetScene* scene) { mScene = scene; }

    void displayRows();
    QString displayRow(int row);

    void setCompleter(QCompleter *c);
    
protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

private slots:
    void insertCompletion(const QString &completion);

    void updateRow(int row);
    void updateScene();
private:
    QString textUnderCursor() const;
    
private:
    CrochetHighlighter *mHighlighter;
    CrochetScene* mScene;
    QCompleter* mCompleter;
};
#endif //CROCHETTEXTVIEW_H
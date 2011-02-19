/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETTEXTVIEW_H
#define CROCHETTEXTVIEW_H

#include "crochetscene.h"
#include "crochethighlighter.h"

#include <QPlainTextEdit>
#include <QCompleter>
#include <QWidget>

class CrochetTextView : public QPlainTextEdit
{
    Q_OBJECT
    
public:
    CrochetTextView(QWidget *parent = 0, CrochetScene* scene = 0);
    ~CrochetTextView();

    void setScene(CrochetScene* scene) { mScene = scene; }

    void setCompleter(QCompleter *c);

    //return a cursor for the start of the block containing pos.
    QTextCursor cursorAtBlockStart(int pos);
    QTextCursor cursorAtRowStart(int row);

    QString copyInstructions();
    
protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

private slots:
    void insertCompletion(const QString &completion);

    void updateRow(int row);
    void updateScene(int pos, int charsRemoved, int charsAdded);
    void addRow(int newRow);
    void createChart(int rows, int cols);
    
private:
    QString textUnderCursor() const;

    /**
     * Generates the text for a given row of the chart without trailing \n.
     * If cleanOutput = true remove the placeholder stitches 'x' by default.
     **/
    QString generateTextRow(int row, bool cleanOutput = false);
    //returns an ordered list of the stitches found in the text.
    QStringList parseTextRow(QString text);

    /**
     * This function strips off any incomplete repeat indicators or other
     * special text input from the user that might be incomplete.
     */
    QString cleanToken(QString token);
    
private:
    CrochetHighlighter *mHighlighter;
    CrochetScene* mScene;
    QCompleter* mCompleter;

/**
 * Line number area.
 */
public:
   
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    
protected:
    void resizeEvent(QResizeEvent *event);
    
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    void setupLineNumberArea();
private:
    QWidget *lineNumberArea;
    QString lineNumberText;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CrochetTextView *view) : QWidget(view) {
        textView = view;
    }
    
    QSize sizeHint() const {
        return QSize(textView->lineNumberAreaWidth(), 0);
    }
    
protected:
    void paintEvent(QPaintEvent *event) {
        textView->lineNumberAreaPaintEvent(event);
    }
    
private:
    CrochetTextView *textView;
};

#endif //CROCHETTEXTVIEW_H
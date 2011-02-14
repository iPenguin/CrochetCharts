/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochettextview.h"

#include <QDebug>
#include <qstringlistmodel.h>

#include <QScrollBar>
#include <QPainter>

#include "stitchcollection.h"

CrochetTextView::CrochetTextView(QWidget *parent, CrochetScene* scene)
    : QPlainTextEdit(parent), mScene(scene), mCompleter(0)
{
    mHighlighter = new CrochetHighlighter(document());
    mCompleter = new QCompleter(this);
    setCompleter(mCompleter);
    
    document()->setMaximumBlockCount(mScene->rowCount());
    
    //update this text view
    connect(mScene, SIGNAL(rowChanged(int)), this, SLOT(updateRow(int)));
    //connect(mScene, SIGNAL(rowAdded(int)), this, SLOT(addRow(int)));
    connect(mScene, SIGNAL(chartCreated(int,int)), this, SLOT(createChart(int,int)));
    //update the data model.
    connect(document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(updateScene(int,int,int)));

    setupLineNumberArea();
}

CrochetTextView::~CrochetTextView()
{
}

void CrochetTextView::updateRow(int row)
{
    QString rowText;
    QTextCursor curs = cursorAtBlockStart(row);
    
    int cols = mScene->columnCount(row);
    
    for(int c = 0; c < cols; ++c) {
        rowText += mScene->cell(row, c)->name() + ", ";
    }

}

void CrochetTextView::updateScene(int pos, int charsRemoved, int charsAdded)
{
    Q_UNUSED(charsRemoved);
    Q_UNUSED(charsAdded);
    
    QTextCursor curs = cursorAtBlockStart(pos);

    int row = curs.blockNumber();
    //TODO: add repeat parsing (ie "\[.*\] 3 times" and "\*.*; repeat from \*" )
    //TODO: add color parsing (ie ch3 in C1) save for version 1.x...
    QStringList stitches, tokens, stitchList;
    stitchList = StitchCollection::inst()->stitchList();
    tokens = curs.block().text().split(",");

    //TODO: break out the parser into a seperate function.
    foreach(QString token, tokens) {
        token = token.simplified().toLower();
        
        QString st;
        foreach(QString temp, stitchList) {
            QRegExp re = QRegExp("^" + temp + "\\s{,}[0-9]{,}$");
            re.setMinimal(true);
            if(token.contains(re)) {
                st = temp;
                break;
            }
        }
        
        token = token.simplified().remove(0, st.length());
        int count = token.toInt();
        if(count <= 0) count = 1;
        for(int i = 0; i < count; ++i) {
            stitches.append(st);
        }
    }

    int cols = mScene->columnCount(row);

    int stitchCount = stitches.count();

    if(stitchCount < cols) {
        for(int i = stitchCount; i < cols; ++i)
            mScene->removeCell(row, i);
    }

    for(int c = 0; c < stitchCount; ++c) {
        QString s = stitches.at(c);
        s = s.simplified().toLower();
        Cell* cell = mScene->cell(row, c);
        if(!cell) {
            cell = new CrochetCell();
            mScene->appendCell(row, cell);
        }
        mScene->cell(row, c)->setStitch(s);
    }
}

void CrochetTextView::createChart(int rows, int cols)
{
    QTextCursor curs = QTextCursor(textCursor());
    curs.movePosition(QTextCursor::Start);
    for(int i = 0; i < rows; ++i) {
        curs.movePosition(QTextCursor::NextBlock);
   
        QString rowText;

        QString curStitch, previousStitch, nextStitch;
        int count = 1;
        bool firstPass = true;

        for(int c = 0; c < cols; ++c) {
            curStitch = mScene->cell(i, c)->name();
            Cell *nextCell = mScene->cell(i, c+1);
            if(nextCell)
                nextStitch = nextCell->name();
            else
                nextStitch = "";

            if(!firstPass) rowText += ", ";

            if(curStitch != previousStitch)
                rowText += curStitch;
            if(curStitch == previousStitch)
                count++;
            if(curStitch != nextStitch) {
                rowText += QString::number(count);
                count = 1;
            }

            previousStitch = curStitch;
        }

        if(i < rows -1)
            rowText.append('\n');
        curs.insertText(rowText);
    }    
}

void CrochetTextView::addRow(int newRow)
{
    QTextCursor curs = QTextCursor(textCursor());
    curs.movePosition(QTextCursor::Start);
    for(int i = 0; i < newRow; ++i) {
        curs.movePosition(QTextCursor::NextBlock);
    }

    QString rowText;
    
    int cols = mScene->columnCount(newRow);

    QString curStitch, previousStitch, nextStitch;
    int count = 1;
    bool firstPass = true;
    
    for(int c = 0; c < cols; ++c) {
        curStitch = mScene->cell(newRow, c)->name();
        Cell *nextCell = mScene->cell(newRow, c+1);
        if(nextCell)
            nextStitch = nextCell->name();
        else
            nextStitch = "";

        if(!firstPass) rowText += ", ";
        
        if(curStitch != previousStitch)
            rowText += curStitch;
        if(curStitch == previousStitch)
            count++;
        if(curStitch != nextStitch) {
            rowText += QString::number(count);
            count = 1;
        }

        previousStitch = curStitch;
    }

    rowText.append('\n');
    curs.insertText(rowText);
}

QTextCursor CrochetTextView::cursorAtBlockStart(int pos)
{
    QTextCursor curs = QTextCursor(textCursor());
    curs.setPosition(pos);
    curs.movePosition(QTextCursor::StartOfBlock);
    
    return curs;
}

/************************************************************************
 * Text Completer functions
 ************************************************************************/
void CrochetTextView::insertCompletion(const QString& completion)
{
    if (mCompleter->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - mCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

void CrochetTextView::focusInEvent(QFocusEvent *e)
{
    if (mCompleter)
        mCompleter->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

QString CrochetTextView::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CrochetTextView::setCompleter(QCompleter *c)
{
    if (mCompleter)
        QObject::disconnect(mCompleter, 0, this, 0);
    
    mCompleter = c;
    
    if (!mCompleter)
        return;
    
    mCompleter->setWidget(this);

    QStringList words = mHighlighter->keywords();
    words.sort();
    mCompleter->setModel(new QStringListModel(words, mCompleter));
    
    mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(mCompleter, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

void CrochetTextView::keyPressEvent(QKeyEvent *e)
{
    if (mCompleter && mCompleter->popup()->isVisible()) {
        // The following keys are forwarded by the mCompleter to the widget
        switch (e->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                e->ignore();
                return; // let the mCompleter do default behavior
            default:
                break;
        }
    }

    switch(e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            e->ignore(); //eat enter/return.
            return;
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
            QPlainTextEdit::keyPressEvent(e); //accept default behavior for arrows.
            return;
        default:
            break;
    }
    
    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!mCompleter || !isShortcut) // do not process the shortcut when we have a mCompleter
         QPlainTextEdit::keyPressEvent(e);
    
    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!mCompleter || (ctrlOrShift && e->text().isEmpty()))
        return;

    QString completionPrefix = textUnderCursor();

    if (completionPrefix != mCompleter->completionPrefix()) {
        mCompleter->setCompletionPrefix(completionPrefix);
        mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(mCompleter->popup()->sizeHintForColumn(0) + mCompleter->popup()->verticalScrollBar()->sizeHint().width());
    if(cr.width() < 100) cr.setWidth(100); // min width of the popup.
    mCompleter->complete(cr); // popup it up!
}

/************************************************************************
 * Line Number Area functions
 ************************************************************************/
void CrochetTextView::setupLineNumberArea()
{
    lineNumberText = tr(" Row ");
    lineNumberArea = new LineNumberArea(this);
    
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(updateLineNumberArea(const QRect &, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CrochetTextView::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int lineTextWidth = fontMetrics().width(lineNumberText + " ");
    
    int space = (fontMetrics().width(QLatin1Char('9')) * digits) + lineTextWidth;
    
    return space;
}

void CrochetTextView::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CrochetTextView::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CrochetTextView::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CrochetTextView::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        
        QColor lineColor = QColor(Qt::lightGray).lighter(120);
        
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    
    setExtraSelections(extraSelections);
}

void CrochetTextView::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    
    
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString(lineNumberText + QString::number(blockNumber + 1) + " ");
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

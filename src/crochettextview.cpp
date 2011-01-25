/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochettextview.h"

#include <QDebug>
#include <qstringlistmodel.h>
#include <QScrollBar>
#include <qtextdocumentfragment.h>

CrochetTextView::CrochetTextView(QWidget *parent, CrochetScene* scene)
    : QPlainTextEdit(parent), mScene(scene), mCompleter(0)
{
    mHighlighter = new CrochetHighlighter(document());
    mCompleter = new QCompleter(this);
    setCompleter(mCompleter);
    
    document()->setMaximumBlockCount(mScene->rowCount());
    
    //update this text view
    connect(mScene, SIGNAL(rowChanged(int)), this, SLOT(updateRow(int)));
    connect(mScene, SIGNAL(rowAdded(int)), this, SLOT(addRow(int)));
    //update the data model.
    connect(document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(updateScene(int,int,int)));

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
    QTextCursor curs = cursorAtBlockStart(pos);

    int row = curs.blockNumber();
    //TODO: add complicated [] 3 times and * ; repeat from * parsing...
    // and make sure the parse puts each stitch in the stitches var seperately.
    QStringList stitches;
    stitches = curs.block().text().split(",");
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

void CrochetTextView::addRow(int newRow)
{
    QTextCursor curs = QTextCursor(textCursor());
    curs.movePosition(QTextCursor::Start);
    for(int i = 0; i < newRow; ++i) {
        curs.movePosition(QTextCursor::NextBlock);
    }
    
    QString rowText;
    
    int cols = mScene->columnCount(newRow);

    bool firstPass = true;
    
    for(int c = 0; c < cols; ++c) {
        if(!firstPass) rowText += ", ";
        rowText += mScene->cell(newRow, c)->name();
        firstPass = false;
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

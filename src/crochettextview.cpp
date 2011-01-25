/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochettextview.h"

#include <QDebug>
#include <qstringlistmodel.h>
#include <QScrollBar>

CrochetTextView::CrochetTextView(QWidget *parent, CrochetScene* scene)
    : QPlainTextEdit(parent), mScene(scene), mCompleter(0)
{

    mHighlighter = new CrochetHighlighter(document());
    mCompleter = new QCompleter(this);
    setCompleter(mCompleter);
    
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateScene()));
    connect(mScene, SIGNAL(rowChanged(int)), this, SLOT(updateRow(int)));
    //connect(this, SIGNAL(selectionChanged()), this, SLOT(updateScene()));
   //Update on every key stroke:
   //connect(this, SIGNAL(textChanged()), this, SLOT(updateScene()));
}

CrochetTextView::~CrochetTextView()
{
    
}

void CrochetTextView::displayRows()
{
    QString chart;
    int rows = mScene->rowCount();
   
    for(int r = 0; r < rows; ++r) {
        chart += displayRow(r) + "\n";
    }

    setPlainText(chart);
}

QString CrochetTextView::displayRow(int row)
{
    QString rowText;

    int cols = mScene->columnCount(row);

    for(int c = 0; c < cols; ++c) {
        rowText += mScene->cell(row, c)->name();
    }
    
    return rowText;
}

void CrochetTextView::updateRow(int row)
{
    QString rowText;
    
    int cols = mScene->columnCount(row);
    
    for(int c = 0; c < cols; ++c) {
        rowText += mScene->cell(row, c)->name() + ", ";
    }

    appendPlainText(rowText);
}

void CrochetTextView::updateScene()
{
    
    qDebug() << "update Scene";
        
}

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

/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochettextview.h"

#include <QDebug>
#include <qstringlistmodel.h>

#include <QScrollBar>
#include <QPainter>

#include "stitchlibrary.h"
#include "settings.h"
#include <math.h>

#include "crochetchartcommands.h"

CrochetTextView::CrochetTextView(QWidget *parent, CrochetScene* scene)
    : QPlainTextEdit(parent), mScene(scene), mCompleter(0)
{
    mHighlighter = new CrochetHighlighter(document());
    mCompleter = new QCompleter(this);
    setCompleter(mCompleter);

    connect(StitchLibrary::inst(), SIGNAL(stitchListChanged()), mHighlighter, SLOT(updateRules()));
    
    document()->setMaximumBlockCount(mScene->rowCount());
    
    //update this text view
    connect(mScene, SIGNAL(rowChanged(int)), SLOT(updateRow(int)));
    //connect(mScene, SIGNAL(rowAdded(int)), SLOT(addRow(int)));
    connect(mScene, SIGNAL(chartCreated(int,int)), SLOT(createChart(int,int)));
    //update the data model.
    connect(document(), SIGNAL(contentsChange(int,int,int)), SLOT(updateScene(int,int,int)));

    setupLineNumberArea();
}

CrochetTextView::~CrochetTextView()
{
}

void CrochetTextView::updateRow(int row)
{
    
    //FIXME: hackish? really what I want is don't update the row if we're editing the row in the textview.
    if(this->hasFocus())
        return;
    
    QString rowText;
    QTextCursor curs = cursorAtRowStart(row);
    
    rowText = generateTextRow(row);

    curs.movePosition(QTextCursor::StartOfBlock);
    curs.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if(!rowText.isEmpty()) {
        curs.insertText(rowText);
    } else {
        curs.deletePreviousChar();
    }
}

void CrochetTextView::updateScene(int pos, int charsRemoved, int charsAdded)
{
    Q_UNUSED(charsRemoved);
    Q_UNUSED(charsAdded);
    
    QTextCursor curs = cursorAtBlockStart(pos);

    int row = curs.blockNumber();
    QString rowText = curs.block().text();
    
    QStringList stitches = parseTextRow(rowText);

    int cols = mScene->columnCount(row);

    int stitchCount = stitches.count();

    if(stitchCount < cols) {
        for(int i = stitchCount; i < cols; ++i)
            mScene->removeCell(row, i);
    }

    mScene->undoStack()->beginMacro(tr("Update chart text"));
    for(int c = 0; c < stitchCount; ++c) {
        QString s = stitches.at(c);
        s = s.simplified().toLower();
        CrochetCell* cell = mScene->cell(row, c);
        
        if(!cell) {
            int x = mScene->grid()[row].count();
            mScene->undoStack()->push(new AddCell(mScene, QPoint(x,row)));
            cell = mScene->cell(row, c);
        }
        mScene->undoStack()->push(new SetCellStitch(mScene, cell, s));
    }
    mScene->undoStack()->endMacro();
}

QString CrochetTextView::generateTextRow(int row, bool cleanOutput, bool useRepeats)
{
    
    QString placeholder = Settings::inst()->value("placeholder").toString();
    QString rowText = "";
    QString curStitch;

    QStringList rowList;

    int cols = mScene->columnCount(row);

    //create a list of stitches
    for(int c = 0; c < cols; ++c) {
        CrochetCell *cell = mScene->cell(row, c);
        if(cell)
            curStitch = cell->name();
        else
            continue;
        
        if(cleanOutput) {
            //parse out any placeholder stitches before continuing.
            if(curStitch == placeholder)
                continue;
        }
        rowList.append(curStitch);
        curStitch = "";
    }
    rowText = generateText(rowList, useRepeats);

    if(rowText == "")
        return rowText;
    
    if(cleanOutput) {
        //capitalize the first letter.
        for(int i = 0; i < rowText.count(); ++i) {
            
            if(rowText.at(i).isLetter()) {
                rowText[i] = rowText.at(i).toUpper();
                break;
            }
        }

        rowText += ".";
    }

    return rowText;
}

QString CrochetTextView::generateText(QStringList row, bool useRepeats)
{
    QString text;
    QString curStitch, previousStitch, nextStitch;
    QMap<QString, QStringList> data;
    int count = 1;
    bool firstPass = true;
    
    bool genRepeats = Settings::inst()->value("generateTextRepeats").toBool();
    QString prefix = ".sws_";

    //FIXME: useRepeats is a hard coded solution to prevent the software from acting weird.
    if(genRepeats && useRepeats) {
        data = generateRepeats(row, prefix);
        row = data.value("row");
    }
    
    previousStitch = "";
    for(int i = 0; i < row.count(); ++i) {
        curStitch = row.value(i);
        nextStitch = row.value(i + 1);

        if(curStitch != previousStitch) {
            if(!firstPass) text += ", ";
            
            if(curStitch.startsWith(prefix)) {
                text += "[" + generateText(data.value(curStitch)) + "] ";
            } else
                text += curStitch;
        }
        if(curStitch == previousStitch)
            count++;
        if(curStitch != nextStitch) {
            text += QString::number(count);
            count = 1;
            if(curStitch.startsWith(prefix))
                text += " times";
        }
        
        previousStitch = curStitch;
        firstPass = false;
    }

    return text;
}

QMap< QString, QStringList > CrochetTextView::generateRepeats(QStringList stitches, QString prefix)
{
    QMap<QString, QStringList> data;
    QStringList row;
    int count = stitches.count();

    for(int i = 0; i < count; ++i) {
        QString value = stitches.value(i);
        for(int j = i + 2; j < count; ++j) {
            if(stitches.value(i) == stitches.value(j)) {
                int diff = j - i;
                int diffSts = false;
                for(int l = 0; l < diff; ++l) {
                    if(stitches.value(i + l) != stitches.value(i))
                        diffSts = true;
                }
                if(!diffSts)
                    continue;

                int count = matchCount(stitches, i, diff);

                if(count > 1) {
                    QStringList sub;
                    for(int k = 0; k < diff; ++k) 
                        sub.append(stitches.value(k));
                    for(int k = 0; k < count; ++k)
                        row.append(prefix + QString::number(i));
                    data.insert(prefix + QString::number(i), sub);
                    j += (diff * count);
                    i += (diff * count);
                }
            }
        }
        if(i < count)
            row.append(value);
    }

    data.insert("row", row);
    return data;
}

int CrochetTextView::matchCount(QStringList stitches, int startPos, int length)
{
    QStringList sub;
    int count = 0;

    for(int i = 0; i < length; ++i)
        sub.append(stitches.value(startPos + i));
    
    for(int i = startPos; i < stitches.count(); ++i) {

        if(sub.value(i%length) != stitches.value(i))
            break;
        count = floor(double(i + 1 - startPos)/length);
    }
    
    return count;
}

QStringList CrochetTextView::parseTextRow(QString text)
{
    //TODO: add color parsing (ie ch3 in C1) save for version 1.x...
    
    QStringList stitches, tokens, stitchList;
    QRegExp rx("\\[(.*)\\] ([0-9]{1,}) times");
    rx.setMinimal(true);
    
    QMap<QString, QMap<QString, QString> > repeats;
    QString prefix = ".sws_";
    int i = 0, pos = 0;
    while ((pos = rx.indexIn(text, pos)) != -1) {
        QMap<QString, QString> repeat;
        repeat.insert(rx.cap(1), rx.cap(2));
        QString key = prefix + QString::number(i);
        repeats.insert(key, repeat);
        text.replace(pos, rx.matchedLength(), key);
        pos += key.length();
        i++;
    }

    rx = QRegExp("\\*(.*); repeat from \\*");
    i = 0; pos = 0;
    while ((pos = rx.indexIn(text, pos)) != -1) {
        QMap<QString, QString> repeat;
        repeat.insert(rx.cap(1), "2");
        QString key = prefix + QString::number(i);
        repeats.insert(key, repeat);
        text.replace(pos, rx.matchedLength(), key);
        pos += key.length();
        i++;
    }  
    
    stitchList = StitchLibrary::inst()->stitchList();
    tokens = text.split(",");
    
    foreach(QString token, tokens) {
        token = cleanToken(token);
        if(token.startsWith(prefix)) {
            QMap<QString, QString> repeat = repeats.value(token);
            foreach(QString key, repeat.keys()) {
                int reps = repeat.value(key).toInt();
                for(int i = 0; i < reps; ++i)
                    stitches << parseTextRow(key);
            }
        } else {
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
    }
    return stitches;
}

QString CrochetTextView::cleanToken(QString token)
{
    token = token.simplified().toLower();
    if(token.startsWith("["))
        token.replace("[", "");
    
    if(token.contains(QRegExp("\\].{,}$")))
        token.replace(QRegExp("\\].{,}"), "");

    if(token.startsWith("*"))
        token.replace("*", "");
    if(token.contains(QRegExp(";.{,}$")))
        token.replace(QRegExp(";.{,}"), "");

    return token;
}

void CrochetTextView::createChart(int rows, int cols)
{
    Q_UNUSED(cols);
    QTextCursor curs = QTextCursor(textCursor());
    curs.movePosition(QTextCursor::Start);
    for(int i = 0; i < rows; ++i) {
        curs.movePosition(QTextCursor::NextBlock);

        QString rowText = generateTextRow(i);
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
    rowText = generateTextRow(newRow);

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

QTextCursor CrochetTextView::cursorAtRowStart(int row)
{
    QTextCursor curs = QTextCursor(textCursor());
    curs.movePosition(QTextCursor::Start);

    for(int i = 0; i < row; ++i) {
        curs.movePosition(QTextCursor::EndOfBlock);
        if(curs.atEnd())
            curs.insertText("\n");
        curs.movePosition(QTextCursor::NextBlock);
    }
    
    return curs;
}


QString CrochetTextView::copyInstructions()
{
    QStringList rowText;
    QString text;
    
    int rows = mScene->rowCount();
    for(int r = 0; r < rows; ++r) {
        QString text = generateTextRow(r, true, true);
        
        if(text != "")
            rowText << text;
    }

    for(int i = 0; i < rowText.count(); ++i) {
        QString row = rowText[i];
        //don't match rows that are already repeats.
        if(row.startsWith(tr("Repeat row")))
            continue;
        
        if(rowText.count(row) > 1) {
            for(int j = i + 1; j < rowText.count(); ++j) {
                if(rowText[j] == row)
                    rowText[j] = tr("Repeat row %1.").arg(i+1);
            }
        }
    }

    for(int i = 0; i < rowText.count(); ++i)
        text += tr("Row %1: %2\n").arg(i+1).arg(rowText[i]);

    return text;

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
        case Qt::Key_Backspace: { //only eat backspace if we're at the start of the line
            if(!textCursor().atBlockStart()) {
                QPlainTextEdit::keyPressEvent(e);
                return;
            } else {
                e->ignore();
                return;
            }
        }
        case Qt::Key_Delete: { //only eat delete if we're at the end of the line
            if(!textCursor().atBlockEnd()) {
                QPlainTextEdit::keyPressEvent(e);
                return;
            } else {
                e->ignore();
                return;
            }
        }
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

/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochethighlighter.h"

#include "stitchcollection.h"
#include "stitchset.h"

#include <QDebug>

CrochetHighlighter::CrochetHighlighter(QTextDocument *parent)
: QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;

    StitchSet* set = StitchCollection::inst()->masterStitchSet();
    foreach(Stitch* s, set->stitches()) {
        keywordPatterns << "\\b" + s->name() + "\\b";
    }
    
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    
    repeatFormat.setForeground(Qt::darkBlue);
    repeatFormat.setFontWeight(QFont::Bold);
    repeatStartExp = QRegExp("\\*");
    repeatEndExp = QRegExp("; repeat from \\*");

    repeat2Start = QRegExp("\\[");
    repeat2End = QRegExp("\\] [0-9]+ times");
    
}

CrochetHighlighter::~CrochetHighlighter()
{
}

void CrochetHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = repeatStartExp.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = repeatEndExp.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + repeatEndExp.matchedLength();
        }

        setFormat(startIndex, repeatStartExp.matchedLength(), repeatFormat);
        setFormat(endIndex, repeatEndExp.matchedLength(), repeatFormat);
        startIndex = repeatStartExp.indexIn(text, startIndex + commentLength);
    }

    if (previousBlockState() != 2)
        startIndex = repeat2Start.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = repeat2End.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(2);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + repeat2End.matchedLength();
        }

        setFormat(startIndex, repeat2Start.matchedLength(), repeatFormat);
        setFormat(endIndex, repeat2End.matchedLength(), repeatFormat);
        startIndex = repeat2Start.indexIn(text, startIndex + commentLength);
    }
    
}

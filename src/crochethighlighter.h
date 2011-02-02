/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETHIGHLIGHTER_H
#define CROCHETHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class CrochetHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    CrochetHighlighter(QTextDocument *parent = 0);
    ~CrochetHighlighter();

    void resetRules();

    QStringList keywords() { return keywordPatterns; }
    
protected:
    void highlightBlock(const QString &text);
    
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QStringList keywordPatterns;
    
    QRegExp repeatStartExp;
    QRegExp repeatEndExp;

    QRegExp repeat2Start;
    QRegExp repeat2End;
    
    QTextCharFormat keywordFormat;
    QTextCharFormat repeatFormat;

};
#endif //CROCHETHIGHLIGHTER_H

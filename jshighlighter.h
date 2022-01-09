#pragma once

#include <QSyntaxHighlighter>

#include <set>
#include <unordered_map>

enum ColorComponent {
    Background,
    Normal,
    Comment,
    Number,
    String,
    Operator,
    Identifier,
    Keyword,
    BuiltIn,
    Sidebar,
    LineNumber,
    Cursor,
    Marker,
    BracketMatch,
    BracketError,
    FoldIndicator
};

class JSHighlighter : public QSyntaxHighlighter
{
public:
    JSHighlighter(QTextDocument *parent = 0);
    void setColor(ColorComponent component, const QColor &color);
    void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);

    QStringList keywords() const;
    void setKeywords(std::set<QString> &&keywords);
    void setKeywords(const std::set<QString> &keywords);

protected:
    void highlightBlock(const QString &text);

private:
    std::unordered_map<ColorComponent, QColor> m_colors;
    std::set<QString> m_keywords;
    std::set<QString> m_knownIds;

    QString m_markString;
    Qt::CaseSensitivity m_markCaseSensitivity;
};

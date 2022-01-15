#include "jshighlighter.h"

class JSBlockData : public QTextBlockUserData
{
public:
    QList<int> bracketPositions;
};

JSHighlighter::JSHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter{parent},
    m_colors {
        // default color scheme
        { Normal,     QColor("#000000") },
        { Comment,    QColor("#808080") },
        { Number,     QColor("#008000") },
        { String,     QColor("#800000") },
        { Operator,   QColor("#808000") },
        { Identifier, QColor("#000020") },
        { Keyword,    QColor("#000080") },
        { BuiltIn,    QColor("#008080") },
        { Marker,     QColor("#ffff00") },
    },
    m_keywords {
        // https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words
        "break",
        "case",
        "catch",
        "continue",
        "default",
        "delete",
        "do",
        "else",
        "finally",
        "for",
        "function",
        "if",
        "in",
        "instanceof",
        "new",
        "return",
        "switch",
        "this",
        "throw",
        "try",
        "typeof",
        "var",
        "void",
        "while",
        "with",

        "true",
        "false",
        "null",
    },
    m_knownIds {
        // built-in and other popular objects + properties
        "Object",
        "prototype",
        "create",
        "defineProperty",
        "defineProperties",
        "getOwnPropertyDescriptor",
        "keys",
        "getOwnPropertyNames",
        "constructor",
        "__parent__",
        "__proto__",
        "__defineGetter__",
        "__defineSetter__",
        "eval",
        "hasOwnProperty",
        "isPrototypeOf",
        "__lookupGetter__",
        "__lookupSetter__",
        "__noSuchMethod__",
        "propertyIsEnumerable",
        "toSource",
        "toLocaleString",
        "toString",
        "unwatch",
        "valueOf",
        "watch",

        "Function",
        "arguments",
        "arity",
        "caller",
        "constructor",
        "length",
        "name",
        "apply",
        "bind",
        "call",

        "String",
        "fromCharCode",
        "length",
        "charAt",
        "charCodeAt",
        "concat",
        "indexOf",
        "lastIndexOf",
        "localCompare",
        "match",
        "quote",
        "replace",
        "search",
        "slice",
        "split",
        "substr",
        "substring",
        "toLocaleLowerCase",
        "toLocaleUpperCase",
        "toLowerCase",
        "toUpperCase",
        "trim",
        "trimLeft",
        "trimRight",

        "Array",
        "isArray",
        "index",
        "input",
        "pop",
        "push",
        "reverse",
        "shift",
        "sort",
        "splice",
        "unshift",
        "concat",
        "join",
        "filter",
        "forEach",
        "every",
        "map",
        "some",
        "reduce",
        "reduceRight",

        "RegExp",
        "global",
        "ignoreCase",
        "lastIndex",
        "multiline",
        "source",
        "exec",
        "test",

        "JSON",
        "parse",
        "stringify",

        "decodeURI",
        "decodeURIComponent",
        "encodeURI",
        "encodeURIComponent",
        "eval",
        "isFinite",
        "isNaN",
        "parseFloat",
        "parseInt",
        "Infinity",
        "NaN",
        "undefined",

        "Math",
        "E",
        "LN2",
        "LN10",
        "LOG2E",
        "LOG10E",
        "PI",
        "SQRT1_2",
        "SQRT2",
        "abs",
        "acos",
        "asin",
        "atan",
        "atan2",
        "ceil",
        "cos",
        "exp",
        "floor",
        "log",
        "max",
        "min",
        "pow",
        "random",
        "round",
        "sin",
        "sqrt",
        "tan",

        "document",
        "window",
        "navigator",
        "userAgent",
    },
    m_markCaseSensitivity{Qt::CaseInsensitive}
{
}

void JSHighlighter::setColor(ColorComponent component, const QColor &color)
{
    m_colors[component] = color;
    rehighlight();
}

void JSHighlighter::highlightBlock(const QString &text)
{
    // parsing state
    enum {
        Start = 0,
        Number = 1,
        Identifier = 2,
        String = 3,
        Comment = 4,
        Regex = 5
    };

    QList<int> bracketPositions;

    int blockState = previousBlockState();
    int bracketLevel = blockState >> 4;
    int state = blockState & 15;
    if (blockState < 0)
    {
        bracketLevel = 0;
        state = Start;
    }

    int start = 0;
    int i = 0;
    while (i <= text.length())
    {
        QChar ch = (i < text.length()) ? text.at(i) : QChar();
        QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();

        switch (state)
        {
        case Start:
            start = i;
            if (ch.isSpace())
            {
                ++i;
            }
            else if (ch.isDigit())
            {
                ++i;
                state = Number;
            }
            else if (ch.isLetter() || ch == '_')
            {
                ++i;
                state = Identifier;
            }
            else if (ch == '\'' || ch == '\"')
            {
                ++i;
                state = String;
            }
            else if (ch == '/' && next == '*')
            {
                ++i;
                ++i;
                state = Comment;
            }
            else if (ch == '/' && next == '/')
            {
                i = text.length();
                setFormat(start, text.length(), m_colors[ColorComponent::Comment]);
            }
            else if (ch == '/' && next != '*')
            {
                ++i;
                state = Regex;
            }
            else
            {
                if (!QString("(){}[]").contains(ch))
                    setFormat(start, 1, m_colors[Operator]);
                if (ch =='{' || ch == '}')
                {
                    bracketPositions += i;
                    if (ch == '{')
                        bracketLevel++;
                    else
                        bracketLevel--;
                }
                ++i;
                state = Start;
            }
            break;

        case Number:
            if (ch.isSpace() || !ch.isDigit())
            {
                setFormat(start, i - start, m_colors[ColorComponent::Number]);
                state = Start;
            }
            else
            {
                ++i;
            }
            break;

        case Identifier:
            if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_'))
            {
                QString token = text.mid(start, i - start).trimmed();
                if (m_keywords.contains(token))
                    setFormat(start, i - start, m_colors[Keyword]);
                else if (m_knownIds.contains(token))
                    setFormat(start, i - start, m_colors[BuiltIn]);
                state = Start;
            }
            else
            {
                ++i;
            }
            break;

        case String:
            if (ch == text.at(start)) {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (prev != '\\') {
                    ++i;
                    setFormat(start, i - start, m_colors[ColorComponent::String]);
                    state = Start;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }
            break;

        case Comment:
            if (ch == '*' && next == '/')
            {
                ++i;
                ++i;
                setFormat(start, i - start, m_colors[ColorComponent::Comment]);
                state = Start;
            }
            else
            {
                ++i;
            }
            break;

        case Regex:
            if (ch == '/')
            {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (prev != '\\')
                {
                    ++i;
                    setFormat(start, i - start, m_colors[ColorComponent::String]);
                    state = Start;
                }
                else
                {
                    ++i;
                }
            }
            else
            {
                ++i;
            }
            break;

        default:
            state = Start;
            break;
        }
    }

    if (state == Comment)
        setFormat(start, text.length(), m_colors[ColorComponent::Comment]);
    else
        state = Start;

    if (!m_markString.isEmpty())
    {
        int pos = 0;
        int len = m_markString.length();
        QTextCharFormat markerFormat;
        markerFormat.setBackground(m_colors[Marker]);
        markerFormat.setForeground(m_colors[Normal]);
        for (;;)
        {
            pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
            if (pos < 0)
                break;
            setFormat(pos, len, markerFormat);
            ++pos;
        }
    }

    if (!bracketPositions.isEmpty())
    {
        JSBlockData *blockData = reinterpret_cast<JSBlockData*>(currentBlock().userData());
        if (!blockData)
        {
            blockData = new JSBlockData;
            currentBlock().setUserData(blockData);
        }
        blockData->bracketPositions = bracketPositions;
    }

    blockState = (state & 15) | (bracketLevel << 4);
    setCurrentBlockState(blockState);
}

void JSHighlighter::mark(const QString &str, Qt::CaseSensitivity caseSensitivity)
{
    m_markString = str;
    m_markCaseSensitivity = caseSensitivity;
    rehighlight();
}

QStringList JSHighlighter::keywords() const
{
    return QStringList{std::begin(m_keywords), std::end(m_keywords)};
}

void JSHighlighter::setKeywords(std::set<QString> &&keywords)
{
    m_keywords = std::move(keywords);
    rehighlight();
}

void JSHighlighter::setKeywords(const std::set<QString> &keywords)
{
    m_keywords = keywords;
    rehighlight();
}

struct BlockInfo
{
    int position;
    int number;
    bool foldable: 1;
    bool folded : 1;
};

Q_DECLARE_TYPEINFO(BlockInfo, Q_PRIMITIVE_TYPE);

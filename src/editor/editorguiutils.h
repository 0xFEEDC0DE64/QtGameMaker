#pragma once

#include <QObject>
#include <QColor>

class QBrush;
class QPen;
class QDialogButtonBox;

QBrush makeGridBrush(int snapX, int snapY, const QPen &gridPen, const QColor &backgroundColor);
QBrush makeGridBrush(int snapX, int snapY, const QColor &gridColor, const QColor &backgroundColor);

struct TransparentBackgroundPattern
{
    Q_GADGET
public:

    enum class Type {
        BlockPattern,
        SingleColor
    };
    Q_ENUM(Type)

    Type type{Type::BlockPattern};
    QColor color1{Qt::gray};
    QColor color2{Qt::white};
    int blockSize{16};

    QColor singleColor{Qt::darkGray};

    constexpr bool operator==(const TransparentBackgroundPattern &other) const = default;
};

QBrush makeTransparentBackgroundPatternBrush(const TransparentBackgroundPattern &transparentBackgroundPattern);

void improveButtonBox(QDialogButtonBox *buttonBox);

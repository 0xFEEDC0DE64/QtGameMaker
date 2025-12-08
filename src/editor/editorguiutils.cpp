#include "editorguiutils.h"

#include <QBrush>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QColor>
#include <QDialogButtonBox>
#include <QPushButton>

QBrush makeGridBrush(int snapX, int snapY, const QPen &gridPen, const QColor &backgroundColor)
{
    QPixmap pixmap{snapX, snapY};
    pixmap.fill(backgroundColor);

    {
        QPainter painter{&pixmap};
        painter.setPen(gridPen);
        painter.drawLine(0, 0, snapX, 0);
        painter.drawLine(0, 0, 0, snapY);
    }

    return QBrush{std::move(pixmap)};
}

QBrush makeGridBrush(int snapX, int snapY, const QColor &gridColor, const QColor &backgroundColor)
{
    QPixmap pixmap{snapX, snapY};
    pixmap.fill(backgroundColor);

    {
        QPainter painter{&pixmap};
        painter.setPen(gridColor);
        painter.drawLine(0, 0, snapX, 0);
        painter.drawLine(0, 0, 0, snapY);
    }

    return QBrush{std::move(pixmap)};
}

void improveButtonBox(QDialogButtonBox *buttonBox)
{
    if (auto button = buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

static QBrush makeBlockPatternBrush(QColor color1, QColor color2, int blockSize)
{
    QBrush brush;
    QPixmap pixmap{blockSize * 2, blockSize * 2};
    QPainter painter{&pixmap};
    painter.setPen(Qt::NoPen);
    painter.setBrush(color1);
    painter.drawRect(0, 0, blockSize, blockSize);
    painter.drawRect(blockSize, blockSize, blockSize, blockSize);
    painter.setBrush(color2);
    painter.drawRect(0, blockSize, blockSize, blockSize);
    painter.drawRect(blockSize, 0, blockSize, blockSize);
    brush.setTexture(std::move(pixmap));
    return brush;
}

QBrush makeTransparentBackgroundPatternBrush(const TransparentBackgroundPattern &transparentBackgroundPattern)
{
    switch (transparentBackgroundPattern.type)
    {
    case TransparentBackgroundPattern::Type::BlockPattern:
        return makeBlockPatternBrush(
            transparentBackgroundPattern.color1,
            transparentBackgroundPattern.color2,
            transparentBackgroundPattern.blockSize);
    case TransparentBackgroundPattern::Type::SingleColor:
        return transparentBackgroundPattern.singleColor;
    }
    qWarning() << "unknown TransparentBackgroundPattern::Type" << transparentBackgroundPattern.type;
    return {};
}

void registerEditorMetatypes()
{
    qRegisterMetaType<TransparentBackgroundPattern::Type>();
}

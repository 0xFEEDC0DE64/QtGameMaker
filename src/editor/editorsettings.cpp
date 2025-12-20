#include "editorsettings.h"

#include <QColor>

QByteArray EditorSettings::mainWindowGeometry() const
{
    return value("mainWindowGeometry").toByteArray();
}

void EditorSettings::setMainWindowGeometry(const QByteArray &mainWindowGeometry)
{
    setValue("mainWindowGeometry", mainWindowGeometry);
}

QByteArray EditorSettings::mainWindowState() const
{
    return value("mainWindowState").toByteArray();
}

void EditorSettings::setMainWindowState(const QByteArray &mainWindowState)
{
    setValue("mainWindowState", mainWindowState);
}

TransparentBackgroundPattern::Type EditorSettings::transparentBackgroundPatternType() const
{
    return value("transparentBackgroundPatternType", QVariant::fromValue(TransparentBackgroundPattern::Type::BlockPattern)).value<TransparentBackgroundPattern::Type>();
}

void EditorSettings::setTransparentBackgroundPatternType(TransparentBackgroundPattern::Type transparentBackgroundPatternType)
{
    setValue("transparentBackgroundPatternType", QVariant::fromValue(transparentBackgroundPatternType));
}

QColor EditorSettings::transparentBackgroundBlockPatternColor1() const
{
    return value("transparentBackgroundBlockPatternColor1", QColor{Qt::gray}).value<QColor>();
}

void EditorSettings::setTransparentBackgroundBlockPatternColor1(QColor transparentBackgroundBlockPatternColor1)
{
    setValue("transparentBackgroundBlockPatternColor1", transparentBackgroundBlockPatternColor1);
}

QColor EditorSettings::transparentBackgroundBlockPatternColor2() const
{
    return value("transparentBackgroundBlockPatternColor2", QColor{Qt::white}).value<QColor>();
}

void EditorSettings::setTransparentBackgroundBlockPatternColor2(QColor transparentBackgroundBlockPatternColor2)
{
    setValue("transparentBackgroundBlockPatternColor2", transparentBackgroundBlockPatternColor2);
}

int EditorSettings::transparentBackgroundBlockPatternBlockSize() const
{
    return value("transparentBackgroundBlockPatternBlockSize", 16).toInt();
}

void EditorSettings::setTransparentBackgroundBlockPatternBlockSize(int transparentBackgroundBlockPatternBlockSize)
{
    setValue("transparentBackgroundBlockPatternBlockSize", transparentBackgroundBlockPatternBlockSize);
}

QColor EditorSettings::transparentBackgroundSingleColor() const
{
    return value("transparentBackgroundSingleColor", QColor{Qt::darkGray}).value<QColor>();
}

void EditorSettings::setTransparentBackgroundSingleColor(QColor transparentBackgroundSingleColor)
{
    setValue("transparentBackgroundSingleColor", transparentBackgroundSingleColor);
}

TransparentBackgroundPattern EditorSettings::transparentBackgroundPattern() const
{
    return TransparentBackgroundPattern {
        transparentBackgroundPatternType(),
        transparentBackgroundBlockPatternColor1(),
        transparentBackgroundBlockPatternColor2(),
        transparentBackgroundBlockPatternBlockSize(),
        transparentBackgroundSingleColor()
    };
}

bool EditorSettings::hideWebsiteImage() const
{
    return value("hideWebsiteImage", false).toBool();
}

void EditorSettings::setHideWebsiteImage(bool hideWebsiteImage)
{
    setValue("hideWebsiteImage", hideWebsiteImage);
}

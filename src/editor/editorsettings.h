#pragma once

#include <QSettings>

#include "editorguiutils.h"

class EditorSettings : public QSettings
{
    Q_OBJECT

public:
    using QSettings::QSettings;

    QByteArray mainWindowGeometry() const;
    void setMainWindowGeometry(const QByteArray &mainWindowGeometry);

    QByteArray mainWindowState() const;
    void setMainWindowState(const QByteArray &mainWindowState);

    TransparentBackgroundPattern::Type transparentBackgroundPatternType() const;
    void setTransparentBackgroundPatternType(TransparentBackgroundPattern::Type transparentBackgroundPatternType);

    QColor transparentBackgroundBlockPatternColor1() const;
    void setTransparentBackgroundBlockPatternColor1(QColor transparentBackgroundBlockPatternColor1);

    QColor transparentBackgroundBlockPatternColor2() const;
    void setTransparentBackgroundBlockPatternColor2(QColor transparentBackgroundBlockPatternColor2);

    int transparentBackgroundBlockPatternBlockSize() const;
    void setTransparentBackgroundBlockPatternBlockSize(int transparentBackgroundBlockPatternBlockSize);

    QColor transparentBackgroundSingleColor() const;
    void setTransparentBackgroundSingleColor(QColor transparentBackgroundSingleColor);

    TransparentBackgroundPattern transparentBackgroundPattern() const;

    QStringList recentFiles() const;
    void pushRecentFile(const QString &file);
    void clearRecentFiles();
    void setRecentFiles(const QStringList &recentFiles);

    bool showRecentFiles() const;
    void setShowRecentFiles(bool showRecentFiles);

    bool autoLoadLastFile() const;
    void setAutoLoadLastFile(bool autoLoadLastFile);

    bool hideWebsiteImage() const;
    void setHideWebsiteImage(bool hideWebsiteImage);
};

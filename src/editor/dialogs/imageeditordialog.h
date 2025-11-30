#pragma once

#include <QDialog>
#include <QString>

#include <memory>

#include "widgets/drawingcanvaswidget.h"

class QLabel;
class QPixmap;
class QImage;
namespace Ui { class ImageEditorDialog; }
class EditorSettings;

class ImageEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageEditorDialog(const QPixmap &pixmap, const QString &title,
                               EditorSettings &settings, QWidget *parent = nullptr);
    ~ImageEditorDialog();

    QPixmap pixmap() const;
    const QImage &image() const;

    void reject() override;

private slots:
    void saveAsPng();
    void transparentBackgroundSettings();

    void changed();
    void selectFont();
    void modeChanged(DrawingCanvasWidget::Mode mode);
    void cursorMoved(const QPoint &pos);
    void zoomChanged(float zoom);

private:
    void updateTitle();
    void updateSize(const QPixmap &pixmap);

    const std::unique_ptr<Ui::ImageEditorDialog> m_ui;

    const QString m_title;
    EditorSettings &m_settings;

    QLabel &m_posLabel;
    QLabel &m_zoomLabel;
    QLabel &m_sizeLabel;
    QLabel &m_memoryLabel;

    bool m_unsavedChanges{};
};

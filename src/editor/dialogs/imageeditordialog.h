#pragma once

#include <QDialog>
#include <QPixmap>
#include <QString>

#include <memory>

namespace Ui { class ImageEditorDialog; }

class ImageEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageEditorDialog(const QPixmap &pixmap, const QString &title, QWidget *parent = nullptr);
    ~ImageEditorDialog();

    const QPixmap &pixmap() const { return m_pixmap; }

    void accept() override;
    void reject() override;

private slots:
    void changed();
    void selectFont();
    void selectLeftButtonColor();
    void selectRightButtonColor();
    void updateLeftButtonColor(const QColor &leftButtonColor);
    void updateRightButtonColor(const QColor &rightButtonColor);

private:
    void updateTitle();

    const std::unique_ptr<Ui::ImageEditorDialog> m_ui;

    QPixmap m_pixmap;
    const QString m_title;

    bool m_unsavedChanges{};
};

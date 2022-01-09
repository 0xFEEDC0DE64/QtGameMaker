#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class ImageEditorDialog; }

class ImageEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageEditorDialog(QWidget *parent = nullptr);
    ~ImageEditorDialog();

private:
    const std::unique_ptr<Ui::ImageEditorDialog> m_ui;
};

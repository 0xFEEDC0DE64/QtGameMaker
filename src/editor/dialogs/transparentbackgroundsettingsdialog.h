#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class TransparentBackgroundSettingsDialog; }

class TransparentBackgroundSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransparentBackgroundSettingsDialog(QWidget *parent = nullptr);
    ~TransparentBackgroundSettingsDialog() override;

private:
    const std::unique_ptr<Ui::TransparentBackgroundSettingsDialog> m_ui;
};

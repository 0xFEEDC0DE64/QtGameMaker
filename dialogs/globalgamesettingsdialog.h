#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class GlobalGameSettingsDialog; }

class GlobalGameSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalGameSettingsDialog(QWidget *parent = nullptr);
    ~GlobalGameSettingsDialog();

private:
    const std::unique_ptr<Ui::GlobalGameSettingsDialog> m_ui;
};

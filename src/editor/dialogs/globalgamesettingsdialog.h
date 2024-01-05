#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class GlobalGameSettingsDialog; }
class GlobalGameSettings;

class GlobalGameSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalGameSettingsDialog(GlobalGameSettings &globalGameSettings, QWidget *parent = nullptr);
    ~GlobalGameSettingsDialog();

    void accept() override;
    void reject() override;

private slots:
    void changed();

private:
    void updateTitle();

    const std::unique_ptr<Ui::GlobalGameSettingsDialog> m_ui;

    GlobalGameSettings &m_globalGameSettings;

    bool m_unsavedChanges{};
};

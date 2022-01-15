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

    void accept() override;
    void reject() override;

private slots:
    void changed();

private:
    void updateTitle();

    const std::unique_ptr<Ui::GlobalGameSettingsDialog> m_ui;

    bool m_unsavedChanges{};
};

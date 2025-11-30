#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class TransparentBackgroundSettingsDialog; }
class EditorSettings;

class TransparentBackgroundSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransparentBackgroundSettingsDialog(const EditorSettings &settings, QWidget *parent = nullptr);
    ~TransparentBackgroundSettingsDialog() override;

    void save(EditorSettings &settings);

public slots:
    void accept() override;

private:
    const std::unique_ptr<Ui::TransparentBackgroundSettingsDialog> m_ui;
};

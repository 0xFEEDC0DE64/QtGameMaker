#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class PreferencesDialog; }
class EditorSettings;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(const EditorSettings &settings, QWidget *parent = nullptr);
    ~PreferencesDialog();

    void save(EditorSettings &settings);

public slots:
    void accept() override;

private:
    const std::unique_ptr<Ui::PreferencesDialog> m_ui;
};

#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class PreferencesDialog; }

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

private:
    const std::unique_ptr<Ui::PreferencesDialog> m_ui;
};

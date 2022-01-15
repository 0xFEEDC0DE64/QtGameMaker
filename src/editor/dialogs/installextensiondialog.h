#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class InstallExtensionDialog; }

class InstallExtensionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstallExtensionDialog(QWidget *parent = nullptr);
    ~InstallExtensionDialog();

private:
    const std::unique_ptr<Ui::InstallExtensionDialog> m_ui;
};

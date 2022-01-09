#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class ExtensionPackagesDialog; }

class ExtensionPackagesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExtensionPackagesDialog(QWidget *parent = nullptr);
    ~ExtensionPackagesDialog();

private:
    const std::unique_ptr<Ui::ExtensionPackagesDialog> m_ui;
};

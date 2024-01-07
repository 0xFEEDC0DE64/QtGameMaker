#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class IncludedFilePropertiesDialog; }
struct IncludedFile;

class IncludedFilePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IncludedFilePropertiesDialog(IncludedFile &includedFile, QWidget *parent = nullptr);
    ~IncludedFilePropertiesDialog() override;

public slots:
    void accept() override;

private:
    IncludedFile &m_includedFile;

    const std::unique_ptr<Ui::IncludedFilePropertiesDialog> m_ui;
};

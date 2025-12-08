#pragma once

#include <QDialog>
#include <memory>

namespace Ui { class ChangeImageSizeDialog; }

class ChangeImageSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeImageSizeDialog(QWidget *parent = nullptr);
    ~ChangeImageSizeDialog() override;

private:
    const std::unique_ptr<Ui::ChangeImageSizeDialog> m_ui;
};

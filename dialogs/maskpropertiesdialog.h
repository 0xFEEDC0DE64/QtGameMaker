#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class MaskPropertiesDialog; }

class MaskPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaskPropertiesDialog(QWidget *parent = nullptr);
    ~MaskPropertiesDialog();

private:
    const std::unique_ptr<Ui::MaskPropertiesDialog> m_ui;
};

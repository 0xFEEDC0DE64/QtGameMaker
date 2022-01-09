#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class ObjectInformationDialog; }

class ObjectInformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectInformationDialog(QWidget *parent = nullptr);
    ~ObjectInformationDialog();

private:
    const std::unique_ptr<Ui::ObjectInformationDialog> m_ui;
};

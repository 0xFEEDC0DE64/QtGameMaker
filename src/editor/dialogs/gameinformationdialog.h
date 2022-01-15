#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class GameInformationDialog; }

class GameInformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameInformationDialog(QWidget *parent = nullptr);
    ~GameInformationDialog();

private:
    const std::unique_ptr<Ui::GameInformationDialog> m_ui;
};

#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class SpeedHorizontalDialog; }
struct SpeedHorizontalAction;

class SpeedHorizontalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpeedHorizontalDialog(SpeedHorizontalAction &action, QWidget *parent = nullptr);
    ~SpeedHorizontalDialog() override;

private:
    const std::unique_ptr<Ui::SpeedHorizontalDialog> m_ui;

    SpeedHorizontalAction &m_action;
};

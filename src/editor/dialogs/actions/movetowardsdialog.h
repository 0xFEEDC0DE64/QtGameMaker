#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class MoveTowardsDialog; }
class MoveTowardsAction;

class MoveTowardsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoveTowardsDialog(MoveTowardsAction &action, QWidget *parent = nullptr);
    ~MoveTowardsDialog() override;

private:
    const std::unique_ptr<Ui::MoveTowardsDialog> m_ui;

    MoveTowardsAction &m_action;
};

#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class MoveFixedDialog; }
class MoveFixedAction;
class ProjectTreeModel;

class MoveFixedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoveFixedDialog(MoveFixedAction &action, ProjectTreeModel *projectModel, QWidget *parent = nullptr);
    ~MoveFixedDialog() override;

private:
    const std::unique_ptr<Ui::MoveFixedDialog> m_ui;

    MoveFixedAction &m_action;
};

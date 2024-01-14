#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class SetGravityDialog; }
struct SetGravityAction;
class ProjectTreeModel;

class SetGravityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetGravityDialog(SetGravityAction &action, ProjectTreeModel *projectModel, QWidget *parent = nullptr);
    ~SetGravityDialog() override;

private:
    const std::unique_ptr<Ui::SetGravityDialog> m_ui;

    SetGravityAction &m_action;
};

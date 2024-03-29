#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class MoveFreeDialog; }
class MoveFreeAction;
class ProjectTreeModel;

class MoveFreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoveFreeDialog(MoveFreeAction &action, ProjectTreeModel *projectModel, QWidget *parent = nullptr);
    ~MoveFreeDialog() override;

private:
    const std::unique_ptr<Ui::MoveFreeDialog> m_ui;

    MoveFreeAction &m_action;
};

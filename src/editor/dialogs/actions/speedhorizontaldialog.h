#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class SpeedHorizontalDialog; }
struct SpeedHorizontalAction;
class ProjectTreeModel;

class SpeedHorizontalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpeedHorizontalDialog(SpeedHorizontalAction &action, ProjectTreeModel *projectModel, QWidget *parent = nullptr);
    ~SpeedHorizontalDialog() override;

private:
    const std::unique_ptr<Ui::SpeedHorizontalDialog> m_ui;

    SpeedHorizontalAction &m_action;
};

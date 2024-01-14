#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class SpeedVerticalDialog; }
struct SpeedVerticalAction;
class ProjectTreeModel;

class SpeedVerticalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpeedVerticalDialog(SpeedVerticalAction &action, ProjectTreeModel *projectModel, QWidget *parent = nullptr);
    ~SpeedVerticalDialog() override;

private:
    const std::unique_ptr<Ui::SpeedVerticalDialog> m_ui;

    SpeedVerticalAction &m_action;
};

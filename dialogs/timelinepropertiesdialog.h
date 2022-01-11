#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class TimeLinePropertiesDialog; }
struct TimeLine;
class ProjectTreeModel;
class TimelineMomentsModel;
class TimelineActionsModel;

class TimeLinePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeLinePropertiesDialog(TimeLine &timeLine, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~TimeLinePropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void changed();

    void timeLineNameChanged(const TimeLine &timeLine);

    void add();
    void change();
    void delete_();
    void clear();
    void shift();
    void duplicate();
    void spread();
    void merge();
    void showInformation();

private:
    void updateTitle();

    const std::unique_ptr<Ui::TimeLinePropertiesDialog> m_ui;

    TimeLine &m_timeLine;
    ProjectTreeModel &m_projectModel;

    const std::unique_ptr<TimelineMomentsModel> m_momentsModel;
    const std::unique_ptr<TimelineActionsModel> m_actionsModel;

    bool m_unsavedChanges{};
};

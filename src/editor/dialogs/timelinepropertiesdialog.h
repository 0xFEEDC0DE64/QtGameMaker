#pragma once

#include <QDialog>

#include <memory>

#include "projectcontainer.h"

namespace Ui { class TimeLinePropertiesDialog; }
class ProjectTreeModel;
class TimelineMomentsModel;

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

    void addMoment();
    void changeMoment();
    void deleteMoments();
    void clearMoments();
    void shiftMoments();
    void duplicateMoments();
    void spreadMoments();
    void mergeMoments();
    void showInformation();

    void currentMomentChanged(const QModelIndex &index);
    void momentsContextMenuRequested(const QPoint &pos);

private:
    void updateTitle();

    const std::unique_ptr<Ui::TimeLinePropertiesDialog> m_ui;

    TimeLine &m_timeLine;
    ProjectTreeModel &m_projectModel;

    TimeLine::moments_container_t m_moments;

    const std::unique_ptr<TimelineMomentsModel> m_momentsModel;

    bool m_unsavedChanges{};
};

#pragma once

#include <QDialog>

#include <memory>

#include "projectcontainer.h"
#include "projectchangesapplier.h"

namespace Ui { class TimeLinePropertiesDialog; }
class ProjectTreeModel;
class TimelineMomentsModel;
class MainWindow;

class TimeLinePropertiesDialog : public QDialog, public ProjectChangesApplier
{
    Q_OBJECT

public:
    explicit TimeLinePropertiesDialog(TimeLine &timeLine, ProjectTreeModel &projectModel, MainWindow &mainWindow);
    ~TimeLinePropertiesDialog();

    void accept() override;
    bool applyChanges() override;
    void reject() override;

private slots:
    void changed();

    void timeLineNameChanged(const TimeLine &timeLine);
    void objectAboutToBeRemoved(const Object &object);
    void objectNameChanged(const Object &object, const QString &oldName);

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

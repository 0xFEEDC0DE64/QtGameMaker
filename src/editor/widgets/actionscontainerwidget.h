#pragma once

#include <QWidget>

#include <memory>

#include "projectcontainer.h"

namespace Ui { class ActionsContainerWidget; }
class ActionsContainerModel;
class ProjectTreeModel;

class ActionsContainerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActionsContainerWidget(QWidget *parent = nullptr);
    ~ActionsContainerWidget();

    ProjectTreeModel *projectModel() { return m_projectModel; }
    const ProjectTreeModel *projectModel() const { return m_projectModel; }
    void setProjectModel(ProjectTreeModel *projectModel) { m_projectModel = projectModel; }

    ActionsContainer *actionsContainer();
    const ActionsContainer *actionsContainer() const;
    void setActionsContainer(ActionsContainer *actionsContainer);

signals:
    void changed();

private slots:
    void actionDoubleClicked(const QModelIndex &index);
    void actionsContextMenuRequested(const QPoint &pos);
    void actionsContainerMissing();

private:
    template<typename T>
    void createNewAction();

    const std::unique_ptr<Ui::ActionsContainerWidget> m_ui;

    const std::unique_ptr<ActionsContainerModel> m_actionsModel;

    ProjectTreeModel *m_projectModel{};
};

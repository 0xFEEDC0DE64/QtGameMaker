#pragma once

#include <QWidget>

#include <memory>

#include "projectcontainer.h"

namespace Ui { class ActionsContainerWidget; }
class ActionsContainerModel;

class ActionsContainerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActionsContainerWidget(QWidget *parent = nullptr);
    ~ActionsContainerWidget();

    ActionsContainer *actionsContainer() const;
    void setActionsContainer(ActionsContainer *actionsContainer);

signals:
    void changed();

private slots:
    void actionDoubleClicked(const QModelIndex &index);
    void actionsContextMenuRequested(const QPoint &pos);

private:
    const std::unique_ptr<Ui::ActionsContainerWidget> m_ui;

    const std::unique_ptr<ActionsContainerModel> m_actionsModel;
};

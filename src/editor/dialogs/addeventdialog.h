#pragma once

#include <QDialog>

#include <optional>
#include <variant>

#include "projectcontainer.h"

namespace Ui { class AddEventDialog; }

class ProjectTreeModel;

class AddEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEventDialog(ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~AddEventDialog();

    const std::optional<std::variant<Object::EventType, QString>> &eventType() const { return m_eventType; }

    void accept() override;
    void reject() override;

private:
    ProjectTreeModel &m_projectModel;

    const std::unique_ptr<Ui::AddEventDialog> m_ui;

    std::optional<std::variant<Object::EventType, QString>> m_eventType;
};

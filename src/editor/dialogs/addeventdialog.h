#pragma once

#include <QDialog>

#include <optional>
#include <variant>

#include "projectcontainer.h"

namespace Ui { class AddEventDialog; }

class ProjectTreeModel;
class MainWindow;

class AddEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEventDialog(ProjectTreeModel &projectModel, MainWindow &mainWindow, QWidget *parent = nullptr);
    ~AddEventDialog();

    const std::optional<std::variant<Object::EventType, QString>> &eventType() const { return m_eventType; }

    void accept() override;
    void reject() override;

private:
    ProjectTreeModel &m_projectModel;
    MainWindow &m_mainWindow;

    const std::unique_ptr<Ui::AddEventDialog> m_ui;

    std::optional<std::variant<Object::EventType, QString>> m_eventType;
};

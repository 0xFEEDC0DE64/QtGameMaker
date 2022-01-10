#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class TriggersDialog; }
struct ProjectContainer;
class TriggersModel;

class TriggersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TriggersDialog(ProjectContainer &project, QWidget *parent = nullptr);
    ~TriggersDialog();

private slots:
    void openCodeEditor();

private:
    const std::unique_ptr<Ui::TriggersDialog> m_ui;

    ProjectContainer &m_project;

    const std::unique_ptr<TriggersModel> m_model;
};

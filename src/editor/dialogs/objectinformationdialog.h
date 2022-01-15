#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class ObjectInformationDialog; }
struct ProjectContainer;

class ObjectInformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectInformationDialog(ProjectContainer &project, QWidget *parent = nullptr);
    ~ObjectInformationDialog();

private:
    const std::unique_ptr<Ui::ObjectInformationDialog> m_ui;

    ProjectContainer &m_project;
};

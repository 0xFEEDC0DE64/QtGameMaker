#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class IncludedFilesDialog; }
struct ProjectContainer;
class IncludedFilesModel;

class IncludedFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IncludedFilesDialog(ProjectContainer &project, QWidget *parent = nullptr);
    ~IncludedFilesDialog();

private:
    const std::unique_ptr<Ui::IncludedFilesDialog> m_ui;

    ProjectContainer &m_project;

    const std::unique_ptr<IncludedFilesModel> m_model;
};

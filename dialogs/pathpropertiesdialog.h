#pragma once

#include <QDialog>

#include <memory>

class QLabel;
namespace Ui { class PathPropertiesDialog; }
struct Path;
class ProjectTreeModel;

class PathPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathPropertiesDialog(Path &path, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~PathPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void add();
    void insert();
    void delete_();

    void changed();

    void pathNameChanged(const Path &path);

private:
    void updateTitle();

    const std::unique_ptr<Ui::PathPropertiesDialog> m_ui;

    Path &m_path;
    ProjectTreeModel &m_projectModel;

    bool m_unsavedChanges{};

    QLabel * const m_labelX;
    QLabel * const m_labelY;
    QLabel * const m_labelArea;
};

#pragma once

#include <QDialog>

#include <memory>
#include <optional>

#include "projectcontainer.h"

class QSpinBox;
class QLabel;
namespace Ui { class PathPropertiesDialog; }
class ProjectTreeModel;
class PathPointsModel;

class PathPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathPropertiesDialog(Path &path, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~PathPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void selectionChanged(const QModelIndex &index);
    void selectedPointChanged(const std::optional<std::size_t> &index);
    void dataChanged(const QModelIndex &index);
    void cursorMoved(const QPoint &point);

    void add();
    void insert();
    void delete_();

    void changed();

    void pathNameChanged(const Path &path);

private:
    void updateTitle();
    void updatePointFields();

    const std::unique_ptr<Ui::PathPropertiesDialog> m_ui;

    Path &m_path;
    ProjectTreeModel &m_projectModel;

    std::vector<Path::Point> m_points;

    const std::unique_ptr<PathPointsModel> m_pointsModel;

    bool m_unsavedChanges{};

    QSpinBox * const m_spinBoxSnapX;
    QSpinBox * const m_spinBoxSnapY;

    QLabel * const m_labelX;
    QLabel * const m_labelY;
    QLabel * const m_labelArea;
};

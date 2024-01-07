#pragma once

#include <QDialog>

#include <memory>
#include <optional>

#include "projectcontainer.h"

class QSpinBox;
class QLabel;
class QMenu;
namespace Ui { class PathPropertiesDialog; }
class ProjectTreeModel;
class PathPointsModel;
class MainWindow;

class PathPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathPropertiesDialog(Path &path, ProjectTreeModel &projectModel, MainWindow &mainWindow);
    ~PathPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void selectionChanged(const QModelIndex &index);
    void selectedPointChanged(const std::optional<std::size_t> &index);
    void dataChanged(const QModelIndex &index);
    void cursorMoved(const QPoint &point);

    void addPoint();
    void insertPoint();
    void deletePoint();

    void undo();
    void clearPath();
    void reversePath();
    void shiftPath();
    void mirrorPathHorizontally();
    void mirrorPathVertically();
    void rotatePath();
    void scalePath();
    void shiftViewLeft();
    void shiftViewRight();
    void shiftViewUp();
    void shiftViewDown();
    void centerView();

    void pointFieldsChanged();
    void changed();

    void pathNameChanged(const Path &path);

    void roomsMenuAboutToShow();

private:
    void updateTitle();
    void updatePointFields();

    Path::Point getPointFromFields() const;

    const std::unique_ptr<Ui::PathPropertiesDialog> m_ui;

    Path &m_path;
    ProjectTreeModel &m_projectModel;

    std::vector<Path::Point> m_points;

    const std::unique_ptr<PathPointsModel> m_pointsModel;

    bool m_unsavedChanges{};

    QSpinBox * const m_spinBoxSnapX;
    QSpinBox * const m_spinBoxSnapY;

    QMenu * const m_menuRooms;

    QLabel * const m_labelX;
    QLabel * const m_labelY;
    QLabel * const m_labelArea;
};

#pragma once

#include <QWidget>
#include <QPalette>

#include <optional>

#include "projectcontainer.h"

struct Object;
class ProjectTreeModel;

class RoomEditWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int snapX READ snapX WRITE setSnapX NOTIFY snapXChanged)
    Q_PROPERTY(int snapY READ snapY WRITE setSnapY NOTIFY snapYChanged)
    Q_PROPERTY(bool gridEnabled READ gridEnabled WRITE setGridEnabled NOTIFY gridEnabledChanged)
    Q_PROPERTY(bool isometricGrid READ isometricGrid WRITE setIsometricGrid NOTIFY isometricGridChanged)

public:
    explicit RoomEditWidget(QWidget *parent = nullptr);

    std::vector<Room::Object> *objects() { return m_objects; }
    const std::vector<Room::Object> *objects() const { return m_objects; }
    void setObjects(std::vector<Room::Object> *objects);

    int snapX() const { return m_snapX; }
    void setSnapX(int snapX);

    int snapY() const { return m_snapY; }
    void setSnapY(int snapY);

    bool gridEnabled() const { return m_gridEnabled; }
    void setGridEnabled(bool gridEnabled);

    bool isometricGrid() const { return m_isometricGrid; }
    void setIsometricGrid(bool isometricGrid);

    QPalette::ColorRole gridRole() const { return m_gridRole; }
    void setGridRole(QPalette::ColorRole gridRole);

    ProjectTreeModel *projectTreeModel() { return m_projectTreeModel; }
    const ProjectTreeModel *projectTreeModel() const { return m_projectTreeModel; }
    void setProjectTreeModel(ProjectTreeModel *projectTreeModel) { m_projectTreeModel = projectTreeModel; update(); }

    const Object *selectedObject() const { return m_selectedObject; }
    void setSelectedObject(const Object *selectedObject);

signals:
    void snapXChanged(int snapX);
    void snapYChanged(int snapY);
    void gridEnabledChanged(bool gridEnabled);
    void isometricGridChanged(bool isometricGrid);

    void cursorMoved(const QPoint &point);

    void changed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint snapPoint(const QPoint &point) const;
    void paintObjects(QPainter &painter);
    void paintDraggedObject(QPainter &painter);

    std::vector<Room::Object> *m_objects{};

    int m_snapX{16};
    int m_snapY{16};
    bool m_gridEnabled{true};
    bool m_isometricGrid{false};

    const Object *m_selectedObject{};

    struct GridBrush {
        int snapX;
        int snapY;
        QBrush brush;
    };

    std::optional<GridBrush> m_gridBrush;

    QPalette::ColorRole m_gridRole{QPalette::Dark};

    ProjectTreeModel *m_projectTreeModel{};

    struct DraggedObject {
        std::reference_wrapper<const Object> object;
        QPoint pos;
    };
    std::optional<DraggedObject> m_draggedObject;
};

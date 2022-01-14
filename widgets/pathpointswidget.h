#pragma once

#include <QWidget>
#include <QPalette>

#include <vector>
#include <optional>

#include "projectcontainer.h"

class PathPointsWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(int gridX READ gridX WRITE setGridX NOTIFY gridXChanged)
    Q_PROPERTY(int gridY READ gridY WRITE setGridY NOTIFY gridYChanged)
    Q_PROPERTY(bool closed READ closed WRITE setClosed NOTIFY closedChanged)

public:
    explicit PathPointsWidget(QWidget *parent = nullptr);
    explicit PathPointsWidget(std::vector<Path::Point> *points, QWidget *parent = nullptr);

    void setPoints(std::vector<Path::Point> *points);

    bool showGrid() const { return m_showGrid; }
    void setShowGrid(bool showGrid);

    int gridX() const { return m_gridX; }
    void setGridX(int gridX);

    int gridY() const { return m_gridY; }
    void setGridY(int gridY);

    bool closed() const { return m_closed; }
    void setClosed(bool closed);

    const std::optional<std::size_t> &selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(const std::optional<std::size_t> &selectedIndex);

    QPalette::ColorRole gridRole() const { return m_gridRole; }
    void setGridRole(QPalette::ColorRole gridRole) { if (gridRole == m_gridRole) return; m_gridRole = gridRole; update(); }

signals:
    void showGridChanged(bool showGrid);
    void gridXChanged(int gridX);
    void gridYChanged(int gridY);
    void closedChanged(bool closed);
    void selectedIndexChanged(const std::optional<std::size_t> &selectedIndex);

    void cursorMoved(const QPoint &point);

    void pointInserted(std::size_t index);
    void pointMoved(std::size_t index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint snapPoint(const QPoint &point) const;

    std::vector<Path::Point> *m_points{};

    bool m_showGrid{true};
    int m_gridX{16};
    int m_gridY{16};

    bool m_closed{true};

    std::optional<std::size_t> m_selectedIndex;
    std::optional<std::size_t> m_dragIndex;

    struct GridBrush {
        int gridX;
        int gridY;
        QBrush brush;
    };

    std::optional<GridBrush> m_gridBrush;

    QPalette::ColorRole m_gridRole{QPalette::Dark};
};

#pragma once

#include <QWidget>
#include <QPalette>

#include <vector>
#include <optional>

#include "projectcontainer.h"

class PathPointsWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int snapX READ snapX WRITE setSnapX NOTIFY snapXChanged)
    Q_PROPERTY(int snapY READ snapY WRITE setSnapY NOTIFY snapYChanged)
    Q_PROPERTY(bool gridEnabled READ gridEnabled WRITE setGridEnabled NOTIFY gridEnabledChanged)
    Q_PROPERTY(bool closed READ closed WRITE setClosed NOTIFY closedChanged)

public:
    explicit PathPointsWidget(QWidget *parent = nullptr);
    explicit PathPointsWidget(std::vector<Path::Point> *points, QWidget *parent = nullptr);

    void setPoints(std::vector<Path::Point> *points);

    int snapX() const { return m_snapX; }
    void setSnapX(int snapX);

    int snapY() const { return m_snapY; }
    void setSnapY(int snapY);

    bool gridEnabled() const { return m_gridEnabled; }
    void setGridEnabled(bool gridEnabled);

    bool closed() const { return m_closed; }
    void setClosed(bool closed);

    const std::optional<std::size_t> &selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(const std::optional<std::size_t> &selectedIndex);

    QPalette::ColorRole gridRole() const { return m_gridRole; }
    void setGridRole(QPalette::ColorRole gridRole);

signals:
    void snapXChanged(int snapX);
    void snapYChanged(int snapY);
    void gridEnabledChanged(bool gridEnabled);
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

    int m_snapX{16};
    int m_snapY{16};
    bool m_gridEnabled{true};

    bool m_closed{true};

    std::optional<std::size_t> m_selectedIndex;
    std::optional<std::size_t> m_dragIndex;

    struct GridBrush {
        int snapX;
        int snapY;
        QBrush brush;
    };

    std::optional<GridBrush> m_gridBrush;

    QPalette::ColorRole m_gridRole{QPalette::Dark};
};

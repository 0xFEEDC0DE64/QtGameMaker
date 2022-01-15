#pragma once

#include <QAbstractTableModel>

#include <vector>
#include <optional>

#include "projectcontainer.h"

class PathPointsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PathPointsModel(std::vector<Path::Point> &points, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex{}) const override;
    int columnCount(const QModelIndex &parent = QModelIndex{}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex{}) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex{}) override;

    std::optional<Path::Point> getPoint(const QModelIndex &index) const;
    std::optional<Path::Point> getPoint(int row) const;

    bool insertPoint(std::size_t index, Path::Point point);
    bool updatePoint(std::size_t index, Path::Point point);
    bool removePoint(std::size_t index);

public slots:
    void pointInserted(std::size_t index);
    void pointMoved(std::size_t index);

private:
    std::vector<Path::Point> &m_points;
};

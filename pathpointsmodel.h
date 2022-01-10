#pragma once

#include <QAbstractTableModel>

#include <vector>

#include "projectcontainer.h"

class PathPointsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PathPointsModel(std::vector<Path::Point> &points, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Path::Point getPoint(const QModelIndex &index) const;

public slots:
    void pointInserted(std::size_t index);
    void pointMoved(std::size_t index);

private:
    std::vector<Path::Point> &m_points;
};

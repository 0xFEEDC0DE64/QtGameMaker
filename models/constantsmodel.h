#pragma once

#include <QAbstractTableModel>

struct ProjectContainer;

class ConstantsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ConstantsModel(ProjectContainer &project, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    ProjectContainer &m_project;
};

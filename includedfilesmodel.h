#pragma once

#include <QAbstractListModel>

struct ProjectContainer;

class IncludedFilesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IncludedFilesModel(ProjectContainer &project, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    ProjectContainer &m_project;
};

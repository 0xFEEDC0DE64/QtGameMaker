#pragma once

#include <QAbstractListModel>
#include <QPixmap>

#include <vector>

class SpritesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SpritesModel(const std::vector<QPixmap> &pixmaps, QObject *parent = nullptr);
    explicit SpritesModel(std::vector<QPixmap> &&pixmaps, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    const std::vector<QPixmap> &pixmaps() const { return m_pixmaps; }
    void setPixmaps(const std::vector<QPixmap> &pixmaps);
    void setPixmaps(std::vector<QPixmap> &&pixmaps);

    auto size() const { return m_pixmaps.size(); }
    auto empty() const { return m_pixmaps.empty(); }
    const auto &front() const { return m_pixmaps.front(); }

    const QPixmap &pixmap(const QModelIndex &index) const;
    const QPixmap &pixmap(int row) const;
    void setPixmap(const QPixmap &pixmap, const QModelIndex &index);
    void setPixmap(QPixmap &&pixmap, const QModelIndex &index);
    void setPixmap(const QPixmap &pixmap, int row);
    void setPixmap(QPixmap &&pixmap, int row);

    void insertPixmap(const QPixmap &pixmap, std::size_t index);
    void insertPixmap(QPixmap &&pixmap, std::size_t index);

private:
    std::vector<QPixmap> m_pixmaps;
};

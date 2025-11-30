#include "spritesmodel.h"

#include <QPixmap>
#include <QDebug>

SpritesModel::SpritesModel(const std::vector<QPixmap> &pixmaps, QObject *parent) :
    QAbstractListModel{parent},
    m_pixmaps{pixmaps}
{
}

SpritesModel::SpritesModel(std::vector<QPixmap> &&pixmaps, QObject *parent) :
    QAbstractListModel{parent},
    m_pixmaps{std::move(pixmaps)}
{
}

int SpritesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_pixmaps.size();
}

QVariant SpritesModel::data(const QModelIndex &index, int role) const
{
    if (index.column() != 0)
        return {};

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return tr("image %0").arg(index.row());
    case Qt::DecorationRole:
        if (index.row() < 0 || std::size_t(index.row()) >= m_pixmaps.size())
        {
            qWarning() << "invalid row" << index.row();
            return {};
        }

        const auto &pixmap = m_pixmaps.at(index.row());
        if (pixmap.isNull())
        {
            QPixmap pixmap{32, 32};
            pixmap.fill(Qt::white);
            return pixmap;
        }
        return pixmap;
    }

    return {};
}

bool SpritesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (row < 0 || std::size_t(row) >= m_pixmaps.size())
    {
        qWarning() << "unexpected row" << row;
        return false;
    }

    if (count < 0 || std::size_t(count) > m_pixmaps.size() - row)
    {
        qWarning() << "unexpected row+count" << count << row;
        return false;
    }

    beginRemoveRows({}, row, row + count - 1);
    auto begin = std::next(std::begin(m_pixmaps), row);
    auto end = std::next(begin, count);
    m_pixmaps.erase(begin, end);
    endRemoveRows();

    return true;
}

bool SpritesModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent.isValid())
        return false;
    if (destinationParent.isValid())
        return false;
    if (count != 1)
        return false;

    beginMoveRows({}, sourceRow, sourceRow, {}, destinationChild);
    auto first  = m_pixmaps.begin();
    auto src    = first + sourceRow;
    auto dst    = first + destinationChild;

    if (src < dst)
        std::rotate(src, src + 1, dst);
    else if (dst < src)
        std::rotate(dst, src, src + 1);
    endMoveRows();

    return true;
}

void SpritesModel::setPixmaps(const std::vector<QPixmap> &pixmaps)
{
    beginResetModel();
    m_pixmaps = pixmaps;
    endResetModel();
}

void SpritesModel::setPixmaps(std::vector<QPixmap> &&pixmaps)
{
    beginResetModel();
    m_pixmaps = std::move(pixmaps);
    endResetModel();
}

const QPixmap &SpritesModel::pixmap(const QModelIndex &index) const
{
    if (!index.isValid())
        qFatal("invalid index");
    return pixmap(index.row());
}

const QPixmap &SpritesModel::pixmap(int row) const
{
    if (row < 0 || std::size_t(row) >= m_pixmaps.size())
        qFatal("row %i out of range", row);
    return m_pixmaps.at(row);
}

void SpritesModel::setPixmap(const QPixmap &pixmap, const QModelIndex &index)
{
    m_pixmaps[index.row()] = pixmap;
    emit dataChanged(index, index, {Qt::DecorationRole});
}

void SpritesModel::setPixmap(QPixmap &&pixmap, const QModelIndex &index)
{
    m_pixmaps[index.row()] = std::move(pixmap);
    emit dataChanged(index, index, {Qt::DecorationRole});
}

void SpritesModel::setPixmap(const QPixmap &pixmap, int row)
{
    const auto index = createIndex(row, 0);
    m_pixmaps[index.row()] = pixmap;
    emit dataChanged(index, index, {Qt::DecorationRole});
}

void SpritesModel::setPixmap(QPixmap &&pixmap, int row)
{
    const auto index = createIndex(row, 0);
    m_pixmaps[index.row()] = std::move(pixmap);
    emit dataChanged(index, index, {Qt::DecorationRole});
}

void SpritesModel::insertPixmap(const QPixmap &pixmap, std::size_t index)
{
    if (index > m_pixmaps.size())
        qFatal("index out of range");

    beginInsertRows({}, index, index);
    m_pixmaps.emplace(std::begin(m_pixmaps) + index, pixmap);
    endInsertRows();
}

void SpritesModel::insertPixmap(QPixmap &&pixmap, std::size_t index)
{
    if (index > m_pixmaps.size())
        qFatal("index out of range");

    beginInsertRows({}, index, index);
    m_pixmaps.emplace(std::begin(m_pixmaps) + index, std::move(pixmap));
    endInsertRows();
}

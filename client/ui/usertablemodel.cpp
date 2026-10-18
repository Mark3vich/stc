#include "usertablemodel.h"

#include "core/usercache.h"

UserTableModel::UserTableModel(UserCache *cache, QObject *parent)
    : QAbstractTableModel(parent)
    , m_cache(cache)
{
    connect(m_cache, &UserCache::changed, this, &UserTableModel::onCacheChanged);
}

int UserTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_cache->count();
}

int UserTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 3;
}

QVariant UserTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    const QVector<User> &users = m_cache->all();
    if (index.row() < 0 || index.row() >= users.size())
        return {};

    const User &user = users.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case 0: return user.id;
                case 1: return user.username;
                case 2: return user.email;
            }
            break;
        case Qt::TextAlignmentRole:
            if (index.column() == 0)
                return int(Qt::AlignCenter);
            break;
        case Qt::ToolTipRole:
            return user.email;
    }
    return {};
}

QVariant UserTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (section) {
        case 0: return tr("Идентификатор");
        case 1: return tr("Имя пользователя");
        case 2: return tr("Электронная почта");
    }
    return {};
}

void UserTableModel::onCacheChanged()
{
    beginResetModel();
    endResetModel();
}
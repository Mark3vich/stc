#include "usercache.h"

const QVector<User> &UserCache::all() const
{
    return m_users;
}

int UserCache::count() const
{
    return m_users.size();
}

void UserCache::replaceAll(const QVector<User> &users)
{
    m_users = users;
    emit changed();
}

void UserCache::clear()
{
    if (m_users.isEmpty())
        return;
    m_users.clear();
    emit changed();
}
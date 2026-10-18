#pragma once

#include "user.h"

#include <QObject>
#include <QVector>

class UserCache : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    const QVector<User> &all() const;
    int count() const;

public slots:
    void replaceAll(const QVector<User> &users);
    void clear();

signals:
    void changed();

private:
    QVector<User> m_users;
};

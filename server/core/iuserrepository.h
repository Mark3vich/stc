#pragma once

#include "user.h"

#include <QString>
#include <QVector>
#include <optional>

class IUserRepository
{
public:
    virtual ~IUserRepository() = default;

    virtual std::optional<User> addUser(const QString &username, const QString &email) = 0;
    virtual QVector<User> getAllUsers() = 0;
    virtual bool emailExists(const QString &email) = 0;

    virtual void closeThreadConnection() = 0;
};

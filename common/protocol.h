#pragma once

#include "user.h"

#include <QJsonObject>
#include <QString>
#include <QVector>

namespace protocol {

struct Request
{
    enum class Type { AddUser, GetUsers, Unknown };

    Type type = Type::Unknown;
    QString username;
    QString email;

    static Request fromJson(const QJsonObject &obj);
};

QJsonObject makeAddUser(const QString &username, const QString &email);
QJsonObject makeGetUsers();

struct Response
{
    enum class Status { Success, Error, Unknown };

    Status status = Status::Unknown;
    QString message;
    QVector<User> users;
    bool hasUserList = false;
};

Response parseResponse(const QJsonObject &obj);

QJsonObject successAddedUser(const QString &message = QStringLiteral("Пользователь успешно добавлен"));
QJsonObject successUsers(const QVector<User> &users);
QJsonObject error(const QString &message);

}

#include "protocol.h"

#include <QJsonArray>
#include <QJsonValue>

namespace protocol {

Request Request::fromJson(const QJsonObject &obj)
{
    Request request;
    const QString action = obj.value(QStringLiteral("action")).toString();

    if (action == QLatin1String("add_user")) {
        request.type = Type::AddUser;
        request.username = obj.value(QStringLiteral("username")).toString().trimmed();
        request.email = obj.value(QStringLiteral("email")).toString().trimmed();
    } else if (action == QLatin1String("get_users")) {
        request.type = Type::GetUsers;
    }
    return request;
}

QJsonObject makeAddUser(const QString &username, const QString &email)
{
    return {
        { QStringLiteral("action"), QStringLiteral("add_user") },
        { QStringLiteral("username"), username },
        { QStringLiteral("email"), email }
    };
}

QJsonObject makeGetUsers()
{
    return { { QStringLiteral("action"), QStringLiteral("get_users") } };
}

Response parseResponse(const QJsonObject &obj)
{
    Response response;

    const QString status = obj.value(QStringLiteral("status")).toString();
    if (status == QLatin1String("success"))
        response.status = Response::Status::Success;
    else if (status == QLatin1String("error"))
        response.status = Response::Status::Error;

    response.message = obj.value(QStringLiteral("message")).toString();

    const QJsonValue usersValue = obj.value(QStringLiteral("users"));
    if (usersValue.isArray()) {
        response.hasUserList = true;
        const QJsonArray usersArray = usersValue.toArray();
        response.users.reserve(usersArray.size());
        for (const QJsonValue &value : usersArray) {
            const QJsonObject jsonUser = value.toObject();
            User user;
            user.id = jsonUser.value(QStringLiteral("id")).toInt();
            user.username = jsonUser.value(QStringLiteral("username")).toString();
            user.email = jsonUser.value(QStringLiteral("email")).toString();
            response.users.append(user);
        }
    }

    return response;
}

QJsonObject successAddedUser(const QString &message)
{
    return {
        { QStringLiteral("status"), QStringLiteral("success") },
        { QStringLiteral("message"), message }
    };
}

QJsonObject successUsers(const QVector<User> &users)
{
    QJsonArray array;
    for (const User &user : users) {
        array.append(QJsonObject{
            { QStringLiteral("id"), user.id },
            { QStringLiteral("username"), user.username },
            { QStringLiteral("email"), user.email }
        });
    }
    return {
        { QStringLiteral("status"), QStringLiteral("success") },
        { QStringLiteral("users"), array }
    };
}

QJsonObject error(const QString &message)
{
    return {
        { QStringLiteral("status"), QStringLiteral("error") },
        { QStringLiteral("message"), message }
    };
}

} 

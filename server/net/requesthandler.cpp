#include "requesthandler.h"

#include "core/iuserrepository.h"

#include <QDebug>

RequestHandler::RequestHandler(IUserRepository *repo)
    : m_repo(repo)
{
}

QJsonObject RequestHandler::handle(const protocol::Request &request)
{
    switch (request.type) {
        case protocol::Request::Type::AddUser:
            return handleAddUser(request);
        case protocol::Request::Type::GetUsers:
            return handleGetUsers();
        case protocol::Request::Type::Unknown:
            break;
    }

    qWarning() << "Unknown action received";
    return protocol::error(QStringLiteral("Unknown action"));
}

QJsonObject RequestHandler::handleAddUser(const protocol::Request &request)
{
    if (request.username.isEmpty() || request.email.isEmpty()) {
        qWarning() << "Add user request with empty fields";
        return protocol::error(QStringLiteral("Username and email are required"));
    }

    const auto added = m_repo->addUser(request.username, request.email);
    if (!added) {
        if (m_repo->emailExists(request.email)) {
            qWarning() << "Email already taken:" << request.email;
            return protocol::error(QStringLiteral("A user with this email already exists"));
        }
        return protocol::error(QStringLiteral("Failed to add user"));
    }

    qInfo() << "User added:" << added->username << "<" << added->email << "> (id=" << added->id << ")";
    return protocol::successAddedUser();
}

QJsonObject RequestHandler::handleGetUsers()
{
    const auto users = m_repo->getAllUsers();
    qInfo() << "Users list sent:" << users.size();
    return protocol::successUsers(users);
}
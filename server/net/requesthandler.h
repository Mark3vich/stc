#pragma once

#include "protocol.h"

#include <QJsonObject>

class IUserRepository;

class RequestHandler
{
public:
    explicit RequestHandler(IUserRepository *repo);

    QJsonObject handle(const protocol::Request &request);

private:
    QJsonObject handleAddUser(const protocol::Request &request);
    QJsonObject handleGetUsers();

    IUserRepository *m_repo = nullptr;
};
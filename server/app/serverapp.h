#pragma once

#include <QObject>

class Server;
class IUserRepository;

class ServerApp : public QObject
{
    Q_OBJECT
public:
    explicit ServerApp(QObject *parent = nullptr);
    ~ServerApp() override;

    bool start(const QString &dbPath, quint16 port);

private:
    IUserRepository *m_repo = nullptr;
    Server *m_server = nullptr;
};
#pragma once

#include <QList>
#include <QTcpServer>

class IUserRepository;
class ClientSession;
class QThread;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(IUserRepository *repo, QObject *parent = nullptr);
    ~Server() override;

    bool start(quint16 port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onSessionFinished();

private:
    struct SessionEntry {
        ClientSession *session = nullptr;
        QThread *thread = nullptr;
    };

    IUserRepository *m_repo = nullptr;
    QList<SessionEntry> m_sessions;
};
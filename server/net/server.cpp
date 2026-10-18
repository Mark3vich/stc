#include "server.h"

#include "clientsession.h"

#include <QDebug>
#include <QHostAddress>
#include <QMetaObject>
#include <QThread>

Server::Server(IUserRepository *repo, QObject *parent)
    : QTcpServer(parent)
    , m_repo(repo)
{
}

Server::~Server()
{
    for (const SessionEntry &entry : m_sessions) {
        if (entry.session) {
            QMetaObject::invokeMethod(entry.session, "stop", Qt::BlockingQueuedConnection);
        }
    }
    
    for (const SessionEntry &entry : m_sessions) {
        if (entry.thread && entry.thread->isRunning()) {
            entry.thread->quit();
            if (!entry.thread->wait(3000)) {
                qWarning() << "Session thread did not finish in time, terminating";
                entry.thread->terminate();
                entry.thread->wait(500);
            }
        }
    }
    m_sessions.clear();
}

bool Server::start(quint16 port)
{
    if (!listen(QHostAddress::Any, port)) {
        qCritical() << "Failed to open port" << port << ":" << errorString();
        return false;
    }
    qInfo() << "Server listening on port" << port;
    return true;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    auto *thread = new QThread(this);
    auto *session = new ClientSession(socketDescriptor, m_repo);
    session->moveToThread(thread);

    connect(thread, &QThread::started, session, &ClientSession::start);
    connect(session, &ClientSession::finished, thread, &QThread::quit);
    connect(session, &ClientSession::finished, this, &Server::onSessionFinished);
    connect(thread, &QThread::finished, session, &QObject::deleteLater);

    m_sessions.append({ session, thread });
    qInfo() << "New client session";
    thread->start();
}

void Server::onSessionFinished()
{
    auto *session = qobject_cast<ClientSession *>(sender());
    if (!session)
        return;

    for (int i = 0; i < m_sessions.size(); ++i) {
        if (m_sessions[i].session != session)
            continue;

        QThread *thread = m_sessions[i].thread;
        m_sessions.removeAt(i);
        if (thread) {
            thread->quit();
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        }
        break;
    }
}
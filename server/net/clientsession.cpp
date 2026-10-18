#include "clientsession.h"

#include "core/iuserrepository.h"
#include "protocol.h"
#include "requesthandler.h"

#include <QAbstractSocket>
#include <QDebug>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QTcpSocket>

namespace {
constexpr int kMaxBufferSize = 1024 * 1024; // 1 MB
}

ClientSession::ClientSession(qintptr socketDescriptor,
                             IUserRepository *repo,
                             QObject *parent)
    : QObject(parent)
    , m_socketDescriptor(socketDescriptor)
    , m_repo(repo)
{
}

void ClientSession::start()
{
    m_socket = new QTcpSocket(this);
    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        qCritical() << "Failed to set socket descriptor:" << m_socket->errorString();
        finish();
        return;
    }

    connect(m_socket, &QTcpSocket::readyRead, this, &ClientSession::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientSession::onDisconnected);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &ClientSession::onSocketError);

    qInfo() << "Client connected:" << m_socket->peerAddress().toString() << ":" << m_socket->peerPort();
}

void ClientSession::stop()
{
    if (m_socket) {
        if (m_socket->state() == QAbstractSocket::ConnectedState) {
            m_socket->disconnectFromHost();
            if (m_socket->state() != QAbstractSocket::UnconnectedState)
                m_socket->waitForDisconnected(1000);
        }
    }
    finish();
}

void ClientSession::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    if (m_buffer.size() > kMaxBufferSize) {
        qWarning() << "Buffer overflow, disconnecting client";
        m_socket->disconnectFromHost();
        return;
    }

    int idx = 0;
    while ((idx = m_buffer.indexOf('\n')) != -1) {
        const QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);
        if (!line.isEmpty())
            processLine(line);
    }
}

void ClientSession::processLine(const QByteArray &line)
{
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Invalid JSON:" << err.errorString();
        sendJson(protocol::error(QStringLiteral("Invalid JSON")));
        return;
    }

    const protocol::Request request = protocol::Request::fromJson(doc.object());
    RequestHandler handler(m_repo);
    sendJson(handler.handle(request));
}

void ClientSession::sendJson(const QJsonObject &obj)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray payload = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    payload.append('\n');
    m_socket->write(payload);
}

void ClientSession::onDisconnected()
{
    qInfo() << "Client disconnected";
    finish();
}

void ClientSession::onSocketError()
{
    if (m_socket->error() == QAbstractSocket::RemoteHostClosedError)
        return;

    qCritical() << "Socket error:" << m_socket->errorString();
    finish();
}

void ClientSession::finish()
{
    if (m_finished)
        return;
    m_finished = true;

    if (m_repo)
        m_repo->closeThreadConnection();

    emit finished();
}
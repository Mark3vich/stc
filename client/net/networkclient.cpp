#include "networkclient.h"
#include "protocol.h"

#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QTcpSocket>
#include <QTimer>

NetworkClient::NetworkClient(QObject *parent): QObject(parent), m_socket(new QTcpSocket(this)), m_reconnectTimer(new QTimer(this))
{
    m_reconnectTimer->setSingleShot(true);
    m_reconnectTimer->setInterval(3000);

    connect(m_socket, &QTcpSocket::connected, this, &NetworkClient::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkClient::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::onSocketReadyRead);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &NetworkClient::onSocketError);
    connect(m_reconnectTimer, &QTimer::timeout, this, &NetworkClient::onReconnectTimeout);
}

void NetworkClient::connectTo(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
    m_manualDisconnect = false;
    m_reconnectTimer->stop();

    if (m_socket->state() != QAbstractSocket::UnconnectedState)
        m_socket->abort();

    m_socket->connectToHost(host, port);
}

void NetworkClient::disconnectFromHost()
{
    m_manualDisconnect = true;
    m_reconnectTimer->stop();
    m_socket->disconnectFromHost();
}

bool NetworkClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkClient::addUser(const QString &username, const QString &email)
{
    sendObject(protocol::makeAddUser(username, email));
}

void NetworkClient::getUsers()
{
    sendObject(protocol::makeGetUsers());
}

void NetworkClient::sendObject(const QJsonObject &obj)
{
    if (!isConnected()) {
        emit transportError(tr("Нет соединения с сервером"));
        return;
    }

    QByteArray payload = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    payload.append('\n');
    m_socket->write(payload);
}

void NetworkClient::onSocketConnected()
{
    m_buffer.clear();
    m_reconnectTimer->stop();
    emit connected();
}

void NetworkClient::onSocketDisconnected()
{
    emit disconnected();
    scheduleReconnect();
}

void NetworkClient::onSocketReadyRead()
{
    m_buffer.append(m_socket->readAll());

    int idx = 0;
    while ((idx = m_buffer.indexOf('\n')) != -1) {
        const QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);
        if (!line.isEmpty())
            processLine(line);
    }
}

void NetworkClient::onSocketError()
{
    emit transportError(m_socket->errorString());
    scheduleReconnect();
}

void NetworkClient::onReconnectTimeout()
{
    if (m_manualDisconnect || m_host.isEmpty() || m_port == 0)
        return;
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
        return;

    m_socket->connectToHost(m_host, m_port);
}

void NetworkClient::scheduleReconnect()
{
    if (m_manualDisconnect || m_host.isEmpty() || m_port == 0)
        return;
    if (!m_reconnectTimer->isActive())
        m_reconnectTimer->start();
}

void NetworkClient::processLine(const QByteArray &line)
{
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        emit protocolError(tr("Некорректный ответ сервера: %1").arg(err.errorString()));
        return;
    }

    const protocol::Response response = protocol::parseResponse(doc.object());
    switch (response.status) {
        case protocol::Response::Status::Success:
            if (response.hasUserList)
                emit usersReceived(response.users);
            else
                emit userAdded(response.message);
            break;
        case protocol::Response::Status::Error:
            emit serverError(response.message);
            break;
        default:
            emit protocolError(tr("Неизвестный статус ответа сервера"));
            break;
        }
}

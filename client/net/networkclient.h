#pragma once

#include "user.h"

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVector>

class QTcpSocket;
class QTimer;

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    explicit NetworkClient(QObject *parent = nullptr);

    void connectTo(const QString &host, quint16 port);
    void disconnectFromHost();
    bool isConnected() const;

    void addUser(const QString &username, const QString &email);
    void getUsers();

signals:
    void connected();
    void disconnected();
    void transportError(const QString &message);
    void userAdded(const QString &message);
    void usersReceived(const QVector<User> &users);
    void serverError(const QString &message);
    void protocolError(const QString &message);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketError();
    void onReconnectTimeout();

private:
    void sendObject(const QJsonObject &obj);
    void processLine(const QByteArray &line);
    void scheduleReconnect();

    QTcpSocket *m_socket = nullptr;
    QTimer *m_reconnectTimer = nullptr;
    QByteArray m_buffer;
    QString m_host;
    quint16 m_port = 0;
    bool m_manualDisconnect = false;
};

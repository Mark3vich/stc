#pragma once

#include "user.h"

#include <QObject>
#include <QVector>

class NetworkClient;
class UserCache;

class AppController : public QObject
{
    Q_OBJECT
public:
    AppController(NetworkClient *net, UserCache *cache, QObject *parent = nullptr);

    void start(const QString &host, quint16 port);

signals:
    void connectionStateChanged(bool connected);
    void addSucceeded();
    void infoMessage(const QString &text);
    void errorMessage(const QString &text);

public slots:
    void requestAddUser(const QString &username, const QString &email);
    void requestRefresh();

private slots:
    void onConnected();
    void onDisconnected();
    void onTransportError(const QString &msg);
    void onUserAdded(const QString &msg);
    void onUsersReceived(const QVector<User> &users);
    void onServerError(const QString &msg);
    void onProtocolError(const QString &msg);

private:
    bool validate(const QString &username, const QString &email, QString *errorOut) const;

    NetworkClient *m_net = nullptr;
    UserCache *m_cache = nullptr;
};
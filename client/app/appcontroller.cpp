#include "appcontroller.h"

#include "core/usercache.h"
#include "net/networkclient.h"

#include <QRegularExpression>

AppController::AppController(NetworkClient *net, UserCache *cache, QObject *parent)
    : QObject(parent), m_net(net), m_cache(cache)
{
    connect(m_net, &NetworkClient::connected, this, &AppController::onConnected);
    connect(m_net, &NetworkClient::disconnected, this, &AppController::onDisconnected);
    connect(m_net, &NetworkClient::transportError, this, &AppController::onTransportError);
    connect(m_net, &NetworkClient::userAdded, this, &AppController::onUserAdded);
    connect(m_net, &NetworkClient::usersReceived, this, &AppController::onUsersReceived);
    connect(m_net, &NetworkClient::serverError, this, &AppController::onServerError);
    connect(m_net, &NetworkClient::protocolError, this, &AppController::onProtocolError);
}

void AppController::start(const QString &host, quint16 port)
{
    emit infoMessage(tr("Подключение к серверу %1:%2…").arg(host).arg(port));
    m_net->connectTo(host, port);
}

void AppController::requestAddUser(const QString &username, const QString &email)
{
    QString error;
    if (!validate(username, email, &error)) {
        emit errorMessage(error);
        return;
    }
    m_net->addUser(username, email);
}

void AppController::requestRefresh()
{
    m_net->getUsers();
}

void AppController::onConnected()
{
    emit connectionStateChanged(true);
    emit infoMessage(tr("Соединение с сервером установлено"));
    requestRefresh();
}

void AppController::onDisconnected()
{
    emit connectionStateChanged(false);
    emit infoMessage(tr("Соединение с сервером закрыто. Повторная попытка через несколько секунд…"));
}

void AppController::onTransportError(const QString &msg)
{
    emit errorMessage(tr("Ошибка сети: %1").arg(msg));
}

void AppController::onUserAdded(const QString &msg)
{
    const QString text = msg.isEmpty() ? tr("Пользователь добавлен") : msg;
    emit infoMessage(text);
    emit addSucceeded();
    requestRefresh();
}

void AppController::onUsersReceived(const QVector<User> &users)
{
    m_cache->replaceAll(users);
    emit infoMessage(tr("Список пользователей обновлён (%1)").arg(users.size()));
}

void AppController::onServerError(const QString &msg)
{
    emit errorMessage(msg);
}

void AppController::onProtocolError(const QString &msg)
{
    emit errorMessage(msg);
}

bool AppController::validate(const QString &username, const QString &email, QString *errorOut) const
{
    if (username.trimmed().isEmpty()) {
        if (errorOut)
            *errorOut = tr("Введите имя пользователя");
        return false;
    }

    static const QRegularExpression emailRe(
        QStringLiteral(R"(^[A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}$)"));
    if (!emailRe.match(email.trimmed()).hasMatch()) {
        if (errorOut)
            *errorOut = tr("Введите корректный адрес электронной почты");
        return false;
    }
    return true;
}
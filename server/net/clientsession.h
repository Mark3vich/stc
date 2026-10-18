#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QObject>

class QTcpSocket;
class IUserRepository;

class ClientSession : public QObject
{
    Q_OBJECT
public:
    ClientSession(qintptr socketDescriptor,
                  IUserRepository *repo,
                  QObject *parent = nullptr);

public slots:
    void start();
    void stop();

signals:
    void finished();

private slots:
    void onReadyRead();
    void onDisconnected();
    void onSocketError();

private:
    void processLine(const QByteArray &line);
    void sendJson(const QJsonObject &obj);
    void finish();

    qintptr m_socketDescriptor = 0;
    IUserRepository *m_repo = nullptr;
    QTcpSocket *m_socket = nullptr;
    QByteArray m_buffer;
    bool m_finished = false;
};
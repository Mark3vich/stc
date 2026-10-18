#pragma once

#include "core/iuserrepository.h"

#include <QMutex>
#include <QSqlDatabase>
#include <QString>

class SqliteRepository : public IUserRepository
{
public:
    explicit SqliteRepository(const QString &dbPath);
    ~SqliteRepository() override;

    bool open();

    std::optional<User> addUser(const QString &username, const QString &email) override;
    QVector<User> getAllUsers() override;
    bool emailExists(const QString &email) override;
    void closeThreadConnection() override;

private:
    QSqlDatabase databaseForCurrentThread(bool *ok);
    bool ensureSchema(QSqlDatabase &db);
    static QString connectionNameForCurrentThread();

    QString m_dbPath;
    QMutex m_mutex;
};
#include "sqliterepository.h"

#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QThread>
#include <QVariant>

SqliteRepository::SqliteRepository(const QString &dbPath)
    : m_dbPath(dbPath)
{
}

SqliteRepository::~SqliteRepository()
{
    closeThreadConnection();
}

bool SqliteRepository::open()
{
    bool ok = false;
    databaseForCurrentThread(&ok);
    return ok;
}

QString SqliteRepository::connectionNameForCurrentThread()
{
    return QStringLiteral("sqlite_repo_%1")
        .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
}

QSqlDatabase SqliteRepository::databaseForCurrentThread(bool *ok)
{
    const QString connName = connectionNameForCurrentThread();

    QSqlDatabase db;
    if (QSqlDatabase::contains(connName)) {
        db = QSqlDatabase::database(connName);
    } else {
        db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connName);
        db.setDatabaseName(m_dbPath);
    }

    if (!db.isOpen() && !db.open()) {
        if (ok) *ok = false;
        return db;
    }

    if (!ensureSchema(db)) {
        if (ok) *ok = false;
        return db;
    }

    if (ok) *ok = true;
    return db;
}

bool SqliteRepository::ensureSchema(QSqlDatabase &db)
{
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("PRAGMA journal_mode=WAL")))
        return false;
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS users ("
            "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  username TEXT NOT NULL,"
            "  email TEXT NOT NULL UNIQUE)"))) {
        return false;
    }
    return true;
}

std::optional<User> SqliteRepository::addUser(const QString &username, const QString &email)
{
    QMutexLocker lock(&m_mutex);

    bool ok = false;
    QSqlDatabase db = databaseForCurrentThread(&ok);
    if (!ok)
        return std::nullopt;

    QSqlQuery query(db);
    query.prepare(QStringLiteral("INSERT INTO users (username, email) VALUES (:u, :e)"));
    query.bindValue(QStringLiteral(":u"), username);
    query.bindValue(QStringLiteral(":e"), email);
    if (!query.exec())
        return std::nullopt;

    User user;
    user.id = query.lastInsertId().toInt();
    user.username = username;
    user.email = email;
    return user;
}

QVector<User> SqliteRepository::getAllUsers()
{
    QMutexLocker lock(&m_mutex);

    QVector<User> result;
    bool ok = false;
    QSqlDatabase db = databaseForCurrentThread(&ok);
    if (!ok)
        return result;

    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("SELECT id, username, email FROM users ORDER BY id")))
        return result;

    while (query.next()) {
        result.append({
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString()
        });
    }
    return result;
}

bool SqliteRepository::emailExists(const QString &email)
{
    QMutexLocker lock(&m_mutex);

    bool ok = false;
    QSqlDatabase db = databaseForCurrentThread(&ok);
    if (!ok)
        return false;

    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT 1 FROM users WHERE email = :e LIMIT 1"));
    query.bindValue(QStringLiteral(":e"), email);
    return query.exec() && query.next();
}

void SqliteRepository::closeThreadConnection()
{
    const QString connName = connectionNameForCurrentThread();
    if (!QSqlDatabase::contains(connName))
        return;

    {
        QSqlDatabase db = QSqlDatabase::database(connName, false);
        if (db.isOpen())
            db.close();
    }
    QSqlDatabase::removeDatabase(connName);
}
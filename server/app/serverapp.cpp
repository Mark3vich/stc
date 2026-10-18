#include "serverapp.h"

#include "db/sqliterepository.h"
#include "net/server.h"

#include <QDebug>

ServerApp::ServerApp(QObject *parent)
    : QObject(parent)
{
}

ServerApp::~ServerApp()
{
    delete m_server;
    m_server = nullptr;
    delete m_repo;
    m_repo = nullptr;
}

bool ServerApp::start(const QString &dbPath, quint16 port)
{
    auto *sqliteRepo = new SqliteRepository(dbPath);
    if (!sqliteRepo->open()) {
        qCritical() << "Failed to open database:" << dbPath;
        delete sqliteRepo;
        return false;
    }
    m_repo = sqliteRepo;

    m_server = new Server(m_repo);
    if (!m_server->start(port)) {
        delete m_server;
        m_server = nullptr;
        delete m_repo;
        m_repo = nullptr;
        return false;
    }
    return true;
}
#include "app/serverapp.h"

#include <QCoreApplication>
#include <QDir>

namespace {
constexpr quint16 kServerPort = 33333;
constexpr const char *kDbFile = "users.db";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("UsersServer"));

    const QString dbPath = QDir(QCoreApplication::applicationDirPath())
                               .filePath(QString::fromLatin1(kDbFile));

    ServerApp server;
    if (!server.start(dbPath, kServerPort))
        return 1;

    return app.exec();
}
#include "app/appcontroller.h"
#include "core/usercache.h"
#include "net/networkclient.h"
#include "ui/mainwindow.h"
#include "ui/usertablemodel.h"

#include <QApplication>
#include <QLocale>

namespace {
constexpr const char *kServerHost = "127.0.0.1";
constexpr quint16 kServerPort = 33333;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));
    QApplication::setApplicationName(QStringLiteral("UsersClient"));
    QApplication::setApplicationDisplayName(QStringLiteral("Клиент пользователей"));

    UserCache cache;
    NetworkClient net;
    AppController controller(&net, &cache);
    UserTableModel model(&cache);

    MainWindow window(&controller, &model);
    window.show();

    QObject::connect(&app, &QApplication::aboutToQuit, &net, &NetworkClient::disconnectFromHost);

    controller.start(QString::fromLatin1(kServerHost), kServerPort);
    return app.exec();
}
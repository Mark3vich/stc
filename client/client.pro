QT += core gui network widgets
CONFIG += c++17
CONFIG -= debug_and_release

TEMPLATE = app
TARGET = client

INCLUDEPATH += $$PWD $$PWD/../common

SOURCES += \
    main.cpp \
    ../common/protocol.cpp \
    core/usercache.cpp \
    net/networkclient.cpp \
    app/appcontroller.cpp \
    ui/mainwindow.cpp \
    ui/usertablemodel.cpp \
    ui/widgets/userformwidget.cpp

HEADERS += \
    ../common/user.h \
    ../common/protocol.h \
    core/usercache.h \
    net/networkclient.h \
    app/appcontroller.h \
    ui/mainwindow.h \
    ui/usertablemodel.h \
    ui/widgets/userformwidget.h
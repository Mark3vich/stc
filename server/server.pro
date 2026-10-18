QT += core network sql
QT -= gui
CONFIG += c++17 console
CONFIG -= app_bundle debug_and_release

TEMPLATE = app
TARGET = server

INCLUDEPATH += $$PWD $$PWD/../common

QMAKE_CXXFLAGS += -Wno-sfinae-incomplete

HEADERS += \
    ../common/user.h \
    ../common/protocol.h \
    core/iuserrepository.h \
    db/sqliterepository.h \
    net/server.h \
    net/clientsession.h \
    net/requesthandler.h \
    app/serverapp.h

SOURCES += \
    main.cpp \
    ../common/protocol.cpp \
    db/sqliterepository.cpp \
    net/server.cpp \
    net/clientsession.cpp \
    net/requesthandler.cpp \
    app/serverapp.cpp

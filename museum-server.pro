QT += concurrent core network multimedia
QT -= gui

TARGET = museum-server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    telnettcpserver.cpp \
    ProjectorQuery.cpp \
    CommandController.cpp \
    ConsoleInputProcessor.cpp \
    waitScenarios.cpp

HEADERS += \
    telnettcpserver.h \
    ProjectorQuery.h \
    CommandController.h \
    ConsoleInputProcessor.h \
    waitScenarios.h


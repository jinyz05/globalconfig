#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T18:30:34
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
    LIBS += -L../
    isEqual(QT_VERSION, 4.5.3):LIBS += -lglobalconfig.4.5.3
    else:LIBS += -lglobalconfig.4.8.3


    LIBS+= -L../../features/openssl
    LIBS+=-lcrypto

SOURCES += main.cpp

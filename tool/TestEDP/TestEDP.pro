#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T18:24:42
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = TestEDP
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

INCLUDEPATH += ../../../../ \
    ../features/OneNet/EDP \
    ../features/OneNet/HTTP \
    ../features/OneNet/

LIBS += -L../globalconfig
isEqual(QT_VERSION, 4.5.3):LIBS += -lglobalconfig.4.5.3
else:LIBS += -lglobalconfig.4.8.3

LIBS+= -L../features/openssl
LIBS+=-lcrypto

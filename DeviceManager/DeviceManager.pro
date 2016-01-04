# -------------------------------------------------
# Project created by QtCreator 2015-12-22T13:06:19
# -------------------------------------------------
QT += network
TARGET = DeviceManager
TEMPLATE = app
INCLUDEPATH += ../OneNet/ \
    ../OneNet/HTTP/
DEFINES += _LINUX
SOURCES += main.cpp \
    devicelistdialog.cpp \
    ../OneNet/HTTP/httpnetworkmanager.cpp \
    ../OneNet/HTTP/httpclient.cpp \
    ../OneNet/cJSON.c \
    deviceeditdialog.cpp \
    cmddialog.cpp \
    ../OneNet/HTTP/devicetablemanager.cpp \
    ../OneNet/HTTP/rstdatapoint.cpp \
    ../OneNet/HTTP/rstdatastream.cpp \
    ../OneNet/HTTP/rstdevice.cpp \
    ../OneNet/HTTP/rstkey.cpp
HEADERS += devicelistdialog.h \
    ../OneNet/HTTP/httpnetworkmanager.h \
    ../OneNet/HTTP/httpclient_p.h \
    ../OneNet/HTTP/httpclient.h \
    ../OneNet/cJSON.h \
    deviceeditdialog.h \
    cmddialog.h \
    ../OneNet/HTTP/devicetablemanager.h \
    ../OneNet/HTTP/rstdatapoint.h \
    ../OneNet/HTTP/rstdatastream.h \
    ../OneNet/HTTP/rstdevice.h \
    ../OneNet/HTTP/rstkey.h
FORMS += devicelistdialog.ui \
    deviceeditdialog.ui \
    cmddialog.ui

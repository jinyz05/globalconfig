# -------------------------------------------------
# Project created by QtCreator 2015-12-22T13:06:19
# -------------------------------------------------
QT += network
TARGET = DeviceManager
TEMPLATE = app
INCLUDEPATH += ../../ \
    ../../HTTP/
DEFINES += _LINUX
SOURCES += main.cpp \
    devicelistdialog.cpp \
    ../../HTTP/httpnetworkmanager.cpp \
    ../../HTTP/httpclient.cpp \
    ../../cJSON.c \
    deviceeditdialog.cpp \
    cmddialog.cpp \
    ../../HTTP/devicetablemanager.cpp \
    ../../HTTP/rstdatapoint.cpp \
    ../../HTTP/rstdatastream.cpp \
    ../../HTTP/rstdevice.cpp \
    ../../HTTP/rstkey.cpp
HEADERS += devicelistdialog.h \
    ../../HTTP/httpnetworkmanager.h \
    ../../HTTP/httpclient_p.h \
    ../../HTTP/httpclient.h \
    ../../cJSON.h \
    deviceeditdialog.h \
    cmddialog.h \
    ../../HTTP/devicetablemanager.h \
    ../../HTTP/rstdatapoint.h \
    ../../HTTP/rstdatastream.h \
    ../../HTTP/rstdevice.h \
    ../../HTTP/rstkey.h
FORMS += devicelistdialog.ui \
    deviceeditdialog.ui \
    cmddialog.ui

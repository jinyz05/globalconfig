# -------------------------------------------------
# Project created by QtCreator 2014-12-17T14:32:23
# -------------------------------------------------
QT -= gui
QT += network
DEFINES += _LINUX
DEFINES += _ENCRYPT
CONFIG += no_lflags_merge
isEqual(QT_VERSION, 4.5.3):TARGET = globalconfig.4.5.3
else:TARGET = globalconfig.4.8.3
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += ../features/OneNet/ \
    ../features/OneNet/EDP \
    ../features/OneNet/HTTP \
    ../features/OneNet/
SOURCES += sharepreference.cpp \
    supperpassword.cpp \
    runshellrun.cpp \
    ../features/OneNet/recordjson.cpp \
    ../features/OneNet/cJSON.c \
    ../features/OneNet/HTTP/devicetablemanager.cpp \
    ../features/OneNet/HTTP/httpclient.cpp \
    ../features/OneNet/HTTP/httpnetworkmanager.cpp \
    ../features/OneNet/HTTP/rstdatapoint.cpp \
    ../features/OneNet/HTTP/rstdatastream.cpp \
    ../features/OneNet/HTTP/rstdevice.cpp \
    ../features/OneNet/HTTP/rstkey.cpp \
    ../features/OneNet/HTTP/rstmanager.cpp \
    ../features/OneNet/EDP/edp.cpp \
    ../features/OneNet/EDP/edpmanager.cpp \
    ../features/OneNet/EDP/recordedp.cpp \
    ../features/OneNet/EDP/EdpKit.c \
    ../features/OneNet/EDP/Openssl.c \
    ../database/tools/mybase64code.cpp
HEADERS += sharepreference.h \
    supperpassword.h \
    runshellrun.h \
    ../features/OneNet/cJSON.h \
    ../features/OneNet/Common.h \
    ../features/OneNet/recordjson.h \
    ../features/OneNet/HTTP/devicetablemanager.h \
    ../features/OneNet/HTTP/httpclient.h \
    ../features/OneNet/HTTP/httpclient_p.h \
    ../features/OneNet/HTTP/httpnetworkmanager.h \
    ../features/OneNet/HTTP/rstdatapoint.h \
    ../features/OneNet/HTTP/rstdatastream.h \
    ../features/OneNet/HTTP/rstdevice.h \
    ../features/OneNet/HTTP/rstkey.h \
    ../features/OneNet/HTTP/rstmanager.h \
    ../features/OneNet/EDP/edp.h \
    ../features/OneNet/EDP/EdpKit.h \
    ../features/OneNet/EDP/edpmanager.h \
    ../features/OneNet/EDP/recordedp.h \
    ../features/OneNet/EDP/Openssl.h \
    ../database/tools/mybase64code.h
include(../config.pri)
INCLUDEPATH += ../$${PRODUCTNAME}

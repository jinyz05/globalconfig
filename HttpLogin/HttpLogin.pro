# -------------------------------------------------
# Project created by QtCreator 2015-12-28T17:45:42
# -------------------------------------------------
QT += network
QT -= gui
TARGET = HttpLogin
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
INCLUDEPATH += ../ \
    ../HTTP/
SOURCES += main.cpp \
    ../HTTP/httpnetworkmanager.cpp \
    ../HTTP/httpclient.cpp
HEADERS += ../HTTP/httpnetworkmanager.h \
    ../HTTP/httpclient_p.h \
    ../HTTP/httpclient.h

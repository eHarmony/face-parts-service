#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T10:23:28
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_getfacerequesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../face-parts-service.pro)

SOURCES += \
    tst_pathdelegator.cpp \
    tst_getfacerequest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

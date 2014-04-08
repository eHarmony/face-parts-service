#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T10:23:28
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = tst_getfacerequesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../face-parts-service.pro)

SOURCES += tst_getfacerequesttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

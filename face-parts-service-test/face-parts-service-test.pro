#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T14:04:44
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = face-parts-service-test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../face-parts-service.pri)

SOURCES += tst_getfacerequest.cpp tst_pathdelegator.cpp \
    main.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    tst_getfacerequest.h \
    tst_pathdelegator.h \
    autotest.h

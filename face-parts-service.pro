#-------------------------------------------------
#
# Project created by QtCreator 2014-04-07T10:03:59
#
#-------------------------------------------------

QT       += core network gui

TARGET = face-parts-service
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(face-parts-service.pri)

SOURCES += main.cpp

# Mac Requirements
# sudo port install atlas +nofortran
# http://stefanfrings.de/qtwebapp/index-en.html

#-------------------------------------------------
#
# Project created by QtCreator 2014-04-07T10:03:59
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = face-parts
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(qtwebapp/lib/bfHttpServer/src/bfHttpServer.pri)
include(qtwebapp/lib/bfLogging/src/bfLogging.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/radidxml.hpp \
    $$PWD/CImg.h \
#    $$PWD/detect-face/eHbbox.h \
    $$PWD/detect-face/eHbox.h \
    $$PWD/detect-face/eHfacemodel.h \
    $$PWD/detect-face/eHfeatpyramid.h \
    $$PWD/detect-face/eHfilter.h \
    $$PWD/detect-face/eHimage.h \
#    $$PWD/detect-face/eHimageFeature.h \
    $$PWD/detect-face/eHmatrix.h \
    $$PWD/detect-face/eHposemodel.h \
    $$PWD/detect-face/eHshiftdt.h \
    $$PWD/detect-face/eHutils.h \
    $$PWD/webservice/pathdelegator.h \
    $$PWD/webservice/getfacerequest.h \
    $$PWD/weblogger.h

SOURCES += $$PWD/main.cpp \
#    $$PWD/detect-face/eHbbox.cpp \
    $$PWD/detect-face/eHbox.cpp \
    $$PWD/detect-face/eHfacemodel.cpp \
    $$PWD/detect-face/eHfeatpyramid.cpp \
    $$PWD/detect-face/eHfilter.cpp \
    $$PWD/detect-face/eHimage.cpp \
#    $$PWD/detect-face/eHimageFeature.cpp \
    $$PWD/detect-face/eHmatrix.cpp \
    $$PWD/detect-face/eHposemodel.cpp \
    $$PWD/detect-face/eHshiftdt.cpp \
    $$PWD/detect-face/eHutils.cpp \
    $$PWD/webservice/pathdelegator.cpp \
    $$PWD/webservice/getfacerequest.cpp \
    $$PWD/weblogger.cpp

INCLUDEPATH += /usr/local/include /Users/jmorra/Downloads/atlas/OSX_PPCG4AltiVec_2/include/ # /System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Versions/Current/Headers/
LIBS += -L/usr/local/lib -ljpeg -lcblas -latlas

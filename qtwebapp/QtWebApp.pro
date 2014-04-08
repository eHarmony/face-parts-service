QT = core network
TARGET = QtWebApp
TEMPLATE = app
CONFIG += CONSOLE

CONFIG(debug, debug|release) {
  DEFINES += SUPERVERBOSE
}

HEADERS = \
    src/static.h \
    src/startup.h \
    src/requestmapper.h \
    src/controller/dumpcontroller.h \
    src/controller/templatecontroller.h \
    src/controller/formcontroller.h \
    src/controller/fileuploadcontroller.h \
    src/controller/sessioncontroller.h

SOURCES = src/main.cpp \
    src/static.cpp \
    src/startup.cpp \
    src/requestmapper.cpp \
    src/controller/dumpcontroller.cpp \
    src/controller/templatecontroller.cpp \
    src/controller/formcontroller.cpp \
    src/controller/fileuploadcontroller.cpp \
    src/controller/sessioncontroller.cpp

OTHER_FILES += \
    Doxyfile \
    etc/* \
    etc/docroot/* \
    etc/templates/* \
    doc/* \
    logs/*

include(lib/qtservice/src/qtservice.pri)
include(lib/bfLogging/src/bfLogging.pri)
include(lib/bfHttpServer/src/bfHttpServer.pri)
include(lib/bfTemplateEngine/src/bfTemplateEngine.pri)


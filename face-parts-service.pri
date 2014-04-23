include(qtwebapp/bfHttpServer/src/bfHttpServer.pri)
include(qtwebapp/bfLogging/src/bfLogging.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/rapidxml.hpp \
    $$PWD/CImg.h \
    $$PWD/detect-face/eHbbox.h \
    $$PWD/detect-face/eHbox.h \
    $$PWD/detect-face/eHfacemodel.h \
    $$PWD/detect-face/eHfeatpyramid.h \
    $$PWD/detect-face/eHfilter.h \
    $$PWD/detect-face/eHimage.h \
    $$PWD/detect-face/eHimageFeature.h \
    $$PWD/detect-face/eHmatrix.h \
    $$PWD/detect-face/eHposemodel.h \
    $$PWD/detect-face/eHshiftdt.h \
    $$PWD/detect-face/eHutils.h \
    $$PWD/webservice/pathdelegator.h \
    $$PWD/webservice/getfacerequest.h \
    $$PWD/weblogger.h

SOURCES += \
    $$PWD/detect-face/eHbbox.cpp \
    $$PWD/detect-face/eHbox.cpp \
    $$PWD/detect-face/eHfacemodel.cpp \
    $$PWD/detect-face/eHfeatpyramid.cpp \
    $$PWD/detect-face/eHfilter.cpp \
    $$PWD/detect-face/eHimage.cpp \
    $$PWD/detect-face/eHimageFeature.cpp \
    $$PWD/detect-face/eHmatrix.cpp \
    $$PWD/detect-face/eHposemodel.cpp \
    $$PWD/detect-face/eHshiftdt.cpp \
    $$PWD/detect-face/eHutils.cpp \
    $$PWD/webservice/pathdelegator.cpp \
    $$PWD/webservice/getfacerequest.cpp \
    $$PWD/weblogger.cpp

macx {
    INCLUDEPATH += /usr/local/include /opt/local/include /usr/local/include/tbb/
    LIBS += -L/usr/local/lib
}

unix:!macx {
    INCLUDEPATH += /usr/include /usr/include/tbb
    LIBS += -L/usr/lib64/atlas/ -L/usr/lib64
}

LIBS += -ljpeg -lcblas -ltbb

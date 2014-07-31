#include "tst_pathdelegator.h"
#include <QString>
#include <QtTest/QTest>
#include <QJsonDocument>
#include <webservice/pathdelegator.h>
#include <webservice/getfaceresource.h>
#include <httpheaders.h>

void PathDelegatorTest::initTestCase() {
    pathDelegator = new PathDelegator();
    QSettings settings;
    faceResource = new GetFaceResource(NULL, NULL, settings);

    pathDelegator->addPath("/face-parts/generate", faceResource);
}

void PathDelegatorTest::cleanupTestCase() {
    delete pathDelegator;
}

void PathDelegatorTest::noPath() {
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);
    request.setPath("/foo");

    pathDelegator->service(request, response);

    QCOMPARE(HttpHeaders::STATUS_NOT_FOUND, response.getStatus());
}

void PathDelegatorTest::goodPath() {
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);

    request.setPath("/face-parts/generate");
    pathDelegator->service(request, response);

    QCOMPARE(HttpHeaders::STATUS_PRECONDITION_FAILED, response.getStatus());
}

void PathDelegatorTest::testRemoveExtension() {
    QCOMPARE(QString("abc"), QString(pathDelegator->removeExtension("abc.jpg")));
    QCOMPARE(QString("abc"), QString(pathDelegator->removeExtension("abc")));
}

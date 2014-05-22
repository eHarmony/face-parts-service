#include "tst_pathdelegator.h"
#include <QString>
#include <QtTest/QTest>
#include <QJsonDocument>
#include <webservice/pathdelegator.h>
#include <webservice/getfaceresource.h>

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

    QCOMPARE(404, response.getStatus());
}

void PathDelegatorTest::goodPath() {
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);

    request.setPath("/face-parts/generate");
    pathDelegator->service(request, response);

    QCOMPARE(500, response.getStatus());
}

#include "tst_pathdelegator.h"
#include <QString>
#include <QtTest/QTest>
#include <QJsonDocument>
#include <webservice/pathdelegator.h>
#include <webservice/getfacerequest.h>

void PathDelegatorTest::initTestCase() {
    pathDelegator = new PathDelegator();
    faceRequest = new GetFaceRequest(NULL, NULL);

    pathDelegator->addPath("/face-parts", faceRequest);
}

void PathDelegatorTest::cleanupTestCase() {
    delete pathDelegator;
}

void PathDelegatorTest::noPath() {
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);

    pathDelegator->service(request, response);

    QCOMPARE(404, response.getStatus());
}

void PathDelegatorTest::goodPath() {
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);

    request.setPath("/face-parts");
    pathDelegator->service(request, response);

    QCOMPARE(500, response.getStatus());
}

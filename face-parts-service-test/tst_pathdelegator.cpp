#include <QString>
#include <QtTest>
#include <QJsonDocument>
#include <webservice/pathdelegator.h>
#include <webservice/getfacerequest.h>

class PathDelegatorTest : public QObject
{
    Q_OBJECT

public:
    PathDelegatorTest();

private:
    PathDelegator* pathDelegator;
    GetFaceRequest* faceRequest;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void noPath();
    void goodPath();
};

void PathDelegatorTest::initTestCase() {
    pathDelegator = new PathDelegator();
    faceRequest = new GetFaceRequest(NULL, NULL);

    pathDelegator->addPath("/face-parts", faceRequest);
}

void PathDelegatorTest::cleanupTestCase() {
    delete pathDelegator;
}

void PathDelegatorTest::noPath() {
    HttpRequest request(NULL);
    HttpResponse response(NULL);

    pathDelegator->service(request, response);

    QCOMPARE(404, response.getStatus());
}

void PathDelegatorTest::goodPath() {
    HttpRequest request(NULL);
    HttpResponse response(NULL);

    request.setPath("/face-parts");
    pathDelegator->service(request, response);

    QCOMPARE(500, response.getStatus());
}

QTEST_APPLESS_MAIN(PathDelegatorTest)


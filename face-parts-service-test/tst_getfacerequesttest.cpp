#include <QString>
#include <QtTest>
#include <webservice/getfacerequest.h>

class GetFaceRequestTest : public QObject
{
    Q_OBJECT

public:
    GetFaceRequestTest();

private:
    facemodel_t* faceModel;
    posemodel_t* poseModel;
    QTemporaryFile* loadFile(const char* path);

private Q_SLOTS:
    void badFile();
    void noFace();
    void goodFace();
    void profile();
    void faceFromPose();
    void initTestCase();
};

void GetFaceRequestTest::initTestCase() {
    faceModel = facemodel_readFromFile("../resources/face_p146.xml");
    poseModel = posemodel_readFromFile("../resourcess/pose_BUFFY.xml");
}

void GetFaceRequestTest::badFile()
{
    GetFaceRequest faceRequest(NULL, NULL);
    HttpRequest request;
    HttpResponse response;

    faceRequest.service(request, response);

    QCOMPARE(500, response.getStatus());
}

void GetFaceRequestTest::noFace() {

}

void GetFaceRequestTest::goodFace() {

}

void GetFaceRequestTest::profile() {

}

void GetFaceRequestTest::faceFromPose() {

}

QTEST_APPLESS_MAIN(GetFaceRequestTest)

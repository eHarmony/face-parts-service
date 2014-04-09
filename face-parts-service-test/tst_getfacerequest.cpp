#include "tst_getfacerequest.h"
#include <QString>
#include <QtTest>
#include <webservice/getfacerequest.h>

void GetFaceRequestTest::initTestCase() {
    facemodel_t* faceModel = facemodel_readFromFile("../resources/face_p146.xml");
    posemodel_t* poseModel = posemodel_readFromFile("../resources/pose_BUFFY.xml");
    faceRequest = new GetFaceRequest(faceModel, poseModel);
}

void GetFaceRequestTest::cleanupTestCase() {
    delete faceRequest;
}

void GetFaceRequestTest::badFile()
{
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);

    faceRequest->service(request, response);

    QCOMPARE(500, response.getStatus());
}

void GetFaceRequestTest::noFace() {
    /*QFile file("resources/48170621_4.jpg");
    QJsonDocument doc = faceRequest->getJSONFaces(&file);
    QJsonArray array;
    QJsonDocument emptyDoc(array);

    QCOMPARE(emptyDoc, doc);*/
}

void GetFaceRequestTest::goodFace() {
    QFile file("resources/10156670_4.jpg");
    QJsonDocument doc = faceRequest->getJSONFaces(&file);
    QJsonDocument expected = loadJson("resources/10156670_4.json");

    QCOMPARE(expected, doc);
}

void GetFaceRequestTest::profile() {
    QFile file("resources/profile.jpg");
    QJsonDocument doc = faceRequest->getJSONFaces(&file);
    QJsonDocument expected = loadJson("resources/profile.json");

    QCOMPARE(expected, doc);
}

void GetFaceRequestTest::faceFromPose() {
    QFile file("resources/10009091_3.jpg");
    QJsonDocument doc = faceRequest->getJSONFaces(&file);
    QJsonDocument expected = loadJson("resources/10009091_3.json");

    QCOMPARE(expected, doc);
}

QJsonDocument GetFaceRequestTest::loadJson(const QString& fileName) {
    QFile jsonFile(fileName);
    jsonFile.open(QFile::ReadOnly);
    return QJsonDocument().fromJson(jsonFile.readAll());
}

#include "tst_getfacerequest.h"
#include <QString>
#include <QtTest>
#include <webservice/getfacerequest.h>

void GetFaceRequestTest::initTestCase() {
    QString modelResources = property("resources").toString();
    testResources = property("testResources").toString();

    facemodel_t* faceModel = facemodel_readFromFile(QString(modelResources + "/face_p146.xml").toStdString().c_str());
    posemodel_t* poseModel = posemodel_readFromFile(QString(modelResources + "/pose_BUFFY.xml").toStdString().c_str());
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
    QFile file(testResources + "/48170621_4.jpg");
    QJsonDocument doc = faceRequest->getJSONFaces(&file);
    QJsonArray array;
    QJsonDocument emptyDoc(array);

    QCOMPARE(emptyDoc, doc);
}

void GetFaceRequestTest::goodFace() {
    testFaces("1");
}

void GetFaceRequestTest::profile() {
    testFaces("profile");
}

void GetFaceRequestTest::faceFromPose() {
    testFaces("10156670_4");
}

void GetFaceRequestTest::testFaces(const QString& fileName) {
    QString base = testResources + "/" + fileName;
    QFile file(base + ".jpg");
    QJsonDocument current = faceRequest->getJSONFaces(&file);
    file.close();

    QFile jsonFile(base + ".json");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument expected = QJsonDocument().fromJson(jsonFile.readAll());
    jsonFile.close();

    QJsonArray currentArray = current.array();
    QJsonArray expectedArray = expected.array();

    QCOMPARE(currentArray.size(), expectedArray.size());

    for (int i=0; i<currentArray.size(); ++i) {
        QJsonObject cFaceParts = currentArray[i].toObject();
        QJsonObject eFaceParts = expectedArray[i].toObject();

        QJsonObject cFace = eFaceParts["face"].toObject();
        QJsonObject eFace = eFaceParts["face"].toObject();
        compareDoubles(cFace["x1"].toDouble(), eFace["x1"].toDouble(), 0.0001);
        compareDoubles(cFace["x2"].toDouble(), eFace["x2"].toDouble(), 0.0001);
        compareDoubles(cFace["y1"].toDouble(), eFace["y1"].toDouble(), 0.0001);
        compareDoubles(cFace["y2"].toDouble(), eFace["y2"].toDouble(), 0.0001);

        QJsonArray cParts = cFaceParts["parts"].toArray();
        QJsonArray eParts = eFaceParts["parts"].toArray();

        QCOMPARE(cParts.size(), eParts.size());

        for (int j=0; j<cParts.size(); ++j) {
            QJsonObject cPart = cParts[i].toObject();
            QJsonObject ePart = eParts[i].toObject();
            compareDoubles(cPart["x"].toDouble(), ePart["x"].toDouble(), 0.0001);
            compareDoubles(cPart["y"].toDouble(), ePart["y"].toDouble(), 0.0001);
        }
    }
}

void GetFaceRequestTest::compareDoubles(double actual, double expected, double delta) {
    QVERIFY(actual-delta < expected && actual+delta > expected);
}

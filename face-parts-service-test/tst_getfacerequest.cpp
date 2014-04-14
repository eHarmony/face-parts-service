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

void GetFaceRequestTest::noFile()
{
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);

    faceRequest->service(request, response);

    QCOMPARE(500, response.getStatus());
}

void GetFaceRequestTest::badFile() {
    QFile file(testResources + "/1.json");
    bool error;
    QJsonDocument doc = faceRequest->getJSONFaces(&file, error);
    file.close();

    QVERIFY(error);
    QCOMPARE(QJsonDocument(), doc);
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

void GetFaceRequestTest::noFace() {
    QFile file(testResources + "/48170621_4.jpg");
    bool error;
    QJsonDocument doc = faceRequest->getJSONFaces(&file, error);
    file.close();
    QJsonArray array;
    QJsonDocument emptyDoc(array);

    QVERIFY(!error);
    QCOMPARE(emptyDoc, doc);
}

void GetFaceRequestTest::testFaces(const QString& fileName) {
    QString base = testResources + "/" + fileName;
    QFile file(base + ".jpg");
    bool error;
    QJsonDocument current = faceRequest->getJSONFaces(&file, error);
    file.close();
    QVERIFY(!error);

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

        QCOMPARE(cFaceParts["pose"], eFaceParts["pose"]);

        QJsonObject cParts = cFaceParts["parts"].toObject();
        QJsonObject eParts = eFaceParts["parts"].toObject();

        QCOMPARE(cParts.size(), eParts.size());

        for (QJsonObject::const_iterator cIter = cParts.constBegin(); cIter != cParts.constEnd(); ++cIter) {
            QJsonArray cSubpart = cIter.value().toArray();
            QJsonArray eSubpart = cParts[cIter.key()].toArray();

            QCOMPARE(cSubpart.size(), eSubpart.size());

            for (int i=0; i<cSubpart.size(); ++i) {
                QJsonObject cPart = cSubpart[i].toObject();
                QJsonObject ePart = eSubpart[i].toObject();
                compareDoubles(cPart["x"].toDouble(), ePart["x"].toDouble(), 0.0001);
                compareDoubles(cPart["y"].toDouble(), ePart["y"].toDouble(), 0.0001);
                QCOMPARE(cPart["num"], ePart["num"]);
            }
        }
    }
}

void GetFaceRequestTest::compareDoubles(double actual, double expected, double delta) {
    QVERIFY(actual-delta < expected && actual+delta > expected);
}

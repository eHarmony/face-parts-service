#include "tst_getfaceresource.h"
#include <QString>
#include <QtTest>
#include <webservice/getfaceresource.h>
#include <iostream>
#include <httpheaders.h>

void GetFaceResourceTest::initTestCase() {
    QString modelResources = property("resources").toString();
    testResources = property("testResources").toString();

    facemodel_t* faceModel = facemodel_readFromFile(QString(modelResources + "/face_p146.xml").toStdString().c_str());
    posemodel_t* poseModel = posemodel_readFromFile(QString(modelResources + "/pose_BUFFY.xml").toStdString().c_str());
    QSettings settings;
    faceResource = new GetFaceResource(faceModel, poseModel, settings);
}

void GetFaceResourceTest::cleanupTestCase() {
    delete faceResource;
}

void GetFaceResourceTest::noFile()
{
    QSettings settings;
    HttpRequest request(&settings);
    HttpResponse response(NULL);

    faceResource->service(request, response);

    QCOMPARE(HttpHeaders::STATUS_PRECONDITION_FAILED, response.getStatus());
}

void GetFaceResourceTest::badFile() {
    QFile file(testResources + "/1.json");
    QJsonDocument doc;
    int status = faceResource->getJSONFaces(&file, doc);
    file.close();

    QCOMPARE(HttpHeaders::STATUS_UNSUPPORTED_MEDIA_TYPE, status);
    QCOMPARE(QJsonDocument(), doc);
}

void GetFaceResourceTest::goodFace() {
    testFaces("1");
}

void GetFaceResourceTest::profile() {
    testFaces("profile");
}

void GetFaceResourceTest::faceFromPose() {
    testFaces("10156670_4");
}

void GetFaceResourceTest::grayscale() {
    testFaces("grayscale");
}

void GetFaceResourceTest::cmyk() {
    testFaces("cmyk");
}

void GetFaceResourceTest::cmykAlternate() {
    testFaces("cmykAlternate");
}

void GetFaceResourceTest::noFace() {
    testNoFaces("48170621_4");
}

void GetFaceResourceTest::testNoFaces(const QString &fileName) {
    QFile file(testResources + QDir::separator() + fileName + ".jpg");
    QJsonDocument doc;
    int status = faceResource->getJSONFaces(&file, doc);
    file.close();
    QJsonArray array;
    QJsonDocument emptyDoc(array);

    QCOMPARE(HttpHeaders::STATUS_SUCCESS, status);
    QCOMPARE(emptyDoc, doc);
}

void GetFaceResourceTest::testFaces(const QString& fileName) {
    QString base = testResources + QDir::separator() + fileName;
    QFile file(base + ".jpg");
    QJsonDocument current;
    int status = faceResource->getJSONFaces(&file, current);
    file.close();
    QCOMPARE(HttpHeaders::STATUS_SUCCESS, status);

    QFile jsonFile(base + ".json");

    QVERIFY(jsonFile.exists());

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument expected = QJsonDocument().fromJson(jsonFile.readAll());
    jsonFile.close();

    QJsonArray currentArray = current.array();
    QJsonArray expectedArray = expected.array();

    // Make sure we're not comparing zero to zero
    QVERIFY(expectedArray.size() > 0);
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
        QCOMPARE(cFaceParts["model"], eFaceParts["model"]);

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

void GetFaceResourceTest::compareDoubles(double actual, double expected, double delta) {
    QVERIFY(actual-delta < expected && actual+delta > expected);
}

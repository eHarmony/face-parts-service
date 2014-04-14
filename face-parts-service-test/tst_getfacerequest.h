#ifndef TST_GETFACEREQUEST_H
#define TST_GETFACEREQUEST_H

#include <QObject>
#include <QJsonDocument>
#include <autotest.h>

class GetFaceRequest;

class GetFaceRequestTest : public QObject
{
    Q_OBJECT

private:
    GetFaceRequest *faceRequest;

    void testFaces(const QString& fileName);
    void compareDoubles(double actual, double expected, double delta);
    void printSVG(const QString& fileName);
    QString testResources;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void badFile();
    void noFile();
    void noFace();
    void goodFace();
    void profile();
    void faceFromPose();
};

DECLARE_TEST(GetFaceRequestTest)

#endif // TST_GETFACEREQUEST_H

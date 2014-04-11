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
    void testFaces(const QString& filename);
    void compareDoubles(double actual, double expected, double delta);
    QString testResources;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void noFace();
    void goodFace();
    void profile();
    void faceFromPose();
    void badFile();
};

DECLARE_TEST(GetFaceRequestTest)

#endif // TST_GETFACEREQUEST_H

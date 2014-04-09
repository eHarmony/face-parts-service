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
    QJsonDocument loadJson(const QString& filename);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void badFile();
    void noFace();
    void goodFace();
    void profile();
    void faceFromPose();
};

DECLARE_TEST(GetFaceRequestTest)

#endif // TST_GETFACEREQUEST_H

#ifndef TST_GETFACERESOURCE_H
#define TST_GETFACERESOURCE_H

#include <QObject>
#include <QJsonDocument>
#include <autotest.h>

class GetFaceResource;

class GetFaceResourceTest : public QObject
{
    Q_OBJECT

private:
    GetFaceResource *faceResource;

    void testNoFaces(const QString& fileName);
    void testFaces(const QString& fileName);
    void compareDoubles(double actual, double expected, double delta);
    void printSVG(const QString& fileName);
    QString testResources;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void cmyk();
    void goodFace();
    void profile();
    void faceFromPose();
    void badFile();
    void noFile();
    void noFace();
    void grayscale();
};

DECLARE_TEST(GetFaceResourceTest)

#endif // TST_GETFACERESOURCE_H

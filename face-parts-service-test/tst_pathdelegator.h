#ifndef TST_PATHDELEGATOR_H
#define TST_PATHDELEGATOR_H

#include <QObject>
#include <autotest.h>

class PathDelegator;
class GetFaceRequest;

class PathDelegatorTest : public QObject
{
    Q_OBJECT

private:
    PathDelegator* pathDelegator;
    GetFaceRequest* faceRequest;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void noPath();
    void goodPath();
};

DECLARE_TEST(PathDelegatorTest)

#endif // TST_PATHDELEGATOR_H

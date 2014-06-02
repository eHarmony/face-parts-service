#ifndef TST_PATHDELEGATOR_H
#define TST_PATHDELEGATOR_H

#include <QObject>
#include <autotest.h>

class PathDelegator;
class GetFaceResource;

class PathDelegatorTest : public QObject
{
    Q_OBJECT

private:
    PathDelegator* pathDelegator;
    GetFaceResource* faceResource;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testRemoveExtension();

    void noPath();
    void goodPath();
};

DECLARE_TEST(PathDelegatorTest)

#endif // TST_PATHDELEGATOR_H

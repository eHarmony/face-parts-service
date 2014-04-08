#ifndef GETFACEREQUEST_H
#define GETFACEREQUEST_H

#include <httprequesthandler.h>
#include <detect-face/eHfacemodel.h>
#include <detect-face/eHposemodel.h>

class HttpRequest;
class HttpResponse;

class GetFaceRequest : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit GetFaceRequest(facemodel_t* faceModel, posemodel_t* poseModel, QObject *parent = 0);
    virtual ~GetFaceRequest();
    void service(HttpRequest &request, HttpResponse &response);
    QJsonDocument getJSONFaces(QTemporaryFile *file);

private:
    facemodel_t* faceModel;
    posemodel_t* poseModel;
};

#endif // GETFACEREQUEST_H

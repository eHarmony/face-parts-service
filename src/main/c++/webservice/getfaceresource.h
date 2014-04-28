#ifndef GETFACERESOURCE_H
#define GETFACERESOURCE_H

#include <httprequesthandler.h>
#include <detect-face/eHfacemodel.h>
#include <detect-face/eHposemodel.h>

class GetFaceResource : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit GetFaceResource(facemodel_t* faceModel, posemodel_t* poseModel, QObject *parent = 0);
    virtual ~GetFaceResource();
    void service(HttpRequest &request, HttpResponse &response);
    QJsonDocument getJSONFaces(QFile *file, bool& error);

private:
    facemodel_t* faceModel;
    posemodel_t* poseModel;

    QJsonObject getProfileParts(const bbox_t& faceBox);
    QJsonObject getFrontalParts(const bbox_t& faceBox);
    QJsonObject getUnknownParts(const bbox_t& faceBox);
    QJsonObject extractPart(const bbox_t &faceBox, const size_t i);
    QJsonArray extractParts(const bbox_t &faceBox, const size_t start, const size_t end);
};

#endif // GETFACERESOURCE_H

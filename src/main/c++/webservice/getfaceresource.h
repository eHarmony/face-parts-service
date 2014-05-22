#ifndef GETFACERESOURCE_H
#define GETFACERESOURCE_H

#include <httprequesthandler.h>
#include <detect-face/eHfacemodel.h>
#include <detect-face/eHposemodel.h>

class GetFaceResource : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit GetFaceResource(facemodel_t* faceModel, posemodel_t* poseModel, const QSettings& settings, QObject *parent = 0);
    virtual ~GetFaceResource();
    void service(HttpRequest &request, HttpResponse &response);
    bool getJSONFaces(QFile *file, QJsonDocument& document);

private:
    facemodel_t* faceModel;
    posemodel_t* poseModel;

    QJsonObject getProfileParts(const bbox_t& faceBox);
    QJsonObject getFrontalParts(const bbox_t& faceBox);
    QJsonObject getUnknownParts(const bbox_t& faceBox);
    QJsonObject extractPart(const bbox_t &faceBox, const size_t i);
    QJsonArray extractParts(const bbox_t &faceBox, const size_t start, const size_t end);
    bool getFaceBoxes(QFile *file, std::vector<bbox_t>& faceBoxes);
    bool drawFacesOnImage(QFile *file, QByteArray& imageBytes);

    const unsigned char* initArray(const QStringList& values);

    const unsigned char* faceColor;
    const unsigned char* foregroundTextColor;
    const unsigned char* pointColor;
    int poseTextSize;
    int numberTextSize;
};

#endif // GETFACERESOURCE_H

#ifndef GETFACERESOURCE_H
#define GETFACERESOURCE_H

#include <httprequesthandler.h>
#include <detect-face/eHfacemodel.h>
#include <detect-face/eHposemodel.h>
#include <CImg.h>

class GetFaceResource : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit GetFaceResource(facemodel_t* faceModel, posemodel_t* poseModel, const QSettings& settings, QObject *parent = 0);
    virtual ~GetFaceResource();
    void service(HttpRequest &request, HttpResponse &response);
    int getJSONFaces(QFile *file, QJsonDocument& document, bool writeJustPoints=false);

private:
    facemodel_t* faceModel;
    posemodel_t* poseModel;

    QJsonObject getProfileParts(const bbox_t& faceBox);
    QJsonObject getFrontalParts(const bbox_t& faceBox);
    QJsonObject getUnknownParts(const bbox_t& faceBox);
    QJsonObject extractPart(const bbox_t &faceBox, const size_t i);
    QJsonArray extractParts(const bbox_t &faceBox, const size_t start, const size_t end);
    int getFaceBoxes(QFile *file, std::vector<bbox_t>& faceBoxes);
    int drawFacesOnImage(QFile *file, QByteArray& imageBytes, const QByteArray& extension);
    int saveFacesOnImage(const cimg_library::CImg<int>& img, QByteArray& imageBytes, const QByteArray& extension);
    QByteArray generateErrorMessage(QFile *file) const;

    const unsigned char* initArray(const QStringList& values);

    const int profilePoints;
    const int frontalPoints;
    const QStringList imageExtensions;
    const QByteArray uploadedFileName;
    const int poseTextSize;
    const int numberTextSize;

    const unsigned char* faceColor;
    const unsigned char* foregroundTextColor;
    const unsigned char* pointColor;

    static const QByteArray FILE_IO_ERROR;
    static const QByteArray NO_FILE_ERROR;
    static const QByteArray IMAGE_TYPE_NOT_SUPPORTED;
    static const QByteArray NO_CHECKSUM;

    const static QByteArray JPEG;
    const static QByteArray JPG;
};

#endif // GETFACERESOURCE_H

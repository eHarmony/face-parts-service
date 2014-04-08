#include "getfacerequest.h"
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <detect-face/eHimage.h>

GetFaceRequest::GetFaceRequest(facemodel_t* faceModel, posemodel_t* poseModel, QObject *parent) :
    HttpRequestHandler(parent) {
    this->faceModel = faceModel;
    this->poseModel = poseModel;
}

void GetFaceRequest::service(HttpRequest &request, HttpResponse &response) {
    QTemporaryFile* file = request.getUploadedFile("file");
    if (file) {
        QJsonDocument doc = getJSONFaces(file);
        response.write(doc.toJson(QJsonDocument::Compact));
    }
    else {
        response.setStatus(500, "Cannot find the file parameter");
    }
}

QJsonDocument GetFaceRequest::getJSONFaces(QTemporaryFile *file) {
    image_t* img = image_readJPG(file->fileName().toStdString().c_str());
    std::vector<bbox_t> faceBoxes = facemodel_detect(faceModel, poseModel, img);
    QJsonArray faces;
    for (std::vector<bbox_t>::const_iterator bboxIter = faceBoxes.begin(); bboxIter != faceBoxes.end(); ++bboxIter) {
        bbox_t faceBox = (*bboxIter);
        QJsonObject face;
        face["x1"] = faceBox.outer.x1;
        face["y1"] = faceBox.outer.y1;
        face["x2"] = faceBox.outer.x2;
        face["y2"] = faceBox.outer.y2;
        QJsonArray points;
        for (std::vector<fbox_t>::const_iterator fboxIter = faceBox.boxes.begin(); fboxIter != faceBox.boxes.end(); ++ fboxIter) {
            fbox_t box = (*fboxIter);
            QJsonObject point;
            point["x"] = (box.x1 + box.x2)/2.0;
            point["y"] = (box.y1 + box.y2)/2.0;
            points.append(point);
        }
        face["points"] = points;
        faces.append(face);
    }
    QJsonDocument doc(faces);
    return doc;
}

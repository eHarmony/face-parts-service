#include "getfaceresource.h"
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <detect-face/eHimage.h>

GetFaceResource::GetFaceResource(facemodel_t* faceModel, posemodel_t* poseModel, QObject *parent) :
    HttpRequestHandler(parent) {
    this->faceModel = faceModel;
    this->poseModel = poseModel;
}

GetFaceResource::~GetFaceResource() {
    if (faceModel) {
        delete faceModel;
    }
    if (poseModel) {
        delete poseModel;
    }
}

void GetFaceResource::service(HttpRequest &request, HttpResponse &response) {
    QTemporaryFile* file = request.getUploadedFile("file");
    if (file) {
        bool error = false;
        QJsonDocument doc = getJSONFaces(file, error);
        if (error) {
            response.setStatus(500, "Internal processing error");
        }
        else {
            response.write(doc.toJson(QJsonDocument::Compact));
        }
    }
    else {
        response.setStatus(500, "Cannot find the file parameter");
    }
}

QJsonDocument GetFaceResource::getJSONFaces(QFile *file, bool& error) {
    std::vector<bbox_t> faceBoxes;
    try {
        image_t* img = image_readJPG(file->fileName().toStdString().c_str());
        faceBoxes = facemodel_detect(faceModel, poseModel, img);
        image_delete(img);
    }
    catch (...) {
        error = true;
        return QJsonDocument();
    }

    QJsonArray faces;
    for (std::vector<bbox_t>::const_iterator bboxIter = faceBoxes.begin(); bboxIter != faceBoxes.end(); ++bboxIter) {
        bbox_t faceBox = (*bboxIter);
        QJsonObject faceParts;
        QJsonObject face;
        face["x1"] = faceBox.outer.x1;
        face["y1"] = faceBox.outer.y1;
        face["x2"] = faceBox.outer.x2;
        face["y2"] = faceBox.outer.y2;
        faceParts["face"] = face;
        faceParts["pose"] = faceBox.pose;

        QJsonObject parts;
        switch (faceBox.boxes.size()) {
        case 39:
            parts = getProfileParts(faceBox);
            faceParts["model"] = QString("profile");
            break;
        case 68:
            parts = getFrontalParts(faceBox);
            faceParts["model"] = QString("frontal");
            break;
        default:
            parts = getUnknownParts(faceBox);
            faceParts["model"] = QString("unknown");
        }

        faceParts["parts"] = parts;
        faces.append(faceParts);
    }
    QJsonDocument doc(faces);
    return doc;
}

QJsonObject GetFaceResource::getProfileParts(const bbox_t &faceBox) {
    QJsonObject parts;
    parts["nose bottom to top"] = extractParts(faceBox, 0, 6);
    parts["eye bottom to top"] = extractParts(faceBox, 6, 11);
    parts["eyebrow nose to ear"] = extractParts(faceBox, 11, 15);
    parts["mouth"] = extractParts(faceBox, 15, 28);
    parts["jaw bottom to top"] = extractParts(faceBox, 28, 39);
    return parts;
}

QJsonObject GetFaceResource::getFrontalParts(const bbox_t &faceBox) {
    QJsonObject parts;
    parts["nostrils right to left"] = extractParts(faceBox, 0, 5);
    parts["nose bottom to top"] = extractParts(faceBox, 5, 9);
    parts["right eye left to right"] = extractParts(faceBox, 9, 15);
    parts["right eyebrow right to left"] = extractParts(faceBox, 15, 20);
    parts["left eye right to left"] = extractParts(faceBox, 20, 26);
    parts["left eyebrow left to right"] = extractParts(faceBox, 26, 31);
    parts["upper lip right to left"] = extractParts(faceBox, 31, 41);
    parts["lower lip left to right"] = extractParts(faceBox, 41, 51);
    parts["right jaw bottom to top"] = extractParts(faceBox, 51, 60);
    parts["left jaw bottom to top"] = extractParts(faceBox, 60, 68);
    return parts;
}

QJsonObject GetFaceResource::getUnknownParts(const bbox_t &faceBox) {
    QJsonObject parts;
    for (size_t i=0; i<faceBox.boxes.size(); ++i) {
        parts[QString::number(i)] = extractPart(faceBox, i);
    }
    return parts;
}

QJsonArray GetFaceResource::extractParts(const bbox_t &faceBox, const size_t start, const size_t end) {
    QJsonArray parts;
    for (size_t i=start; i<end; ++i) {
        parts.append(extractPart(faceBox, i));
    }
    return parts;
}

QJsonObject GetFaceResource::extractPart(const bbox_t &faceBox, const size_t i) {
    fbox_t box = faceBox.boxes[i];
    QJsonObject part;
    part["x"] = (box.x1 + box.x2)/2.0;
    part["y"] = (box.y1 + box.y2)/2.0;
    part["num"] = (int)i;
    return part;
}

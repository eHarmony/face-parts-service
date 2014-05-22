#include "getfaceresource.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <detect-face/eHimage.h>
#include <CImg.h>
#include <httpheaders.h>

GetFaceResource::GetFaceResource(facemodel_t* faceModel, posemodel_t* poseModel, const QSettings& settings, QObject *parent) :
    HttpRequestHandler(parent) {
    this->faceModel = faceModel;
    this->poseModel = poseModel;

    QStringList faceColorStr = settings.value("faceColor", QStringList(QStringList() << "255" << "0" << "0")).toStringList();
    QStringList foregroundTextColorStr = settings.value("foregroundTextColor", QStringList(QStringList() << "0" << "0" << "0")).toStringList();
    QStringList pointColorStr = settings.value("pointColor", QStringList(QStringList() << "0" << "255" << "0")).toStringList();
    poseTextSize = settings.value("poseTextSize", 20).toInt();
    numberTextSize = settings.value("numberTextSize", 15).toInt();

    faceColor = initArray(faceColorStr);
    foregroundTextColor = initArray(foregroundTextColorStr);
    pointColor = initArray(pointColorStr);
}

const unsigned char* GetFaceResource::initArray(const QStringList &values) {
    unsigned char* array = new unsigned char[values.length()];
    for (int i=0; i<values.length(); ++i) {
        array[i] = values[i].toInt();
    }
    return array;
}

GetFaceResource::~GetFaceResource() {
    if (faceModel) {
        delete faceModel;
    }
    if (poseModel) {
        delete poseModel;
    }
    if (faceColor) {
        delete[] faceColor;
    }
    if (foregroundTextColor) {
        delete[] foregroundTextColor;
    }
    if (pointColor) {
        delete[] pointColor;
    }
}

void GetFaceResource::service(HttpRequest &request, HttpResponse &response) {
    QFile* file = request.getUploadedFile("file");
    if (file) {
        if (request.getPath().endsWith(".jpeg") || request.getPath().endsWith(".jpg")) {
            QByteArray imageBytes;
            if (drawFacesOnImage(file, imageBytes)) {
                response.setHeader(HttpHeaders::CONTENT_TYPE, HttpHeaders::IMAGE_JPEG);
                response.setHeader(HttpHeaders::CONTENT_LENGTH, imageBytes.size());
                response.write(imageBytes);
            }
            else {
                response.setStatus(500, "Internal processing error, is the uploaded file a valid jpeg?");
            }
        }
        else {
            QJsonDocument doc;
            if (getJSONFaces(file, doc)) {
                response.write(doc.toJson(QJsonDocument::Compact));
            }
            else {
                response.setStatus(500, "Internal processing error, is the uploaded file a valid jpeg?");
            }
        }
    }
    else {
        response.setStatus(500, "Cannot find the file parameter");
    }
}

bool GetFaceResource::drawFacesOnImage(QFile *file, QByteArray& imageBytes) {
    std::vector<bbox_t> faceBoxes;

    if (!getFaceBoxes(file, faceBoxes)) {
        return false;
    }

    cimg_library::CImg<int> img;
    img.load_jpeg(file->fileName().toStdString().c_str());

    for (std::vector<bbox_t>::const_iterator bboxIter = faceBoxes.begin(); bboxIter != faceBoxes.end(); ++bboxIter) {
        bbox_t faceBox = (*bboxIter);
        img.draw_line(faceBox.outer.x1, faceBox.outer.y1, faceBox.outer.x2, faceBox.outer.y1, faceColor);
        img.draw_line(faceBox.outer.x2, faceBox.outer.y1, faceBox.outer.x2, faceBox.outer.y2, faceColor);
        img.draw_line(faceBox.outer.x2, faceBox.outer.y2, faceBox.outer.x1, faceBox.outer.y2, faceColor);
        img.draw_line(faceBox.outer.x1, faceBox.outer.y2, faceBox.outer.x1, faceBox.outer.y1, faceColor);

        // Transparent and opaque background text
        img.draw_text((faceBox.outer.x2 + faceBox.outer.x1)/2.0,
                      faceBox.outer.y1 - 15,
                      QString::number(faceBox.pose).toStdString().c_str(),
                      foregroundTextColor,
                      0,
                      1,
                      poseTextSize);

        for (size_t i=0; i<faceBox.boxes.size(); ++i) {
            fbox_t part = faceBox.boxes[i];
            img.draw_circle((part.x1 + part.x2)/2.0, (part.y1 + part.y2)/2.0, 5, pointColor, 0.5);

            // Transparent and opaque background text
            img.draw_text((part.x1 + part.x2)/2.0,
                          (part.y1 + part.y2)/2.0 + 10,
                          QString::number(i).toStdString().c_str(),
                          foregroundTextColor,
                          0,
                          1,
                          numberTextSize);
        }
    }

    // Since we don't have libjpeg 8 we can't use jpeg_mem_dest.  As a result we have to
    // go to file and then read the information back in.  We have to open it first so that it exists
    // then we close it after we read it back in.
    QTemporaryFile tempJpegFile;
    tempJpegFile.open();
    img.save_jpeg(tempJpegFile.fileName().toStdString().c_str());
    imageBytes = tempJpegFile.readAll();
    tempJpegFile.close();
    return true;
}

bool GetFaceResource::getFaceBoxes(QFile *file, std::vector<bbox_t>& faceBoxes) {
    try {
        image_t* img = image_readJPG(file->fileName().toStdString().c_str());
        faceBoxes = facemodel_detect(faceModel, poseModel, img);
        image_delete(img);
        return true;
    }
    catch (...) {
        return false;
    }
}

 bool GetFaceResource::getJSONFaces(QFile *file, QJsonDocument& document) {
    std::vector<bbox_t> faceBoxes;

    if (!getFaceBoxes(file, faceBoxes)) {
        return false;
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
    document.setArray(faces);
    return true;
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

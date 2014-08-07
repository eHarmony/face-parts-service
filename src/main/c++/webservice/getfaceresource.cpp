#include "getfaceresource.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QDateTime>
#include <detect-face/eHimage.h>
#include <httpheaders.h>

const QByteArray GetFaceResource::FILE_IO_ERROR = "Internal processing error, is the uploaded file a valid jpeg?  ";
const QByteArray GetFaceResource::NO_FILE_ERROR = "Cannot find the file parameter";
const QByteArray GetFaceResource::IMAGE_TYPE_NOT_SUPPORTED = "Requested image type is not supported";
const QByteArray GetFaceResource::NO_CHECKSUM ="Cannot calculate checksum!!!";
const QByteArray GetFaceResource::JPEG = "jpeg";
const QByteArray GetFaceResource::JPG = "jpg";

/**
 * @brief GetFaceResource::GetFaceResource
 *
 * This constructor is responsible for extracting
 * all the settings applicable to this resource.  These values will be set accoriding to the supplied
 * settings, or by reasonable defaults.
 *
 * @param faceModel
 * @param poseModel
 * @param settings
 * @param parent
 */
GetFaceResource::GetFaceResource(facemodel_t* faceModel, posemodel_t* poseModel, const QSettings& settings, QObject *parent) :
    HttpRequestHandler(parent),
    profilePoints(settings.value("profilePoints", 39).toInt()),
    frontalPoints(settings.value("frontalPoints", 68).toInt()),
    imageExtensions(settings.value("imageResponses", "jpg,jpeg,png").toStringList()),
    uploadedFileName(settings.value("uploadedFileName", "file").toByteArray()),
    poseTextSize(settings.value("poseTextSize", 20).toInt()),
    numberTextSize(settings.value("numberTextSize", 15).toInt())
{
    this->faceModel = faceModel;
    this->poseModel = poseModel;

    // These setting control how the returned JPEG has data drawn on it.  The face color is the color of the lines surrounding the faces.
    // The foreground text color is the text color (the backgroud is transparent).  The point color is the color of the fiducial markers.
    // the pose text size is the font size for the pose label, and the number text size is the font size for the number labels.
    QStringList faceColorStr = settings.value("faceColor", "255,0,0").toStringList();
    QStringList foregroundTextColorStr = settings.value("foregroundTextColor", "0,0,0").toStringList();
    QStringList pointColorStr = settings.value("pointColor", "0,255,0").toStringList();

    faceColor = initArray(faceColorStr);
    foregroundTextColor = initArray(foregroundTextColorStr);
    pointColor = initArray(pointColorStr);
}

/**
 * @brief GetFaceResource::initArray -- Copy the unsigned integer values in values to an array.
 * @param values
 * @return A native const unsigned char* array of the StringList
 */
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

/**
 * @brief GetFaceResource::service
 *
 * The method called when an HTTP request comes in for this service.  In order
 * to proceed with any processing, a file must be uploaded via a multipart form with the name declared by the
 * uploadedFileName setting.  If this file is not present, a 500 is returned.
 *
 * If no other parameters are present, a JSON object is returned with a structured version of any faces found.
 * Examples of the JSON structure can be seen in the test directory.
 *
 * The request will also respond to extensions of the path which request an image specified in the imageExtensions list.
 * if this is the case the Face Parts will be drawn on the input image and a new image returned.  The request will also
 * respond to a parameter called points.  If that parameter is present, and it's value is inline then the parts JSON will
 * not contain a structured value of the points, and just list the points in order.
 * @param request
 * @param response
 */
void GetFaceResource::service(HttpRequest &request, HttpResponse &response) {
    QFile* file = request.getUploadedFile(uploadedFileName);
    if (file) {
        QByteArray path = request.getPath().toLower();
        int extensionLocation = path.lastIndexOf(".");
        if (extensionLocation > 0) {
            QByteArray extension = path.right(path.size() - extensionLocation - 1);
            if (imageExtensions.contains(extension)) {
                QByteArray imageBytes;
                int status = drawFacesOnImage(file, imageBytes, extension);
                if (status == HttpHeaders::STATUS_SUCCESS) {
                    response.setHeader(HttpHeaders::CONTENT_TYPE, QByteArray("image/") + extension);
                    response.setHeader(HttpHeaders::CONTENT_LENGTH, imageBytes.size());
                    response.setHeader(HttpHeaders::CACHE_CONTROL, HttpHeaders::NO_CACHE);
                    response.write(imageBytes);
                }
                else {
                    response.setStatus(status, generateErrorMessage(file));
                }
            }
            else {
                response.setStatus(HttpHeaders::STATUS_UNSUPPORTED_MEDIA_TYPE, IMAGE_TYPE_NOT_SUPPORTED);
            }
        }
        else {
            QJsonDocument doc;
            bool writeJustPoints = request.getParameterMap().contains("points") && request.getParameter("points") == "inline";
            int status = getJSONFaces(file, doc, writeJustPoints);
            if (status == HttpHeaders::STATUS_SUCCESS) {
                response.write(doc.toJson(QJsonDocument::Compact));
            }
            else {
                response.setStatus(status, generateErrorMessage(file));
            }
        }
    }
    else {
        response.setStatus(HttpHeaders::STATUS_PRECONDITION_FAILED, NO_FILE_ERROR);
    }
}

/**
 * @brief GetFaceResource::generateErrorMessage -- Create an error message using the checksum of the file
 *
 * This will create an error message using the Md5 checksum of the supplied file.  This is useful such that
 * if mining an error log, one can go back and find the exact file that caused the error message by comparing
 * checksums.
 *
 * @param file
 * @return the error message
 */
QByteArray GetFaceResource::generateErrorMessage(QFile *file) const {
    QByteArray now = QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8();
    QByteArray checksum;
    if (file->open(QIODevice::ReadOnly)) {
        QByteArray fileBytes = file->readAll();
        file->close();

        checksum = QByteArray(QCryptographicHash::hash(fileBytes, QCryptographicHash::Md5).toHex().toUpper());
    }
    else {
        checksum = NO_CHECKSUM;
    }
    return FILE_IO_ERROR + QByteArray("Time: ") + now + QByteArray("  Checksum: ") + checksum;
}

/**
 * @brief GetFaceResource::drawFacesOnImage -- Draws the face parts on the supplied jpeg image.
 *
 * This will draw all the face parts on the supplied imageBytes.  The imageBytes are modified to
 * contain the face parts.  This will use a variety of the settings to help the drawing process.
 *
 * @param file
 * @param imageBytes
 * @param extension
 * @return the HTTP return code
 */
int GetFaceResource::drawFacesOnImage(QFile *file, QByteArray& imageBytes, const QByteArray& extension) {
    std::vector<bbox_t> faceBoxes;

    int status = getFaceBoxes(file, faceBoxes);
    if (status != HttpHeaders::STATUS_SUCCESS) {
        return status;
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
            img.draw_circle((part.x1 + part.x2)/2.0, (part.y1 + part.y2)/2.0, 5, pointColor, 1);

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

    return saveFacesOnImage(img, imageBytes, extension);
}

/**
 * @brief GetFaceResource::saveFacesOnImage -- Reads the supplied CImg and puts the image into imageBytes.
 * @param img
 * @param imageBytes
 * @param extension
 * @return the HTTP return code
 */
int GetFaceResource::saveFacesOnImage(const cimg_library::CImg<int>& img, QByteArray &imageBytes, const QByteArray &extension) {
    // Since we don't have libjpeg 8 we can't use jpeg_mem_dest.  As a result we have to
    // go to file and then read the information back in.  We have to open it first so that it exists
    // then we close it after we read it back in.
    QTemporaryFile tempImageFile;
    tempImageFile.open();
    const char* fileName = tempImageFile.fileName().toStdString().c_str();
    if (extension == JPG || extension == JPEG) {
        img.save_jpeg(fileName);
    }
    else {
        return HttpHeaders::STATUS_UNSUPPORTED_MEDIA_TYPE;
    }
    imageBytes = tempImageFile.readAll();
    tempImageFile.close();
    return HttpHeaders::STATUS_SUCCESS;
}

/**
 * @brief GetFaceResource::getFaceBoxes -- Gets the face parts of the image in the file.
 * @param file
 * @param faceBoxes
 * @return the HTTP return code
 */
int GetFaceResource::getFaceBoxes(QFile *file, std::vector<bbox_t>& faceBoxes) {
    try {
        image_t* img = image_readJPG(file->fileName().toStdString().c_str());
        if (img == NULL) {
            return HttpHeaders::STATUS_ERROR;
        }
        faceBoxes = facemodel_detect(faceModel, poseModel, img);
        image_delete(img);
        return HttpHeaders::STATUS_SUCCESS;
    }
    catch (...) {
        return HttpHeaders::STATUS_UNSUPPORTED_MEDIA_TYPE;
    }
}

/**
 * @brief GetFaceResource::getJSONFaces -- Gets the face parts as JSON of the file and writes it to the document.
 * @param file
 * @param document
 * @param writeJustPoints -- Says whether or not to list the points or break them down into meaningful groups
 * @return the HTTP return code
 */
int GetFaceResource::getJSONFaces(QFile *file, QJsonDocument& document, bool writeJustPoints) {
    std::vector<bbox_t> faceBoxes;

    int status = getFaceBoxes(file, faceBoxes);
    if (status != HttpHeaders::STATUS_SUCCESS) {
        return status;
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

        int numBoxes = (int)faceBox.boxes.size();
        if (writeJustPoints) {
            QJsonArray parts;
            for (std::vector<fbox_t>::const_iterator fboxIter = faceBox.boxes.begin(); fboxIter != faceBox.boxes.end(); ++fboxIter) {
                fbox_t box = *fboxIter;
                parts.append((box.x1 + box.x2)/2.0);
                parts.append((box.y2 + box.y2)/2.0);
            }
            faceParts["parts"] = parts;
        }
        else {
            QJsonObject parts;
            if (numBoxes == profilePoints) {
                parts = getProfileParts(faceBox);
                faceParts["model"] = QString("profile");
            }
            else if (numBoxes == frontalPoints) {
                parts = getFrontalParts(faceBox);
                faceParts["model"] = QString("frontal");
            }
            else {
                parts = getUnknownParts(faceBox);
                faceParts["model"] = QString("unknown");
            }
            faceParts["parts"] = parts;
        }
        faces.append(faceParts);
    }
    document.setArray(faces);
    return status;
}

/**
 * @brief GetFaceResource::getProfileParts -- Gets the face parts for the profile model
 * @param faceBox
 * @return a JSON object of profile parts
 */
QJsonObject GetFaceResource::getProfileParts(const bbox_t &faceBox) {
    QJsonObject parts;
    parts["nose bottom to top"] = extractParts(faceBox, 0, 6);
    parts["eye bottom to top"] = extractParts(faceBox, 6, 11);
    parts["eyebrow nose to ear"] = extractParts(faceBox, 11, 15);
    parts["mouth"] = extractParts(faceBox, 15, 28);
    parts["jaw bottom to top"] = extractParts(faceBox, 28, 39);
    return parts;
}

/**
 * @brief GetFaceResource::getFrontalParts -- Gets the face parts for the frontal model
 * @param faceBox
 * @return a JSON object of frontal parts
 */
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

/**
 * @brief GetFaceResource::getUnknownParts -- Gets the face parts for an unknown model
 * @param faceBox
 * @return a JSON object of uknown parts
 */
QJsonObject GetFaceResource::getUnknownParts(const bbox_t &faceBox) {
    QJsonObject parts;
    for (size_t i=0; i<faceBox.boxes.size(); ++i) {
        parts[QString::number(i)] = extractPart(faceBox, i);
    }
    return parts;
}

/**
 * @brief GetFaceResource::extractParts -- Extract all the parts from start to end in the faceBox array
 * @param faceBox
 * @param start
 * @param end
 * @return a JSON array of all the parts from start to end
 */
QJsonArray GetFaceResource::extractParts(const bbox_t &faceBox, const size_t start, const size_t end) {
    QJsonArray parts;
    for (size_t i=start; i<end; ++i) {
        parts.append(extractPart(faceBox, i));
    }
    return parts;
}

/**
 * @brief GetFaceResource::extractPart -- Extract one specific face part at the center of the box at index i
 * @param faceBox
 * @param i
 * @return a JSON object of the (x, y) center of the identified box, and index as num
 */
QJsonObject GetFaceResource::extractPart(const bbox_t &faceBox, const size_t i) {
    fbox_t box = faceBox.boxes[i];
    QJsonObject part;
    part["x"] = (box.x1 + box.x2)/2.0;
    part["y"] = (box.y1 + box.y2)/2.0;
    part["num"] = (int)i;
    return part;
}

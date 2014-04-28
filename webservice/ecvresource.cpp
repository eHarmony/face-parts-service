#include "ecvresource.h"

const QByteArray ECVResource::SERVER_UP_TEXT = "SERVER UP";
const QByteArray ECVResource::SERVER_DOWN_TEXT = "SERVER DOWN";
const int ECVResource::SERVER_UP_STATUS = 200;
const int ECVResource::SERVER_DOWN_STATUS = 503;

ECVResource::ECVResource(QFile* ecvFile, QObject *parent) :
    HttpRequestHandler(parent) {
    this->ecvFile = ecvFile;
}

ECVResource::~ECVResource() {
    if (ecvFile != NULL) {
        delete ecvFile;
    }
}

void ECVResource::service(HttpRequest &request, HttpResponse &response) {
    Q_UNUSED(request);
    if (isServerUp()) {
        response.setStatus(SERVER_UP_STATUS, SERVER_UP_TEXT);
        response.write(SERVER_UP_TEXT);
    }
    else {
        response.setStatus(SERVER_DOWN_STATUS, SERVER_DOWN_TEXT);
        response.write(SERVER_DOWN_TEXT);
    }
}

bool ECVResource::isServerUp() const {
    if (ecvFile == NULL) {
        return true;
    }
    return !ecvFile->exists();
}

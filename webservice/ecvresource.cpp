#include "ecvresource.h"

ECVResource::ECVResource( QFile* ecvFile, QObject *parent) :
    HttpRequestHandler(parent) {
    this->ecvFile = ecvFile;
}

ECVResource::~ECVResource() {

}

void ECVResource::service(HttpRequest &request, HttpResponse &response) {
    Q_UNUSED(request);
    if (isServerUp()) {
        response.setStatus(200);
        response.write("SERVER UP");
    }
    else {
        response.setStatus(503);
        response.write("SERVER DOWN");
    }
}

bool ECVResource::isServerUp() const {
    if (ecvFile == NULL) {
        return true;
    }
    return !ecvFile->exists();
}

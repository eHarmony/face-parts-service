#include "pathdelegator.h"
#include <httpheaders.h>

PathDelegator::PathDelegator(QObject *parent) :
    HttpRequestHandler(parent) {
}

PathDelegator::~PathDelegator() {
    for (QMap<QByteArray, HttpRequestHandler*>::iterator iter = paths.begin(); iter != paths.end(); ++iter) {
        if (iter.value()) {
            delete iter.value();
        }
    }
}

void PathDelegator::service(HttpRequest &request, HttpResponse &response) {
    QByteArray path = removeExtension(request.getPath());
    if (paths.contains(path)) {
        paths[path]->service(request, response);
    }
    else {
        response.setStatus(HttpHeaders::STATUS_NOT_FOUND, QByteArray("Cannot find ") + path);
    }
}

void PathDelegator::addPath(const QByteArray &path, HttpRequestHandler *handler) {
    paths.insert(path, handler);
}

QByteArray PathDelegator::removeExtension(const QByteArray &path) const {
    if (path.contains(".")) {
        return path.left(path.indexOf("."));
    }
    return path;
}

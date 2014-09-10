#include "pathdelegator.h"
#include <httpheaders.h>
#include <weblogger.h>

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

/**
 * @brief PathDelegator::service -- Delegate the request to the service designed to handle that request
 * @param request
 * @param response
 */
void PathDelegator::service(HttpRequest &request, HttpResponse &response) {
    QByteArray path = removeExtension(request.getPath());
    if (paths.contains(path)) {
        paths[path]->service(request, response);
    }
    else {
        response.setStatus(HttpHeaders::STATUS_NOT_FOUND, QByteArray("Cannot find ") + path);
    }
    WebLogger::instance()->log(QtWarningMsg, QByteArray::number(response.getStatus()) + ": " + response.getStatusText());
}

/**
 * @brief PathDelegator::addPath -- Add a service at a specific path
 * @param path
 * @param handler
 */
void PathDelegator::addPath(const QByteArray &path, HttpRequestHandler *handler) {
    paths.insert(path, handler);
}

/**
 * @brief PathDelegator::removeExtension -- A helper method to remove the extension from a path
 * @param path
 * @return the path with the extension removed.
 */
QByteArray PathDelegator::removeExtension(const QByteArray &path) const {
    if (path.contains(".")) {
        return path.left(path.indexOf("."));
    }
    return path;
}

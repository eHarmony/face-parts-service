#ifndef HTTPHEADERS_H
#define HTTPHEADERS_H

#include <QByteArray>

class HttpHeaders
{
public:
    const static QByteArray CONTENT_TYPE;
    const static QByteArray ACCEPT;
    const static QByteArray CONTENT_LENGTH;
    const static QByteArray CACHE_CONTROL;
    const static QByteArray NO_CACHE;
    const static int STATUS_SUCCESS;
    const static int STATUS_NOT_FOUND;
    const static int STATUS_PRECONDITION_FAILED;
    const static int STATUS_UNSUPPORTED_MEDIA_TYPE;
    const static int STATUS_ERROR;
};

#endif // HTTPHEADERS_H

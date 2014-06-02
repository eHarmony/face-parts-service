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
    const static int STATUS_ERROR;
    const static int STATUS_NOT_FOUND;

};

#endif // HTTPHEADERS_H

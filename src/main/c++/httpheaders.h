#ifndef HTTPHEADERS_H
#define HTTPHEADERS_H

#include <QByteArray>

class HttpHeaders
{
public:
    const static QByteArray CONTENT_TYPE;
    const static QByteArray ACCEPT;
    const static QByteArray IMAGE_JPG;
    const static QByteArray IMAGE_JPEG;
    const static QByteArray CONTENT_LENGTH;

};

#endif // HTTPHEADERS_H

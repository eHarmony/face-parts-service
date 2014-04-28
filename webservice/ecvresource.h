#ifndef ECVRESOURCE_H
#define ECVRESOURCE_H

#include <httprequesthandler.h>

class ECVResource : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit ECVResource(QFile* ecvFile, QObject *parent = 0);
    virtual ~ECVResource();
    void service(HttpRequest &request, HttpResponse &response);

private:
    QFile* ecvFile;

    bool isServerUp() const;
    static const QByteArray SERVER_UP_TEXT;
    static const QByteArray SERVER_DOWN_TEXT;
    static const int SERVER_UP_STATUS;
    static const int SERVER_DOWN_STATUS;

};

#endif // ECVRESOURCE_H

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

};

#endif // ECVRESOURCE_H

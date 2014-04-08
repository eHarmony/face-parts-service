#ifndef PATHDELEGATOR_H
#define PATHDELEGATOR_H

#include <QObject>
#include <QMap>
#include <QByteArray>
#include <httprequesthandler.h>

class HttpRequest;
class HttpResponse;

class PathDelegator : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit PathDelegator(QObject *parent = 0);
    void service(HttpRequest &request, HttpResponse &response);
    void addPath(const QByteArray& path, HttpRequestHandler* handler);

private:
    QMap<QByteArray, HttpRequestHandler*> paths;

};

#endif // PATHDELEGATOR_H

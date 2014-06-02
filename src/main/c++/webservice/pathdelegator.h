#ifndef PATHDELEGATOR_H
#define PATHDELEGATOR_H

#include <QObject>
#include <QMap>
#include <QByteArray>
#include <httprequesthandler.h>

class PathDelegator : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit PathDelegator(QObject *parent = 0);
    virtual ~PathDelegator();
    void service(HttpRequest &request, HttpResponse &response);
    void addPath(const QByteArray& path, HttpRequestHandler* handler);
    QByteArray removeExtension(const QByteArray& path) const;

private:
    QMap<QByteArray, HttpRequestHandler*> paths;

};

#endif // PATHDELEGATOR_H

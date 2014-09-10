#ifndef WEBLOGGER_H
#define WEBLOGGER_H

#include <filelogger.h>

class WebLogger
{
public:
    static void setFileLogger(FileLogger* fileLogger);
    static void log(const QtMsgType type, const QString& message, const QString &file="", const QString &function="", const int line=0);

private:
    WebLogger();
    WebLogger(WebLogger const&);
    void operator=(WebLogger const&);

    static FileLogger *logger;
};

#endif // WEBLOGGER_H

#ifndef WEBLOGGER_H
#define WEBLOGGER_H

#include <filelogger.h>

class WebLogger
{
public:
    static void setFileLogger(FileLogger* fileLogger);
    static FileLogger* instance();

private:
    WebLogger();
    WebLogger(WebLogger const&);
    void operator=(WebLogger const&);

    static FileLogger *logger;
};

#endif // WEBLOGGER_H

#include "weblogger.h"

FileLogger* WebLogger::logger = NULL;

void WebLogger::setFileLogger(FileLogger *fileLogger) {
    WebLogger::logger = fileLogger;
}

void WebLogger::log(const QtMsgType type, const QString& message, const QString &file, const QString &function, const int line) {
    if (logger) {
        logger->log(type, message, file, function, line);
    }
}

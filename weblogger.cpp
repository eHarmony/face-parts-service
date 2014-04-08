#include "weblogger.h"

FileLogger* WebLogger::logger = NULL;

void WebLogger::setFileLogger(FileLogger *fileLogger) {
    WebLogger::logger = fileLogger;
}

FileLogger* WebLogger::instance() {
    return logger;
}

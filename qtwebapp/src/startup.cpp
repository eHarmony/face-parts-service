/**
  @file
  @author Stefan Frings
*/

#include "static.h"
#include "startup.h"
#include "dualfilelogger.h"
#include "httplistener.h"
#include "requestmapper.h"
#include "staticfilecontroller.h"
#include <QDir>

/** Name of this application */
#define APPNAME "QtWebApp"

/** Publisher of this application */
#define ORGANISATION "Butterfly"

/** Short description of this application */
#define DESCRIPTION "Web service based on Qt"

/** The HTTP listener of the application */
HttpListener* listener;

void Startup::start() {
    // Initialize the core application
    QCoreApplication* app = application();
    app->setApplicationName(APPNAME);
    app->setOrganizationName(ORGANISATION);
    QString configFileName=Static::getConfigDir()+"/"+QCoreApplication::applicationName()+".ini";

    // Configure logging
    QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    logSettings->beginGroup("logging");
    Logger* logger=new FileLogger(logSettings,10000,app);
    logger->installMsgHandler();

    // Configure template loader and cache
    QSettings* templateSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    templateSettings->beginGroup("templates");
    Static::templateLoader=new TemplateCache(templateSettings,app);

    // Configure session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    sessionSettings->beginGroup("sessions");
    Static::sessionStore=new HttpSessionStore(sessionSettings,app);

    // Configure static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    fileSettings->beginGroup("docroot");
    Static::staticFileController=new StaticFileController(fileSettings,app);

    // Configure and start the TCP listener
    qDebug("ServiceHelper: Starting service");
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,app);
    listenerSettings->beginGroup("listener");
    listener=new HttpListener(listenerSettings,new RequestMapper(app),app);

    if (logSettings->value("bufferSize",0).toInt()>0 && logSettings->value("minLevel",0).toInt()>0) {
        qDebug("You see these debug messages because the logging buffer is enabled");
    }
    qWarning("ServiceHelper: Service has started");
}

void Startup::stop() {
    // Note that the stop method is not called when you terminate the application abnormally
    // by pressing Ctrl-C or when you kill it by the task manager of your operating system.

    // Deleting the listener here is optionally because QCoreApplication does it already.
    // However, QCoreApplication closes the logger at first, so we would not see the shutdown
    // debug messages, without the following line of code:
    delete listener;

    qWarning("ServiceHelper: Service has been stopped");
}


Startup::Startup(int argc, char *argv[])
    : QtService<QCoreApplication>(argc, argv, APPNAME)
{
    setServiceDescription(DESCRIPTION);
    //setStartupType(AutoStartup);
}




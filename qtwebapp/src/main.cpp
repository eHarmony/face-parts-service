/**
  @file
  @author Stefan Frings
*/

/**
  @mainpage
  This project provides libraries for writing server-side web application in C++
  based on the Qt toolkit. It is a light-weight implementation that works like 
  Java Servlets from the programmers point of view.
  <p>
  Features:

  - HttpListener is a HTTP 1.1 web server with support for
      - persistent connections
      - chunked and non-chunked transfer
      - file uploads
      - cookies
      - dynamic thread pool
      - optional file cache
      - sessions via HttpSessionStore class
  - The Template engine supports
      - multi languages
      - optional file cache
      - for all text based file formats
  - The Logger classes provide
      - automatic backups and file rotation
      - configurable message format
      - messages may contain thread-local info variables
      - optional ring-buffer for writing debug messages in case of errors
      - they are configurable at runtime without program restart
  - The Example application in Startup demonstrates how to
      - install and run as Windows service, Unix daemon or at the command-line
      - search the config file in multiple common directories
      - write controller (Servlets) for common use-cases

  If you write a real application based on this source, take a look into startup.cpp,
  which contains startup and shutdown procedures. The example source sets
  up a single listener on port 8080, however multiple listeners with individual
  configuration could be set up.
  <p>
  Incoming requests are mapped to several controller classes in requestmapper.cpp, based on the
  requested URL. These controllers work like Java Servlets, they are derived from HttpRequestHandler.
  If you want to learn form the example, then focus on these classes.
  <p>
  High-availability and HTTPS encryption can be easily added by putting an Apache HTTP server
  in front of the self-written web application using the mod-proxy module with sticky sessions.
*/

#include "startup.h"

#include "httplistener.h"
#include "requestmapper.h"
#include "static.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QString>

/**
  Entry point of the program.
  Passes control to the service helper.
*/
int main(int argc, char *argv[]) {
    // Use a qtservice wrapper to start the application as a Windows service or Unix daemon
    Startup startup(argc, argv);
    return startup.exec();
}

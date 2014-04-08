/**
  @file
  @author Stefan Frings
*/

#ifndef STARTUP_H
#define STARTUP_H

#include <QtCore/QCoreApplication>
#include "qtservice.h"

/**
  Helper class to install and run the application as a windows
  service.
*/
class Startup : public QtService<QCoreApplication>
{
public:

    /** Constructor */
    Startup(int argc, char *argv[]);

protected:

    /** Start the service */
    void start();

    /**
      Stop the service gracefully.
      @warning This method is not called when the program terminates
      abnormally, e.g. after a fatal error, or when killed from outside.
    */
    void stop();

};

#endif // STARTUP_H

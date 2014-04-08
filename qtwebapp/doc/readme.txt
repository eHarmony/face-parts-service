You need to install the QT SDK with QT 4.7.0 or newer.
Open the project file QtWepApp.pro with QT Creator.

To quickly test the program, run it with argument -e. You should configure
your project in Qt Creator accordingly, so you can run and debug it within
QT Creator.

When you stop the program by clicking the red stop button under Linux,
you will see the error message "The program crashed". This is not an error,
but a harmless side-effect of the qtservice library.

When you run the application without any command-line option, it will try
to start the Windows service or Unix daemon. To get this working, you must
install the service once by using the option -i.

The argument -h will display a help about the other available options.

The argument -c does nothing useful in the current version.

If you like to generate a class documentation in HTML, then install Doxygen
and execute the Doxygen command in the projects main directory. The result
will be placed in doc/html.

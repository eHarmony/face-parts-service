# Mac Installation Instructions
1.  Install XCode developer tools (not sure about this one)
2.  brew install tbb
3.  Install MacPorts from https://www.macports.org/install.php
4.  sudo port install atlas +nofortran (might take a while)
5.  Download and install Qt 5.2.1 from http://qt-project.org/downloads

# Linux Installation Instructions
1.  sudo yum install atlas-devel.x86_64
2.  sudo yum install libjpeg-devel
3.  sudo yum install tbb-devel
4.  sudo yum install qt5

# Make Instructions
1.  Locate qmake (or add it to your path)
2.  Create a build directory and cd into it
3.  qmake path/to/src/face-parts-service.pro
  1. For debug version run qmake CONFIG=debug path/to/src/face-parts-service.pro
4.  make

# Test Instructions
1.  Locate qmake (or add it to your path)
2.  Create a build directory and cd into it
3.  qmake path/to/src/face-parts-service-test/face-parts-service-test.pro
4.  make
5.  ./face-parts-service-test resources=path/to/src/resources testResources=path/to/src/face-parts-service-test/resources
6.  Verify that all tests have passed

This makes use of a modified version of QtWebApp, found at http://stefanfrings.de/qtwebapp/index-en.html

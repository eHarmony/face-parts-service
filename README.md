# Mac Installation Instructions
1.  Install XCode developer tools
2.  Install MacPorts from https://www.macports.org/install.php
3.  sudo port install atlas +nofortran (might take a while)
4.  Download and install Qt 5.2.1 from http://qt-project.org/downloads

# Linux Installation Instructions
1.  sudo yum install libjpeg-devel
2.  sudo yum install atlas-devel.x86_64
3.  sudo yum install qt5

# Make instructions
1.  Locate qmake (or add it to your path)
2.  Create a build directory and cd into it
3.  qmake path/to/src/face-parts-service.pro
  1. For debug version run qmake CONFIG=debug path/to/src/face-parts-service.pro
4.  make

# Test instructions
1.  Locate qmake (or add it to your path)
2.  Create a build directory and cd into it
3.  qmake path/to/src/face-parts-service-test/face-parts-service-test.pro
4.  make
5.  ./face-parts-service-test resources=path/to/src/resources testResources=path/to/src/face-parts-service-test/resources
6.  Verify that all tests have passed
# Executive Summary

This is a RESTful API for segmenting human faces from an image.  The software is powered by [face parts](http://www.ics.uci.edu/~xzhu/face/) using [CImg](http://cimg.sourceforge.net/) for image loading.  The goal of this project is threefold:

1.  Remove the dependency on Matlab
2.  Make the code more usable by wrapping it in a RESTful API.  This makes use of a modified version of [QtWebApp](http://stefanfrings.de/qtwebapp/index-en.html)
3.  Speed up the code by making use of [Threaded Building Blocks](https://www.threadingbuildingblocks.org/)

# Mac Necessary Libraries

1.  Install XCode developer tools and homebrew
2.  brew install tbb
3.  Install MacPorts from [here](https://www.macports.org/install.php)
4.  sudo port install atlas +nofortran (might take a while)
5.  Download and install Qt 5.2.1 from [here](http://qt-project.org/downloads)

# Red Necessary Libraries

1.  sudo yum install atlas-devel.x86_64
2.  sudo yum install libjpeg-devel
3.  sudo yum install tbb-devel
4.  sudo yum install qt5-base-devel

# Ubuntu Necessary Libraries

1.  sudo apt-get install libatlas-dev
2.  sudo apt-get install libjpeg-dev
3.  sudo apt-get install libtbb-dev
4.  sudo apt-get install qt5-default

(for running the code, don't install the devel versions, just use the regular versions)

# Make Instructions
We are using maven as a build architecture.  I have tested this using [maven version 3.0.5](http://maven.apache.org/download.cgi).

1.  Add qmake to your path
2.  mvn install

# Running the Server

In order to start the webserver run `target/classes/face-parts-service src/main/resources/configfile.ini`.  At this point, the server is ready to segment images.  In order to submit an image, create a multipart form and submit a JPEG file (right now the server only works with JPEGs) to http://localhost:8084/face-parts/generate with a key named "file".  An example of submitting a file using the [Chrome](https://www.google.com/intl/en-US/chrome/browser/) web browser extension [Postman](https://chrome.google.com/webstore/detail/postman-rest-client/fdmmgilgnpjigdojojpjoooidkmcomcm?hl=en) can be seen below.

![Submitting an image with Postman](/images/postman_demo.png)

If the request was successful, a 200 response will be returned along with some JSON described here, where the outer array is a list of all the faces found:

    [
        "face": {
            "x1": <upper left x coordinate of face box>,
            "y1": <upper left y coordinate of face box>,
            "x2": <lower right x coordinate of face box>,
            "y2": <lower right x coordinate of face box>
        },
        "pose": <angle the person is facing from -90 to 90 degrees>,
        "model": <frontal or profile depending on which model was used to find the face>,
        "parts":  {
            <descriptive name of face region>: [
                {
                    "x": <x coordinate of face part>,
                    "y": <y coordinate of face part>,
                    "num": <the part number in the model>
                }
            ]
        }
    ]

The server is also able to return an image with the information about the face overlayed on it.  In order to do that, submit the same request as above to http://localhost:8084/face-parts/generate.jpg

If you want to get just a list of the points instead of the descriptive regions, submit the request to http://localhost:8084/face-parts/generate?points=inline

# Configuration

The webserver can be configured in a variety of ways.  An example configuration file is found at `src/main/resources/configfile.ini`.  It is very important that the lines

    face_model=src/main/resources/face_p146.xml
    pose_model=src/main/resources/pose_BUFFY.xml

point to valid files, as these represent the models used for segmentation.  These paths are relative to the directory the program was started from.

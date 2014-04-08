#include <QCoreApplication>
#include <QSettings>
#include <httplistener.h>
#include <filelogger.h>
#include <webservice/pathdelegator.h>
#include <webservice/getfacerequest.h>
#include <weblogger.h>

int main(int argc, char** argv){
    QCoreApplication app(argc,argv);

    QSettings settings(argv[1], QSettings::IniFormat, &app);

    FileLogger* logger = new FileLogger(&settings);
    WebLogger::setFileLogger(logger);

    facemodel_t* faceModel = facemodel_readFromFile(settings.value("face_model").toString().toStdString().c_str());
    posemodel_t* poseModel = posemodel_readFromFile(settings.value("pose_model").toString().toStdString().c_str());

    PathDelegator* pathDelegator = new PathDelegator();
    pathDelegator->addPath("/face-parts", new GetFaceRequest(faceModel, poseModel));

    new HttpListener(&settings, pathDelegator, &app);

    return app.exec();
}

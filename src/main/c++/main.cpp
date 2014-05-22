#include <QCoreApplication>
#include <QSettings>
#include <httplistener.h>
#include <filelogger.h>
#include <webservice/pathdelegator.h>
#include <webservice/getfaceresource.h>
#include <webservice/ecvresource.h>
#include <weblogger.h>

int main(int argc, char** argv) {
    QCoreApplication app(argc,argv);

    QSettings settings(argv[1], QSettings::IniFormat, &app);

    facemodel_t* faceModel = facemodel_readFromFile(settings.value("face_model").toString().toStdString().c_str());
    posemodel_t* poseModel = posemodel_readFromFile(settings.value("pose_model").toString().toStdString().c_str());

    FileLogger logger(&settings);
    WebLogger::setFileLogger(&logger);

    QFile *ecvFile = NULL;
    if (settings.contains("ecv_file")) {
        ecvFile = new QFile(settings.value("ecv_file").toString());
    }
    PathDelegator pathDelegator;
    pathDelegator.addPath("/face-parts/generate", new GetFaceResource(faceModel, poseModel, settings));
    pathDelegator.addPath("/face-parts/ecv", new ECVResource(ecvFile));

    new HttpListener(&settings, &pathDelegator, &app);

    return app.exec();
}

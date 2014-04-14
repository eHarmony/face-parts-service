#include <QCoreApplication>
#include <QSettings>
#include <httplistener.h>
#include <filelogger.h>
#include <webservice/pathdelegator.h>
#include <webservice/getfacerequest.h>
#include <weblogger.h>

void printSVG(const QString &fileName, facemodel_t* faceModel, posemodel_t* poseModel) {
    QFile file(fileName);

    image_t* img = image_readJPG(file.fileName().toStdString().c_str());
    std::vector<bbox_t> faces = facemodel_detect(faceModel, poseModel, img);
    image_delete(img);

    // 12 hexidecimal colors.
    char colors[73] = "D36767D39D67D3D3679DD36767D36767D39D67D3D3679DD36767D39D67D3D367D3D3679D";
    printf("<?xml version=\"1.0\" standalone=\"yes\"?>\n");
      printf("<svg width=\"%d\" height=\"%d\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n", (int)img->sizx, (int)img->sizy);
      for (unsigned int i = 0; i < faces.size(); ++i) {
        printf("<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" style=\"fill:none;stroke:red;stroke-width:3;opacity:1.0\"/> <!-- face %i -->\n",
           faces[i].outer.x1,
           faces[i].outer.y1,
           faces[i].outer.x2 - faces[i].outer.x1,
           faces[i].outer.y2 - faces[i].outer.y1,
           i
        );
        printf("<text x=\"%f\" y=\"%f\" font-family=\"Courier New\" font-size=\"12\" style=\"stroke:white;stroke-width:0.5\">Pose: %d</text>\n",
               (faces[i].outer.x2 + faces[i].outer.x1) / 2.0,
                faces[i].outer.y1 - 10,
               faces[i].pose);

        for (unsigned int j = 0; j < faces[i].boxes.size(); ++j) {
          unsigned int b = 6 * ((j*5) % 12);

          printf("<text x=\"%f\" y=\"%f\" font-family=\"Courier New\" font-size=\"12\" style=\"stroke:white;stroke-width:0.5\">%i</text>\n",
             (faces[i].boxes[j].x1 + faces[i].boxes[j].x2) / 2.0 - 4.0,
             (faces[i].boxes[j].y1 + faces[i].boxes[j].y2) / 2.0 - 4.0,
             j
             );

          printf("<circle cx=\"%f\" cy=\"%f\" r=\"3\" style=\"stroke:#006600; fill:#%c%c%c%c%c%c\"/> <!-- face %i, point %i -->\n",
             (faces[i].boxes[j].x1 + faces[i].boxes[j].x2) / 2.0,
             (faces[i].boxes[j].y1 + faces[i].boxes[j].y2) / 2.0,
             colors[b], colors[b+1], colors[b+2], colors[b+3], colors[b+4], colors[b+5],
             i, j
             );
        }
      }
      printf("</svg>\n");
}


int main(int argc, char** argv) {
    QCoreApplication app(argc,argv);

    QSettings settings(argv[1], QSettings::IniFormat, &app);

    facemodel_t* faceModel = facemodel_readFromFile(settings.value("face_model").toString().toStdString().c_str());
    posemodel_t* poseModel = posemodel_readFromFile(settings.value("pose_model").toString().toStdString().c_str());

    /*printSVG(argv[2], faceModel, poseModel);
    fflush(stdout);
    exit(0);*/

    FileLogger* logger = new FileLogger(&settings);
    WebLogger::setFileLogger(logger);

    PathDelegator* pathDelegator = new PathDelegator();
    pathDelegator->addPath("/face-parts", new GetFaceRequest(faceModel, poseModel));

    new HttpListener(&settings, pathDelegator, &app);

    return app.exec();
}

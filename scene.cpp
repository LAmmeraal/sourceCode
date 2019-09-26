// scene.cpp: Line remains in center of resized window.
#include "scene.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Scene w;
   w.show();
   return a.exec();
}

Scene::Scene(QWidget *parent) {
   scene.addLine(100, 100, 250, 180);
   view.setScene(&scene);
   setCentralWidget(&view);
}

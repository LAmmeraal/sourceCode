// cubewireframemain.cpp: Cube (wireframe); main function.
#include "cubewireframe.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   CubeWireframe w;
   w.show();
   return a.exec();
}
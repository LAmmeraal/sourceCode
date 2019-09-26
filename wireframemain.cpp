// wireframemain.cpp: Wireframe models; main function.
#include "wireframe.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Wireframe w;
   w.show();
   return a.exec();
}
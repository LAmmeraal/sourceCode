// sphtriamain.cpp: Sphere approximated by triangles; main function;
#include "sphtria.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   SphTria w;
   w.show();
   return a.exec();
}

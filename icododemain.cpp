// icododemain.cpp: Icosahedron and dodecahedron; main function.
#include "icodode.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   IcoDode w;
   w.show();
   return a.exec();
}

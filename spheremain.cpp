// spheremain.cpp: Globe model of sphere; main function.
#include "sphere.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Sphere w;
   w.show();
   return a.exec();
}

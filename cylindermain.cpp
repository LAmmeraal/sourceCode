// cylindermain.cpp: Approximated (hollow) cylinder; main function.
#include "cylinder.h"
int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Cylinder w;
   w.show();
   return a.exec();
}

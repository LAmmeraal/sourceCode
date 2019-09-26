// torusmain.cpp: Generating a torus; main function.
#include "torus.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Torus w;
   w.show();
   return a.exec();
}

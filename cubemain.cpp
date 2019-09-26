// cubemain.cpp: Generating a cube; main function.
#include "cube.h"
int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Cube w;
   w.show();
   return a.exec();
}

// hfacesmain.cpp:  Hidden faces, Z-buffer algorithm; main function.
#include "hfaces.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   HFaces w;
   w.show();
   return a.exec();
}

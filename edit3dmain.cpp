// edit3dmain.cpp: 3D editing; main function.
#include "edit3d.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Edit3D w;
   w.show();
   return a.exec();
}

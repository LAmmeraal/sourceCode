// drawpolygonmain.cpp: Drawing a polygon; main function.
#include "drawpolygon.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   DrawPolygon w;
   w.show();
   return a.exec();
}

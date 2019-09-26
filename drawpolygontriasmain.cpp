// drawpolygontriasmain.cpp: Triangulation demo; main function.
#include "drawpolygontrias.h"
int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   DrawPolygonTrias w;
   w.show();
   return a.exec();
}

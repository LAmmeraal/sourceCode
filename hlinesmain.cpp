// hlinesmain.cpp: Hidden-line elimination; main function.
#include "hlines.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   HLines w;
   w.show();
   return a.exec();
}

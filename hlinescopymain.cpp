// hlinescopymain.cpp: Copying and shifting; main function.
#include "hlinescopy.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   HLinesCopy w;
   w.show();
   return a.exec();
}

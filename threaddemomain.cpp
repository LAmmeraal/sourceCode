// threaddemomain.cpp: Demo of thread; main function.
#include "threaddemo.h"
int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   ThreadDemo w;
   w.show();
   return a.exec();
}

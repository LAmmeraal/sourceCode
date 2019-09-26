// threadprogressmain.cpp:  A progress bar; main function.
#include "threadprogress.h"
int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    ThreadProgress w;
    w.show();
    return a.exec();
}

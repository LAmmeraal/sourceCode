// threaddemo.h: Thread demonstration program.
#ifndef THREADDEMO_H
#define THREADDEMO_H
#include <QtWidgets>

using namespace std;

class MyThread : public QThread {
   void run();
};

class ThreadDemo: public QMainWindow {
   Q_OBJECT
public:
   ThreadDemo(QWidget *parent = nullptr);
   void calculate();
   int n;
   qreal average;
   MyThread thread;
   bool resultAvailable = false;
protected:
   void paintEvent(QPaintEvent *e) override;
   void mousePressEvent(QMouseEvent *event) override;
   void closeEvent(QCloseEvent *event) override;
public slots:
   void onFinished();
};
#endif // THREADDEMO_H

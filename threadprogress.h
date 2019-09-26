// threadprogress.h: A progress bar.
#ifndef THREADPROGRESS_H
#define THREADPROGRESS_H
#include <QtWidgets>

using namespace std;

class MyThread : public QThread {
   void run();
};

class ThreadProgress : public QMainWindow {
   Q_OBJECT
public:
   ThreadProgress(QWidget *parent = nullptr);
   void calculate();
   int n;
   qreal average;
   MyThread thread;
   QProgressBar progressBar;
   bool resultAvailable = false;
protected:
   void paintEvent(QPaintEvent *e) override;
   void mousePressEvent(QMouseEvent *event) override;
   void closeEvent(QCloseEvent *event) override;
public slots:
   void onFinished();
signals:
   void progress(int);
};
#endif // THREADPROGRESS_H



// threadprogress.cpp: A progress bar.
#include "threadprogress.h"

ThreadProgress::ThreadProgress(QWidget *parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int scrW = rec.width(), scrH = rec.height();
   setGeometry(scrW / 8, scrH / 6, 6 * scrW / 8, 4 * scrH / 6);
   setWindowTitle("Thread demo: GUI responsive during computing");
   thread.setParent(this); // Used in MyThread::run below.
   repaint();
}

void ThreadProgress::calculate() {
   qreal sum = 0;
   for (int i = 1; i <= n; i++) {
      if (thread.isInterruptionRequested()) {
         return;
      }
      emit progress(i * 100 / n); // For progress bar
      qreal i2 = i * i;
      for (int j = 1; j <= n; j++) {
         qreal j2 = j * j;
         for (int k = 1; k <= n; k++)
            sum += sqrt(i2 + j2 + k * k);
      }
   }
   if (n > 0) {
      average = sum / (n * n * n);
      resultAvailable = true;
   }
}

void ThreadProgress::onFinished() {
   progressBar.hide();
   repaint();
}

void ThreadProgress::paintEvent(QPaintEvent *e) {
   QPainter pnt(this);
   pnt.drawText(30, 100,
      "Click the mouse button to start a new computation.");
   if (resultAvailable) {
      pnt.drawText(30, 150,
         "For i, j, k running independently from 1 to " +
         QString::number(n) +
         ", the average of all values sqrt(i^2 + j^2 + k^2) is " +
         QString::number(average));
   }
}

void ThreadProgress::mousePressEvent(QMouseEvent *event) {
   if (thread.isRunning())  // in case of impatience...
      thread.requestInterruption();
   bool ok;
   int n1 = QInputDialog::getInt(this, "Thread demo",
      "Enter n  to compute n x n x n calculations",
      1, 1, 1000000000, 1, &ok);
   if (!ok)   // Cancel button used?
      return; // n, average and resultAvailable not changed.
   n = n1;
   resultAvailable = false;
   connect(&thread, SIGNAL(finished()), this, SLOT(onFinished()));
   connect(this, SIGNAL(progress(int)),
      &progressBar, SLOT(setValue(int)));
   progressBar.show();
   progressBar.setGeometry(pos().x() + 20, pos().y() + height() / 2,
      width() - 40, 20);
   progressBar.setWindowTitle("Computing with n = " +
      QString::number(n));
   thread.start();
}

void ThreadProgress::closeEvent(QCloseEvent *event) {
   thread.terminate();
   event->accept();
}

void MyThread::run() {
   ThreadProgress *p = (ThreadProgress*)(parent());
      p->calculate();
}

// changeline.cpp: Derived class as alternative solution.
#include "changeline.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   ChangeLine w;
   w.show();
   return a.exec();
}

ChangeLine::ChangeLine(QWidget *parent) :
   QMainWindow(parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int h = rec.height(), w = rec.width();
   setGeometry(w / 10, h / 10, 8 * w / 10, 8 * h / 10);
   setWindowTitle("Click a button to change the line");
   xDeviationPerc = 0;
   buttonLeft.setParent(this);
   buttonRight.setParent(this);
   buttonLeft.setText("Left");
   buttonRight.setText("Right");
}

void ChangeLine::resizeEvent(QResizeEvent *e) {
   QMainWindow::resizeEvent(e);
   int w = width(), h = height(),
      wButton = w / 4, hButton = h / 6;
   buttonLeft.setGeometry(0, h - hButton, wButton, hButton);
   buttonRight.setGeometry(w - wButton, h - hButton,
      wButton, hButton);
}

void ChangeLine::paintEvent(QPaintEvent *e) {
   QPainter painter(this);
   int xStart = width() / 2, yStart = 9 * height() / 10,
      xEnd = xStart + xDeviationPerc * width() / 100,
      yEnd = height() / 10;
   painter.drawLine(xStart, yStart, xEnd, yEnd);
}

void MyButton::mouseReleaseEvent(QMouseEvent *e) {
   ChangeLine *p = (ChangeLine*)(parent());
   if (this == &(p->buttonLeft))
      p->xDeviationPerc -= 10;
   else
      p->xDeviationPerc += 10;
   p->repaint();
}

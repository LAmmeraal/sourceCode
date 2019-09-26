// slotdemo.cpp: Signals and slots.
#include "slotdemo.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   SlotDemo w;
   w.show();
   return a.exec();
}

SlotDemo::SlotDemo(QWidget *parent) :
   QMainWindow(parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int h = rec.height(), w = rec.width();
   setGeometry(w / 10, h / 10, 8 * w / 10, 8 * h / 10);
   setWindowTitle("Click a button to change the line");
   xDeviationPerc = 0;
   connect(&buttonLeft, SIGNAL(released()), this,
      SLOT(handleButtonLeft()));
   connect(&buttonRight, SIGNAL(released()), this,
      SLOT(handleButtonRight()));
   buttonLeft.setParent(this);
   buttonLeft.setText("Left");
   buttonRight.setParent(this);
   buttonRight.setText("Right");
}

void SlotDemo::resizeEvent(QResizeEvent *e) {
   QMainWindow::resizeEvent(e);
   int w = width(), h = height(),
      wButton = w / 4, hButton = h / 6;
   buttonLeft.setGeometry(0, h - hButton, wButton, hButton);
   buttonRight.setGeometry(w - wButton, h - hButton,
      wButton, hButton);
}

void SlotDemo::paintEvent(QPaintEvent *e) {
   QPainter p(this);
   int xStart = width() / 2, yStart = 9 * height() / 10,
      xEnd = xStart + xDeviationPerc * width() / 100,
      yEnd = height() / 10;
   p.drawLine(xStart, yStart, xEnd, yEnd);
}

void SlotDemo::handleButtonLeft() {
   xDeviationPerc -= 10;
   repaint();
}

void SlotDemo::handleButtonRight() {
   xDeviationPerc += 10;
   repaint();
}

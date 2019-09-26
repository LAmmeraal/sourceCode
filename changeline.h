// changeline.h: Derived class as alternative solution.
#ifndef CHANGELINE_H
#define CHANGELINE_H
#include <QtWidgets>

class MyButton : public QPushButton {
public:
   void mouseReleaseEvent(QMouseEvent *e) override;
};

class ChangeLine : public QMainWindow {
   Q_OBJECT
public:
   ChangeLine(QWidget *parent = nullptr);
   int xDeviationPerc;
   MyButton buttonLeft, buttonRight;
protected:
   void paintEvent(QPaintEvent *e) override;
   void resizeEvent(QResizeEvent *e) override;
};
#endif // CHANGELINE_H

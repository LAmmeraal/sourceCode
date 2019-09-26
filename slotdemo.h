// slotdemo.h: Signals and slots.
#ifndef SLOTDEMO_H
#define SLOTDEMO_H
#include <QtWidgets>

class SlotDemo : public QMainWindow {
   Q_OBJECT
public:
   SlotDemo(QWidget *parent = nullptr);
protected:
   int xDeviationPerc;
   QPushButton buttonLeft, buttonRight;
   void paintEvent(QPaintEvent *e) override;
   void resizeEvent(QResizeEvent *e) override;
private slots:
   void handleButtonLeft();
   void handleButtonRight();
};
#endif // SLOTDEMO_H

// crankshaft.h: Animation in a scene.
#ifndef CRANKSHAFT_H
#define CRANKSHAFT_H
#include <QtWidgets>
using namespace std;

class MyScene : public QGraphicsScene {
   Q_OBJECT
public:
   qreal alpha, delta;
   const qreal w = 2000, h = 1000,
      r = w / 6, d = 2 * r,
      xC = -w / 4, yC = 0,
      xLeft = xC - r, yBottom = yC - r,
      a = 2.5 * r, b = r,
      aSq = a * a;
public slots:
   void update();
   void slow();
   void fast();
};

class Crankshaft : public QMainWindow {
   Q_OBJECT
public:
   Crankshaft(QWidget *parent = nullptr);
protected:
   QPushButton *pSlow, *pFast;
   QTransform D;
   qreal c;
   MyScene *pScene;
   QGraphicsView view;
};
#endif // CRANKSHAFT_H

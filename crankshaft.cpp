// crankshaft.cpp: Animation in a scene.
#include "crankshaft.h"

int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   Crankshaft w;
   w.show();
   return a.exec();
}

Crankshaft::Crankshaft(QWidget *parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int scrW = rec.width(), scrH = rec.height();
   qreal wD = 0.5 * scrW,
      hD = wD / 2, // Same ratio 2 : 1 as logical coordinates
      wMargin = 0.5 * (scrW - wD),
      hMargin = 0.5 * (scrH - hD);
   setGeometry(wMargin, hMargin, wD + 50, hD + 50);
   setWindowTitle("Crankshaft in motion");
   QHBoxLayout *pHLayout = new QHBoxLayout;
   QVBoxLayout *pVLayout = new QVBoxLayout;
   pSlow = new QPushButton("Slower");
   pFast = new QPushButton("Faster");
   pHLayout->addWidget(pSlow);
   pHLayout->addWidget(pFast);
   pScene = new MyScene;
   pScene->delta = M_PI / 45; // 4 degrees
   view.setScene(pScene);
   pVLayout->addWidget(&view);
   pVLayout->addLayout(pHLayout);
   qreal wL = pScene->w, hL = pScene->h,
      c = min(wD / wL, hD / hL);
   D = QTransform(c, 0, 0,
      0, -c, 0,  // y-axis pointing upward
      wD / 2, hD / 2, 1); // Origin in center
// With this 3 x 3 matrix D the following matrix
// multiplication applies to compute device coordinates
// (X, Y) from logical coordinates (x, y):
//    (X  Y  1) = (x  y  1)D
   view.setTransform(D);
   pScene->alpha = 0;
   pScene->addRect(-wL / 2, -hL / 2, wL, hL, QPen(Qt::white));
   QWidget *pW = new QWidget(this);
   pW->setLayout(pVLayout);
   setCentralWidget(pW);
   connect(pSlow, SIGNAL(released()), pScene, SLOT(slow()));
   connect(pFast, SIGNAL(released()), pScene, SLOT(fast()));
   QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), pScene, SLOT(update()));
   timer->start(100);
}

void MyScene::update() {
   alpha += delta;
   clear();
   qreal xP = xC + r * cos(alpha),
      yP = yC + r * sin(alpha),
      xA = xP + sqrt(aSq - yP * yP), xB = xA + b;
   QPen pen(Qt::black, 20);
   addEllipse(xLeft, yBottom, d, d, pen);
   addLine(xC, yC, xP, yP, pen);
   addLine(xP, yP, xA, 0, pen);
   addLine(xA, 0, xB, 0, pen);
}

void MyScene::slow() {
   delta *= 0.5;
}

void MyScene::fast() {
   delta *= 2;
}

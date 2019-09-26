// torus.cpp: Generating a torus.
#include "torus.h"

Torus::Torus(QWidget *parent) {
   setWindowTitle("Torus");
   newTorusAct = new QAction("New Torus", this);
   newTorusAct->setShortcut(Qt::CTRL | Qt::Key_N);
   connect(newTorusAct, &QAction::triggered, this,
      &Torus::newTorus);
   fileMenu->insertAction(openAct, newTorusAct);
   fileMenu->removeAction(openAct);
   fileMenu->removeAction(importAct);
   fileName = "torus.dat"; // For any output file
}

void Torus::newTorus() {
   if (thread.isRunning()) {
      interruptProcess();
   }
   bool ok;
   n = QInputDialog::getInt(this, "Torus",
      "Number of points on a circle          ",
      30, 3, 1000, 1, &ok);
   if (ok)
      R = QInputDialog::getDouble(this, "Torus",
         "Radius R of large circle           ",
         10, 0.01, 1000000, 2, &ok);
   if (ok)
      r = QInputDialog::getDouble(this, "Torus",
         "Radius r of small circles          ",
         R / 4, 0, R, 3, &ok);
   if (!ok)
      return;
   world.clear();
   polyList.clear();
   edges.clear();
   selection.clear();
   defineObject();
   boundsCenterDistance();
   drawImage();
}

void Torus::addQuad(int A, int B, int C, int D) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   polygon.nrs.push_back(D);
   polyList.push_back(polygon);
}

void Torus::defineObject() {
   qreal delta = 2 * M_PI / n;
   // Vertices:
   world.clear();
   world.push_back(Point3D(1e30, 0, 0));
   // Dummy: first real vertex number is 1.
   for (int i = 0; i < n; i++) {
      qreal alpha = i * delta, cosa = cos(alpha), sina = sin(alpha);
      for (int j = 0; j < n; j++) {
         qreal beta = j * delta, x = R + r * cos(beta),
            x1 = cosa * x, y1 = sina * x, z1 = r * sin(beta);
         world.push_back(Point3D(x1, y1, z1));
      }
   }
   // Faces:
   for (int i = 0; i < n; i++) {
      int i1 = (i + 1) % n;
      for (int j = 0; j < n; j++) {
         int j1 = (j + 1) % n,
            A = i * n + j + 1, B = i1 * n + j + 1,
            C = i1 * n + j1 + 1, D = i * n + j1 + 1;
         addQuad(A, B, C, D);
      }
   }
}

void Torus::paintEvent(QPaintEvent *e) {
   if (starting) { // Do this only once
      starting = false;
      newTorus();
   }
   else
      Edit3D::paintEvent(e);
}

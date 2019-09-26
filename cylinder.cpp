// cylinder.cpp: Approximated (hollow) cylinder.
#include "cylinder.h"

Cylinder::Cylinder(QWidget *parent) {
   setWindowTitle("Prism or cylinder");
   newCylinderAct = new QAction("New Cylinder", this);
   newCylinderAct->setShortcut(Qt::CTRL | Qt::Key_N);
   connect(newCylinderAct, &QAction::triggered, this,
      &Cylinder::newCylinder);
   fileMenu->insertAction(openAct, newCylinderAct);
   fileMenu->removeAction(openAct);
   fileMenu->removeAction(importAct);
   fileName = "cylinder.dat"; // Used for output file
}

void Cylinder::newCylinder() {
   if (thread.isRunning())
      interruptProcess();
   bool ok;
   double D, d;
   n = QInputDialog::getInt(this, "Cylinder",
      "Number of points on a circle                  ",
      30, 3, 10000, 1, &ok);
   if (ok)
      D = QInputDialog::getDouble(this, "Cylinder",
         "Outside diameter                           ",
         2, 0.02, 1000000, 2, &ok);
   if (ok)
      d = QInputDialog::getDouble(this, "Cylinder",
         "Inside diameter (or 0 for solid cylinder)  ",
         D / 2, 0, D, 3, &ok);
   if (ok)
      h = QInputDialog::getDouble(this, "Cylinder",
         "Cylinder height                             ",
         D / 2, 0.0, 1000000, 2, &ok);
   if (!ok)
      return;
   R = D / 2;
   r = d / 2;
   world.clear();
   polyList.clear();
   edges.clear();
   selection.clear();
   defineObject();
   boundsCenterDistance();
   drawImage();
}

void Cylinder::addQuad(int A, int B, int C, int D) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   polygon.nrs.push_back(D);
   polyList.push_back(polygon);
}

void Cylinder::addTria(int A, int B, int C) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   polyList.push_back(polygon);
}

void Cylinder::defineObject() {
   qreal delta = 2 * M_PI / n, thickness = R - r;
   world.clear();
   world.push_back(Point3D(1e30, 0, 0));  // Dummy
      // Dummy: first real vertex number is 1.
   int nRadii = (r > 0 ? 2 : 1);
   for (int iRadius = 0; iRadius < nRadii; iRadius++) {
      qreal radius = R - iRadius * thickness; // R or r
      for (int iLevel = 1; iLevel >= 0; iLevel--) {
         qreal z = iLevel * h;
         for (int iAngle = 0; iAngle < n; iAngle++) {
            qreal alpha = iAngle * delta,
               cosa = cos(alpha), sina = sin(alpha),
               x = radius * cosa, y = radius * sina;
            world.push_back(Point3D(x, y, z));
         }
      }
   }
   if (nRadii == 1) { // Solid cylinder, no hole
      // Centers of circles added as vertices 
      // for better and faster triangulation:
      world.push_back(Point3D(0, 0, h));
      world.push_back(Point3D(0, 0, 0));
   }

   // Outer faces:
   for (int i = 1; i < n; i++)
      addQuad(i, n + i, n + i + 1, i + 1);
   addQuad(n, 2 * n, n + 1, 1);

   if (r > 0) { // Cylindrical hole
      for (int i = 2 * n + 1; i < 3 * n; i++)
         addQuad(i, i + 1, i + n + 1, i + n);
      addQuad(3 * n, 2 * n + 1, 3 * n + 1, 4 * n);
   }
   if (r > 0) { // Non-zero diameter of hole
      // Top face:
      for (int i = 0; i < n - 1; i++)
         addQuad(i + 2, -(i + 2 + 2 * n), i + 1 + 2 * n,
            -(i + 1));
      addQuad(1, -(2 * n + 1), 3 * n, -n);

      // Bottom face:
      for (int i = 0; i < n - 1; i++)
         addQuad(2 * n - i - 1, -(4 * n - i - 1), 4 * n - i,
            -(2 * n - i));
      addQuad(2 * n, -4 * n, 3 * n + 1, -(n + 1));
   }
   else {
      int south = world.size() - 1, north = south - 1;
      // Top face:
      for (int i = 1; i < n; i++)
         addTria(i + 1, -north, -i);
      addTria(1, -north, -n);
      // Bottom face:
      for (int i = 1; i < n; i++)
         addTria(n + i, -south, -(n + i + 1));
      addTria(2 * n, -south, -(n + 1));
   }
}

void Cylinder::paintEvent(QPaintEvent *e) {
   if (starting) { // Do this only once
      starting = false;
      newCylinder();
   }
   else 
      Edit3D::paintEvent(e);
}

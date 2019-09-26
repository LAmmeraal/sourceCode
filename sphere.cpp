// sphere.cpp: Globe model of sphere.
#include "sphere.h"

Sphere::Sphere(QWidget *parent) {
   setWindowTitle("Globe model of sphere");
   newSphereAct = new QAction("New Sphere", this);
   newSphereAct->setShortcut(Qt::CTRL | Qt::Key_N);
   connect(newSphereAct, &QAction::triggered, this,
      &Sphere::newSphere);
   fileMenu->insertAction(openAct, newSphereAct);
   fileMenu->removeAction(openAct);
   fileMenu->removeAction(importAct);
   fileName = "sphere.dat"; // Used for output file
}

void Sphere::newSphere() {
   if (thread.isRunning())
      interruptProcess();
   bool ok;
   n = QInputDialog::getInt(this, "Sphere",
      "Enter n (180 degrees is divided by n): ",
      20, 2, 10000, 1, &ok);
   if (!ok)
      return;
   defineObject();
   boundsCenterDistance();
   drawImage();
}

void Sphere::addTria(int A, int B, int C) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   polyList.push_back(polygon);
}

void Sphere::addQuad(int A, int B, int C, int D) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   polygon.nrs.push_back(D);
   polyList.push_back(polygon);
}

void Sphere::defineObject() { // Sphere with radius R = 1
   qreal delta = M_PI / n;
   world.clear();
   polyList.clear();
   edges.clear();
   selection.clear();
   world.push_back(Point3D(1e30, 0, 0));  // Dummy
   world.push_back(Point3D(0, 0, 1));     // North pole
   for (int i = 1; i < n; i++) {
      qreal phi = i * delta, // Angle with vertical axis
         r = sin(phi),    // Radius of horizontal circle
         z = cos(phi);
      for (int j = 0; j < 2 * n; j++) {
         qreal theta = j * delta, // Angle with plane y = 0
            x = r * cos(theta), y = r * sin(theta);
         world.push_back(Point3D(x, y, z));
      }
   }
   world.push_back(Point3D(0, 0, -1));     // South pole

   // 2n triangles at the north pole:
   int N = 2 * n;
   for (int j = 0; j < N - 1; j++)
      addTria(j + 2, j + 3, 1);
   addTria(N + 1, 2, 1);

   // For each quadrangle ABCD, side AB lies on line of latitude i,
   // while CD lies on line of magnitude i - 1 (i=2, 3,...,n-1):
   for (int i = 2; i < n; i++) {
      int first = 2 * (i - 1) * n + 2;
      for (int j = 0; j < N - 1; j++) {
         int A = first + j, B = A + 1, C = B - N, D = C - 1;
         addQuad(A, B, C, D);
      }
      // Vertex B of the last quadrangle coincides with vertex A
      // of the first one:
      int A = first + N - 1, B = first, C = first - N, D = A - N;
      addQuad(A, B, C, D);
   }

   // 2n triangles at the south pole:
   int B = 2 * (n - 1) * n + 2;
   int first = 2 * (n - 2)*n + 2;
   for (int j = 0; j < N - 1; j++) {
      int A = first + j, C = A + 1;
      addTria(A, B, C);
   }
   int A = first + N - 1, C = first;
   addTria(A, B, C);
}

void Sphere::paintEvent(QPaintEvent *e) {
   if (starting) { // Do this only once
      starting = false;
      newSphere();
   }
   else
      Edit3D::paintEvent(e);
}

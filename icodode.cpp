// icodode.cpp: Icosahedron and dodecahedron.
#include "icodode.h"

IcoDode::IcoDode(int mode, QWidget *parent) {
   setWindowTitle("Icosahedron and dodecahedron");
   fileMenu->clear();
   onlyIcosaAct = new QAction("Show only icosahedron",
      this);
   onlyDodecaAct = new QAction("Show only dodecahedron",
      this);
   bothAct = new QAction("Show both icosahedron and dodecahedron",
      this);
   objects = new QActionGroup(this);
   objects->addAction(bothAct);
   objects->addAction(onlyIcosaAct);
   objects->addAction(onlyDodecaAct);
   bothAct->setCheckable(true);
   onlyIcosaAct->setCheckable(true);
   onlyDodecaAct->setCheckable(true);
   fileMenu->addAction(bothAct);
   bothAct->setChecked(true);
   fileMenu->addAction(onlyIcosaAct);
   fileMenu->addAction(onlyDodecaAct);
   fileMenu->addSeparator();
   fileMenu->addAction(writeDataFileAct);
   fileMenu->addAction(exportAct);
   fileMenu->addAction(exitAct);

   connect(onlyIcosaAct, &QAction::triggered, this,
      &IcoDode::refresh);
   connect(onlyDodecaAct, &QAction::triggered, this,
      &IcoDode::refresh);
   connect(bothAct, &QAction::triggered, this,
      &IcoDode::refresh);

   fileName = "icodode.dat"; // Default output file name
   defineObject(mode);
   boundsCenterDistance();
   drawImage();
}

void IcoDode::triaFace(int A, int B, int C, int centroid) {
   if (!onlyDodecaAct->isChecked()) {
      Polygon3D polygon;
      polygon.nrs.push_back(A);
      polygon.nrs.push_back(B);
      polygon.nrs.push_back(C);
      polyList.push_back(polygon);
   }

   // Centroid:
   qreal X = (world[A].x + world[B].x + world[C].x) / 3,
      Y = (world[A].y + world[B].y + world[C].y) / 3,
      Z = (world[A].z + world[B].z + world[C].z) / 3;
   world[12 + centroid] = Point3D(X, Y + offsetY, Z);
}

void IcoDode::addPentaFace(int A, int B, int C, int D, int E) {
   Polygon3D polygon;
   polygon.nrs.push_back(A + offsetNr);
   polygon.nrs.push_back(B + offsetNr);
   polygon.nrs.push_back(C + offsetNr);
   polygon.nrs.push_back(D + offsetNr);
   polygon.nrs.push_back(E + offsetNr);
   polyList.push_back(polygon);
}

void IcoDode::defineObject(int mode) {
   const qreal angle36 = 0.2 * M_PI, angle72 = 0.4 * M_PI,
      root5 = sqrt(5), hroot5 = 0.5 * root5;
   world.clear();
   offsetY = 0;
   if (bothAct->isChecked()) {
      offsetY = 2.5; // Display dodeca beside icosa
   }
   world.resize(33);
   // 0: dummy, 1-12: icosahedron, 13-32: dodecahedron
   polyList.clear();
   selection.clear();
   world[0] = Point3D(1e30, 0, 0);  // Dummy

   // Icosahedron:
   world[1] = Point3D(0, 0, hroot5);   // North pole
   for (int i = 2; i <= 6; i++) {
      qreal alpha = (i - 2)*angle72;
      world[i] = Point3D(cos(alpha), sin(alpha), 0.5);
   }
   for (int i = 7; i <= 11; i++) {
      qreal alpha = angle36 + (i - 7) * angle72;
      world[i] = Point3D(cos(alpha), sin(alpha), -0.5);
   }
   world[12] = Point3D(0, 0, -hroot5);   // South pole

   for (int i = 1; i <= 5; i++) {
      triaFace(1, i + 1, i % 5 + 2, i);
      // 1:{1,2,3}, 2:{1,3,4}, 3:{1,4,5}, 4:{1,5,6}, 5:{1,6,2}
   }

   for (int i = 1; i <= 5; i++) {
      triaFace(i + 1, i + 6, i % 5 + 2, 4 + 2 * i);
      // 6:{2,7,3}, 8:{3,8,4}, 10:{4,9,5}, 12:{5,10,6}, 14:{6,11,2}
      triaFace(i % 5 + 2, i + 6, i % 5 + 7, 5 + 2 * i);
      // 7:{3,7,8}, 9:{4,8,9}, 11:{5,9,10}, 13:{6,10,11},
      // 15:{2,11,7}
   }
   for (int i = 1; i <= 5; i++) {
      triaFace(12, i % 5 + 7, i + 6, 15 + i);
      // 16:{12,8,7}, 17:{12,9,8}, 18:{12,10,9}, 19:{12,11,10},
      // 20{12,7,11}
   }
   if (mode == 1 || onlyIcosaAct->isChecked()) {
      world.resize(13);
      return;
   }
   // Now for the 12 dodecahedron faces:
   offsetNr = 12;
   if (onlyDodecaAct->isChecked()) {
      for (int i = 1; i <= 20; i++)
         world[i] = world[i + 12];
      offsetNr = 0;
      world.resize(21); // 0, 1,...,20
   }
   addPentaFace(1, 2, 3, 4, 5);
   addPentaFace(1, 6, 7, 8, 2);
   addPentaFace(2, 8, 9, 10, 3);
   addPentaFace(3, 10, 11, 12, 4);
   addPentaFace(4, 12, 13, 14, 5);
   addPentaFace(5, 14, 15, 6, 1);
   addPentaFace(20, 19, 18, 17, 16);
   addPentaFace(20, 15, 14, 13, 19);
   addPentaFace(19, 13, 12, 11, 18);
   addPentaFace(18, 11, 10, 9, 17);
   addPentaFace(17, 9, 8, 7, 16);
   addPentaFace(16, 7, 6, 15, 20);
}

void IcoDode::refresh() {
   defineObject();
   boundsCenterDistance();
   drawImage();
}



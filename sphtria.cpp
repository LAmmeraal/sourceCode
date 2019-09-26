// sphtria.cpp: Sphere approximated by triangles.
#include "sphtria.h"

SphTria::SphTria(QWidget *parent) : IcoDode(1, nullptr) {
   // 1 in the IcoDode constructor call: only icosahedron
   setWindowTitle("Sphere approximated by triangles");
   improveAct = new QAction("Improve sphere", this);
   connect(improveAct, &QAction::triggered, this,
      &SphTria::improve);
   newIcosaAct = new QAction("Back to icosahedron", this);
   connect(newIcosaAct, &QAction::triggered, this,
      &SphTria::newIcosa);
   fileMenu->clear();
   fileMenu->addAction(improveAct);
   fileMenu->addAction(interruptAct);
   fileMenu->addAction(newIcosaAct);
   fileMenu->addAction(writeDataFileAct);
   fileMenu->addAction(exportAct);
   fileMenu->addAction(exitAct);
   scaleToUnitRadius();
   fileName = "sphtria.dat"; // Used for any output file
}

void SphTria::scaleToUnitRadius() {
   // Scale such that all vertices lie on a sphere with radius 1:
   qreal f = 1 / world[1].z; // 1: north pole; 12: south pole
   for (int i = 1; i <= 12; i++) {
      world[i].x *= f; world[i].y *= f; world[i].z *= f;
   }
}

void SphTria::improve() { // Improve sphere model
   qreal f; // scale factor for midpoints
   int nFaces = polyList.size();
   for (int i = 0; i < nFaces; i++) {
      Polygon3D polygon = polyList[i];
      int iA = polygon.nrs[0], iB = polygon.nrs[1],
         iC = polygon.nrs[2];
      Point3D A = world[iA], B = world[iB], C = world[iC];
      if (i == 0) {
         qreal x = (B.x + C.x) / 2, y = (B.y + C.y) / 2,
            z = (B.z + C.z) / 2;
         f = 1 / sqrt(x * x + y * y + z * z);
      }
      Point3D
         D(f * (B.x + C.x) / 2, f * (B.y + C.y) / 2,
            f * (B.z + C.z) / 2),
         E(f * (C.x + A.x) / 2, f * (C.y + A.y) / 2,
            f * (C.z + A.z) / 2),
         F(f * (A.x + B.x) / 2, f * (A.y + B.y) / 2,
            f * (A.z + B.z) / 2);
      int iD = world.size(), iE = iD + 1, iF = iE + 1;
      world.push_back(D);
      world.push_back(E);
      world.push_back(F);
      // Replace ABC with DEF:
      polygon.nrs[0] = iD;
      polygon.nrs[1] = iE;
      polygon.nrs[2] = iF;
      polyList[i] = polygon;
      // Add AFE, BDF, CED at the end of polyList
      polygon.nrs[0] = iA;
      polygon.nrs[1] = iF;
      polygon.nrs[2] = iE;
      polyList.push_back(polygon);
      polygon.nrs[0] = iB;
      polygon.nrs[1] = iD;
      polygon.nrs[2] = iF;
      polyList.push_back(polygon);
      polygon.nrs[0] = iC;
      polygon.nrs[1] = iE;
      polygon.nrs[2] = iD;
      polyList.push_back(polygon);
   }
   boundsCenterDistance();
   drawImage();
}

void SphTria::newIcosa() {
   defineObject();
   boundsCenterDistance();
   drawImage();
}

void SphTria::defineObject() {
   IcoDode::defineObject(1);
   scaleToUnitRadius();
}

void SphTria::addToThreadStart() {
   improveAct->setEnabled(false);
   newIcosaAct->setEnabled(false);
   Edit3D::addToThreadStart();
}

void SphTria::addToThreadFinish() {
   improveAct->setEnabled(true);
   newIcosaAct->setEnabled(true);
   Edit3D::addToThreadFinish();
   if (interrupted)
      newIcosa();
}

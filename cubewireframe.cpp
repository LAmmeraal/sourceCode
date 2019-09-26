// cubewireframe.cpp: Cube (wireframe).
#include "cubewireframe.h"

CubeWireframe::CubeWireframe(QWidget *parent) {
   setWindowTitle("Cube as a wireframe");
   fileMenu->removeAction(openAct);
}

void CubeWireframe::defineObject() {
   // Bottom face, z = -1:
   world.push_back(Point3D(+1, -1, -1)); // 0
   world.push_back(Point3D(+1, +1, -1)); // 1
   world.push_back(Point3D(-1, +1, -1)); // 2
   world.push_back(Point3D(-1, -1, -1)); // 3

   // Top face, z = +1:
   world.push_back(Point3D(+1, -1, +1)); // 4
   world.push_back(Point3D(+1, +1, +1)); // 5
   world.push_back(Point3D(-1, +1, +1)); // 6
   world.push_back(Point3D(-1, -1, +1)); // 7

   edges.insert(IntPair(0, 1)); // Bottom
   edges.insert(IntPair(1, 2));
   edges.insert(IntPair(2, 3));
   edges.insert(IntPair(3, 0));

   edges.insert(IntPair(4, 5)); // Top
   edges.insert(IntPair(5, 6));
   edges.insert(IntPair(6, 7));
   edges.insert(IntPair(7, 4));

   edges.insert(IntPair(0, 4)); // Vertical
   edges.insert(IntPair(1, 5));
   edges.insert(IntPair(2, 6));
   edges.insert(IntPair(3, 7));

   rhoMin = sqrt(12);
   // = sqrt(xrange * xrange + yrange * yrange + zrange * zrange),
   // where xrange = yrange = zrange = 1 - (-1) = 2
   rho0 = rho = 3 * rhoMin;
}

void CubeWireframe::paintEvent(QPaintEvent *e) {
   if (edges.size() == 0) {
      defineObject(); // Do this only once
      boundsCenterDistance();
   }
   computePerspCoord();
   QPainter pnt(this);
   pnt.setPen(QPen(Qt::black, 2));

   for (EdgeSet::iterator k = edges.begin(); k != edges.end(); k++) {
      int i = (*k).i, j = (*k).j;
      pnt.drawLine(xDev(scr[i].x), yDev(scr[i].y),
         xDev(scr[j].x), yDev(scr[j].y));
   }
}



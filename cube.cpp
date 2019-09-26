// cube.cpp: Generating a cube.
#include "cube.h"

Cube::Cube(QWidget *parent) {
   setWindowTitle("Cube");
   fileMenu->removeAction(openAct);
   fileName = "cube.dat";
   defineObject();
   drawImage();
}

void Cube::addFace(int A, int B, int C, int D) {
   Polygon3D polygon;
   polygon.nrs.push_back(A);
   polygon.nrs.push_back(B);
   polygon.nrs.push_back(C);
   polygon.nrs.push_back(D);
   polyList.push_back(polygon);
}

void Cube::defineObject() {
   world.push_back(Point3D(1e30, 0, 0)); // Dummy
   // Bottom face, z = -1:
   world.push_back(Point3D(+1, -1, -1)); // 1
   world.push_back(Point3D(+1, +1, -1)); // 2
   world.push_back(Point3D(-1, +1, -1)); // 3
   world.push_back(Point3D(-1, -1, -1)); // 4

   // Top face, z = +1:
   world.push_back(Point3D(+1, -1, +1)); // 5
   world.push_back(Point3D(+1, +1, +1)); // 6
   world.push_back(Point3D(-1, +1, +1)); // 7
   world.push_back(Point3D(-1, -1, +1)); // 8

   boundsCenterDistance();

   addFace(5, 6, 7, 8); // top face
   addFace(1, 2, 6, 5); // front face
   addFace(2, 3, 7, 6); // right face
   addFace(3, 4, 8, 7); // back face
   addFace(4, 1, 5, 8); // left face
   addFace(4, 3, 2, 1); // bottom face
}

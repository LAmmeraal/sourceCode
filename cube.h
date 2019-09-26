// cube.h: Generating a cube.
#ifndef CUBE_H
#define CUBE_H
#include "edit3d.h"

class Cube : public Edit3D {
   Q_OBJECT
public:
   Cube(QWidget *parent = nullptr);
protected:
   void defineObject();
   void addFace(int A, int B, int C, int D);
};
#endif // CUBE_H

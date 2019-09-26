// sphere.h: Globe model of sphere.
#ifndef SPHERE_H
#define SPHERE_H
#include "edit3d.h"

class Sphere : public Edit3D {
   Q_OBJECT
public:
   Sphere(QWidget *parent = nullptr);
protected:
   bool starting = true;
   int n;
   QAction *newSphereAct;
   void paintEvent(QPaintEvent *e)override;
   void defineObject();
   void addTria(int A, int B, int C);
   void addQuad(int A, int B, int C, int D);
protected slots:
   void newSphere();
};
#endif // SPHERE_H

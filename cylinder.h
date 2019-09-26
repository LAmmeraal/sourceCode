// cylinder.h: Approximated (hollow) cylinder.
#ifndef CYLINDER_H
#define CYLINDER_H
#include "edit3d.h"

class Cylinder : public Edit3D {
   Q_OBJECT
public:
   Cylinder(QWidget *parent = nullptr);
protected:
   bool starting = true;
   int n; // Number of vertices on a circle
   qreal r, R, h;
   QAction *newCylinderAct;
   void paintEvent(QPaintEvent *e)override;
   void defineObject();
   void addQuad(int A, int B, int C, int D);
   void addTria(int A, int B, int C);
protected slots:
   void newCylinder();
};
#endif // CYLINDER_H

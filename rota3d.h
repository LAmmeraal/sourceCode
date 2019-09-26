// rota3d.h: Rotation about arbitrary axis.
#include "points.h"

class Rota3D {
   qreal r11, r12, r13, r21, r22, r23, r31, r32, r33, r41, r42, r43;
public:
   Rota3D(const Point3D &a, const Point3D &b, qreal alpha);
   void rotate(const Point3D &p, Point3D &p1);
};

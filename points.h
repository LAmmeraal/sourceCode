// points.h: Types Point2D and Point3D.
#ifndef POINTS_H
#define POINTS_H
#include <QtWidgets>
using namespace std;

struct Point2D {
   qreal x, y;
   Point2D(qreal x = 0, qreal y = 0) {
      this->x = x; this->y = y;
   }
};

struct Point3D {
   qreal x, y, z;
   Point3D(qreal x = 0, qreal y = 0, qreal z = 0) {
      this->x = x; this->y = y; this->z = z;
   }
};
#endif // POINTS_H
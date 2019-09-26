// hfaces.h: Hidden faces, Z-buffer algorithm.
#ifndef HFACES_H
#define HFACES_H
#include "basic3d.h"
#include "triangulate.h"

class HFaces : public Basic3D {
   Q_OBJECT
public:
   HFaces(QWidget *parent = nullptr);
protected:
   int wbuf, hbuf = 0;
   float **buf;
   qreal sunZ = 1 / sqrt(3), sunY = sunZ, sunX = -sunZ,
      inprodMin, inprodMax, inprodRange;
   vector<Tria> tria;    // triangles of all polygons
   vector<int> polnrs;   // polygon numbers of triangles in tria
   void paintEvent(QPaintEvent *e) override;   
   void allocatebuffer();
   int colorCode(qreal a, qreal b, qreal c)const;
   inline int xDev(qreal x)const {
      return xDevCenter + int(x - xsCenter);
   }
   inline int yDev(qreal y)const {
      return yDevCenter - int(y - ysCenter);
   }
};
#endif // HFACES_H

// wireframe.cpp: Wireframe models.
#include "wireframe.h"

Wireframe::Wireframe(QWidget *parent) {
   setWindowTitle("Wireframe model");
}

void Wireframe::paintEvent(QPaintEvent *e) {
   newObjectTest();
   QPainter pnt(this);
   pnt.setPen(QPen(Qt::black, 2));
   edges.clear();
   for (int i = 0; i < polyList.size(); i++) {
      Polygon3D polygon = polyList[i];
      // Put all successive vertex pairs (abs(pol(k)), next),
      // where next = pol((k+1) % pol.size(),
      // in the vector 'edges':
      int n = polygon.nrs.size();
      for (int k = 0; k < n; k++) {
         int from = polygon.nrs[k], k1 = (k + 1) % n,
            to = polygon.nrs[k1];
         if (to >= 0)
            edges.insert(IntPair(abs(from), to));
      }
   }
   for (EdgeSet::iterator k = edges.begin();
      k != edges.end(); k++) {
      int from = (*k).i, to = (*k).j;
      pnt.drawLine(xDev(scr[from].x), yDev(scr[from].y),
         xDev(scr[to].x), yDev(scr[to].y));
   }
}

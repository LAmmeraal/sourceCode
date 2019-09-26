// drawpolygontrias.h: Demonstration of triangulation.
#ifndef DRAWPOLYGONTRIAS_H
#define DRAWPOLYGONTRIAS_H
#include "drawpolygon.h"
#include "triangulate.h"

class DrawPolygonTrias : public DrawPolygon {
   Q_OBJECT
protected:
   vector<Tria> trias;
   int cnt = 1;
   void paintEvent(QPaintEvent *e) override;
   void mousePressEvent(QMouseEvent *event) override;
   void drawTriangles(QPainter &painter,
      const vector<Tria> &tria, const vector <Point2D> &v)const;
};
#endif // DRAWPOLYGONTRIAS_H

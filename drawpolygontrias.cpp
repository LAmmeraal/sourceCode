// drawpolygontrias.cpp: Demonstration of triangulation.
#include "drawpolygontrias.h"

void DrawPolygonTrias::mousePressEvent(QMouseEvent *event) {
   DrawPolygon::mousePressEvent(event);
   if (ready && v.size() > 2) {
      vector<int> pol;
      for (int i = 0; i < v.size(); i++)
         pol.push_back(i);
      if (!Triangulate::isCounterclockwise(v, pol))
         for (int i = 0; i < v.size(); i++)
            pol[i] = v.size() - 1 - i;
      if (!Triangulate::triangulate(v, pol, trias)) {
         QMessageBox msgBox(this);
         msgBox.setText("This is not a simple polygon."); 
         msgBox.exec();
      }
      cnt++; // For variation in colors; see paint event
      repaint();
   }
}

void DrawPolygonTrias::paintEvent(QPaintEvent *e) {
   setWindowTitle("Triangulation of polygons");
   DrawPolygon::paintEvent(e);
   if (v.size() == 0) // Clear any previously
      trias.clear();  // generated triangles
   if (ready && trias.size() > 0) {
      QPainter painter(this);
      painter.setTransform(D);
      drawTriangles(painter, trias, v);
   }
}

void DrawPolygonTrias::drawTriangles(QPainter &painter,
   const vector<Tria> &tria, const vector <Point2D> &v)const {
   for (int i = 0; i < tria.size(); i++) {
      Tria tr = tria[i];
      painter.setPen(QPen(Qt::black, 3));
      int cnt1 = cnt * (i + 1);
      int r = 17 * cnt1 % 256,
         g = 67 * cnt1 % 256,
         b = 97 * cnt1 % 256;
      painter.setBrush(QColor(r, g, b, 255));
      Point2D t0[] = { v[tr.A], v[tr.B], v[tr.C] };
      QPointF t[3];
      for (int j = 0; j < 3; j++)
         t[j] = QPointF(t0[j].x, t0[j].y);
      painter.drawPolygon(t, 3);
   }
}

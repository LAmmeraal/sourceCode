// drawpolygon.h: Drawing a polygon.
#ifndef DRAWPOLYGON_H
#define DRAWPOLYGON_H
#include <QtWidgets>
#include "points.h"
using namespace std;

class DrawPolygon : public QMainWindow {
   Q_OBJECT
public:
   DrawPolygon(QWidget *parent = nullptr);
protected:
   void showCoords(QWidget *p)const;
   int W, H;
   qreal w, h, c;
   bool outsideLogicalBoundaries(const Point2D &P)const;
   Point2D Pstart;
   bool ready, start;
   int xpos, ypos;
   QTransform D, L;
   vector<Point2D> v;
   void paintEvent(QPaintEvent *e) override;
   void resizeEvent(QResizeEvent *e) override;
   void mousePressEvent(QMouseEvent *event) override;
   void mouseMoveEvent(QMouseEvent *event) override;
   void keyPressEvent(QKeyEvent *) override;
   bool samePoint(const Point2D &P, const Point2D &Q)const;
   QString str(qreal x)const;
};
#endif // DRAWPOLYGON_H

// drawpolygon.cpp: Drawing a polygon.
#include "drawpolygon.h"

DrawPolygon::DrawPolygon(QWidget *parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int scrW = rec.width(), scrH = rec.height();
   setGeometry(scrW / 10, scrH / 10,
      8 * scrW / 10, 8 * scrH / 10);
   setWindowTitle("Define polygon");
   ready = false;
   setCursor(Qt::CrossCursor);
   setMouseTracking(true);
   start = true;
}

void DrawPolygon::resizeEvent(QResizeEvent *e) {
   w = 600;       // Logical coordinate
   h = 400;       // ranges
   W = width();   // Current device coordinate
   H = height();  // ranges (pixels)
   c = min(W / w, H / h);
   D = QTransform(c, 0, 0,
      0, -c, 0,  // y-axis pointing upward
      W / 2, H / 2, 1); // Origin in center
// With this 3 x 3 matrix D the following matrix multiplication
// applies to compute device coordinates (X, Y) from logical
// coordinates (x, y):
// (X  Y  1) = (x  y  1)D
   L = D.inverted();
}

bool DrawPolygon::samePoint(const Point2D &P,
   const Point2D &Q)const {
   qreal dx = P.x - Q.x, dy = P.y - Q.y;
   return dx * dx + dy * dy < 50;
}

void DrawPolygon::keyPressEvent(QKeyEvent *event) {
   if (event->key() == Qt::Key_Escape) {
      v.clear();
      ready = false;
      repaint();
   }
}

QString DrawPolygon::str(qreal x)const {
   return QString::number(x);
}

void DrawPolygon::showCoords(QWidget *p)const {
   QMessageBox msgBox(p);
   msgBox.setWindowTitle("Vertices");
   QString s;
   for (unsigned int i = 0; i < v.size(); i++) {
      Point2D P = v[i];
      s += str(i) + "   " + str(P.x) + " " + str(P.y) + "\n";
   }
   msgBox.setText(s);
   msgBox.exec();
}

bool DrawPolygon::outsideLogicalBoundaries(const Point2D &P)const {
   qreal x = P.x, y = P.y;
   return x < -w / 2 || x > w / 2 || y < -h / 2 || y > h / 2;
}

void DrawPolygon::mousePressEvent(QMouseEvent *event) {
   if (event->button() == Qt::LeftButton) {
      QPointF qP = L.map(event->pos());
      Point2D P(qP.x(), qP.y());
      if (outsideLogicalBoundaries(P))
         return;
      if (v.size() > 0 && samePoint(P, v[0])) {
         ready = true;
      }
      if (!ready)
         v.push_back(P);
   }
   else
      showCoords(this);
   repaint();
}

void DrawPolygon::mouseMoveEvent(QMouseEvent *event) {
   QPointF pos = L.map(event->pos());
   xpos = pos.x();
   ypos = pos.y();
   repaint();
}

void DrawPolygon::paintEvent(QPaintEvent *e) {
   if (start) {
      QMessageBox msgBox(this);
      msgBox.setText(
         "Click the left mouse button to define polygon vertices,\n"
         "click the right mouse button to show vertex coordinates,\n"
         "or  the Esc key to clear the window.");
      msgBox.exec();
      start = false;
   }
   QPainter painter(this);
   QString s = "(" + str(xpos) + ", " + str(ypos) + ")";
   painter.drawText(10, 20, s);
   int xP, yP;

   // Drawing crosses and vertex numbers:
   for (int i = 0; i < v.size(); i++) {
      QPointF q(v[i].x, v[i].y), qP = D.map(q);
      xP = qP.x();
      yP = qP.y();
      painter.drawLine(xP - 5, yP, xP + 5, yP);
      painter.drawLine(xP, yP - 5, xP, yP + 5);
      painter.drawText(xP + 10, yP + 10, str(i));
   }
   painter.setTransform(D);
   // Here we will use logical coordinates
   for (int i = 0; i < v.size(); i++) {
      int xQ = int(v[i].x), yQ = int(v[i].y);
      if (i > 0) {
         painter.drawLine(xP, yP, xQ, yQ);
      }
      xP = xQ;
      yP = yQ;
   }
   if (ready)
      painter.drawLine(xP, yP, v[0].x, v[0].y);
   // Show the rectangle with opposite corners (-w/2, -h/2)
   // and (w/2, h/2):
   painter.drawRect(int(-w / 2), int(-h / 2), int(w), int(h));
}

// basic3d.h: Base class for 3D classes.
#ifndef BASIC3D_H
#define BASIC3D_H
#include <QtWidgets>
#include "points.h"
#include <set>
using namespace std;

struct IntPair {
   int i, j;
   IntPair(int i, int j) {
      if (i < j) {
         this->i = i; this->j = j;
      }
      else {
         this->i = j; this->j = i;
      }
   }
   bool operator<(const IntPair &x)const {
      return this->i < x.i || (this->i == x.i && this->j < x.j);
   }
   bool operator==(const IntPair &x)const {
      return this->i == x.i && this->j == x.j;
   }
};

struct Polygon3D {
   vector<int> nrs;    // Vertex numbers (n elements)
   qreal a, b, c, h;   // of the plane equation ax + by + cz = h
};

typedef set<IntPair, less<IntPair> >EdgeSet;

class Basic3D : public QMainWindow {
   Q_OBJECT
protected:
   Basic3D(QWidget *parent = 0);
   qreal xsMin, xsMax, ysMin, ysMax, xsRange = 0, ysRange,
      theta0Deg = 30, theta0 = theta0Deg * M_PI / 180,
      thetaDeg = theta0Deg, theta = theta0,  // Horizontal
      phi0Deg = 70, phi0 = phi0Deg * M_PI / 180,
      phiDeg = phi0Deg, phi = phi0,        // Vertical
      xwMin, xwMax, ywMin, ywMax, zwMin, zwMax,
      xC, yC, zC, rho, rho0, rhoMin, rhoMax, d;
   const int hMenu = 21;
   int wDraw, hDraw,  // Drawing in wDraw x hDraw pixels
      xDevCenter, yDevCenter;
   bool fileDialogDone = false;
   QString fileName = "";
   QMenu *viewMenu, *fileMenu;
   QAction *openAct, *exitAct, *incrDistAct, *decrDistAct,
      *resetAct, *angleChangeAct;
   int xPress, yPress;
   void newObjectTest();
   void computePerspCoord();
   void boundsCenterDistance();
   qreal xsCenter, ysCenter;
   void inputObject();
   inline int xDev(qreal x)const {
      return xDevCenter + int(d * (x - xsCenter));
   }
   inline int yDev(qreal y)const {
      return yDevCenter - int(d * (y - ysCenter));
   }
   bool different(int i, int j) const;
   void mousePressEvent(QMouseEvent *event) override;
   void mouseReleaseEvent(QMouseEvent *e) override;
   void resizeEvent(QResizeEvent *e) override;
public:
   void simpleMessage(const QString &title, const QString &mes);
   vector<Point3D> world, eye;
   vector<Point2D> scr;
   EdgeSet edges;
   vector <Polygon3D> polyList;
   bool defaultPaint = true; // Can be overwritten in derived class
   qreal zNear, zFar, eps1;  // Used in HLines
public slots:
   void open();
   void incrDist();
   void decrDist();
   void resetViewpoint();
   void angleChange();
};
#endif // BASIC3D_H

// hlines.h: Hidden-line elimination.
#ifndef HLINES_H
#define HLINES_H
#include "basic3d.h"
#include "triangulate.h"

const int N = 30; // There will be N x N screen rectangles:

class ScreenRect {
public:
   vector<int> trianrs;
};

class PointPair {
public:
   qreal xP, yP, xQ, yQ; // screen coordinates
};

class MyThread : public QThread {
   void run();
};

class HLines : public Basic3D {
   Q_OBJECT
protected:
   ScreenRect screenrect[N][N];
   int width0, height0;
   vector<PointPair> todraw;
   bool dashed = false, showNrs = false, appClosed = false,
      interrupted = false;
   void paintEvent(QPaintEvent *event) override;
   void mouseReleaseEvent(QMouseEvent *event)override;
   void closeEvent(QCloseEvent *event) override;
   QAction *exportAct, *dashAct, *showNrsAct, *setPenWidthAct,
      *interruptAct;
   QString fileNameSVG;
   QTextStream *pOutSVG = nullptr;
   void myDrawLine(QPainter &pnt, bool dashSVG,
      int x1, int y1, int x2, int y2)const;
   int penWidth = 2; // can be overwritten
public:
   HLines(QWidget *parent = nullptr);
   vector<Tria> tria; // triangles of all polygons
   vector<int>relatedtrianrs;
   vector<int> polnrs; // polnrs[i] is the number of the
                       // polygon to which tria[i] belongs.
   void linesegment(int iP, int iQ, int iTrStart);
   void collectTrias(int i, int j);
   void calculate();
   MyThread thread;
   QProgressBar progressBar;
   virtual void addToThreadStart();
   virtual void addToThreadFinish();
public slots:
   void drawImage();
   void open();
   void exportSVG();
   void dashLinesToggle();
   void showNrsToggle();
   void onFinished();
   void setPenWidth();
   void interruptProcess();
signals:
   void progress(int);
};
#endif // HLINES_H

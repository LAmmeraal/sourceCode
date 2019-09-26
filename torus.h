// torus.h: Generating a torus.
#ifndef TORUS_H
#define TORUS_H
#include "edit3d.h"

class Torus : public Edit3D {
   Q_OBJECT
public:
   Torus(QWidget *parent = nullptr);
protected:
   bool starting = true;
   int n; // Number of vertices on each circle
   qreal r, R;
   QAction *newTorusAct;
   void paintEvent(QPaintEvent *e)override;
   void defineObject();
   void addQuad(int A, int B, int C, int D);
protected slots:
   void newTorus();
};
#endif // TORUS_H

// icodode.h: Icosahedron and dodecahedron,
#ifndef ICODODE_H
#define ICODODE_H
#include "edit3d.h"

class IcoDode : public Edit3D {
   Q_OBJECT
public:
   IcoDode(int mode = 3, QWidget *parent = nullptr);
protected:
   qreal offsetY;
   int offsetNr;
   QAction *onlyIcosaAct, *onlyDodecaAct, *bothAct;
   QActionGroup *objects;
   void defineObject(int mode = 3);
   void triaFace(int A, int B, int C, int Centroid);
   void addPentaFace(int A, int B, int C, int D, int E);
protected slots:
   void refresh();
};
#endif // ICODODE_H

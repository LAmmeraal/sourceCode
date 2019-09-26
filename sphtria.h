// sphtria.h: Sphere approximated by triangles.
#ifndef SPHTRIA_H
#define SPHTRIA_H
#include "icodode.h"

class SphTria : public IcoDode {
   Q_OBJECT
public:
   SphTria(QWidget *parent = nullptr);
protected:
   bool starting = true;
   QAction *improveAct, *newIcosaAct;
   void defineObject();
   void scaleToUnitRadius();
   void addToThreadStart();
   void addToThreadFinish();
protected slots:
   void improve();
   void newIcosa();
};
#endif // SPHTRIA_H

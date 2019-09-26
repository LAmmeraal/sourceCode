// wireframe.h: Wireframe models.
#ifndef WIREFRAME_H
#define WIREFRAME_H
#include "basic3d.h"

class Wireframe : public Basic3D {
   Q_OBJECT
public:
   Wireframe(QWidget *parent = nullptr);
protected:
   void paintEvent(QPaintEvent *e) override;
};
#endif // WIREFRAME_H

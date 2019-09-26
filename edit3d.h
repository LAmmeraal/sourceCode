// edit3d.h: 3D editing.
#ifndef EDIT3D_H
#define EDIT3D_H
#include "hlines.h"

class Edit3D : public HLines {
   Q_OBJECT
public:
   Edit3D(QWidget *parent = nullptr);
   void drawImage0();
protected:
   void paintEvent(QPaintEvent *e);
   QAction *writeDataFileAct, *importAct, *selectAct,
      *selectAllAct, *deselectAllAct, *duplicateAct, *scaleAct,
      *shiftAct, *rotateAct, *addVertexAct, *addFaceAct,
      *delSelectedVerticesAct, *noVertexDataShownAct,
      *showNrsActExt, *showXAct, *showYAct, *showZAct;
   QActionGroup *annotation;
   QMenu *editMenu;
   int showCoords = 0;
   set<int> selection;
   void mousePressEvent(QMouseEvent *event)override;
   void removeAnyTrailingDummies();
   void addToThreadStart() override;
   void addToThreadFinish();
protected slots:
   void open();
   void import();
   void writeDataFile();
   void selectVertices();
   void selectAll();
   void deselectAll();
   void duplicateVertices();
   void shiftVertices();
   void scaleVertices();
   void rotateVertices();
   void addVertex();
   void addFace();
   void delSelectedVertices();
   void noVertexDataShown();
   void showXOn();
   void showYOn();
   void showZOn();
   void showNrsOn();
};
#endif // EDIT3D_H

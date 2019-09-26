// hlinescopy.h: Copying and shifting.
#ifndef HLINESCOPY_H
#define HLINESCOPY_H
#include "hlines.h"

class HLinesCopy : public HLines {
   Q_OBJECT
public:
   HLinesCopy(QWidget *parent = nullptr);
protected:
   QMenu *editMenu;
   QAction *selectAct;
   int first, last;
protected slots:
   void selectVertices();
};
#endif // HLINESCOPY_H

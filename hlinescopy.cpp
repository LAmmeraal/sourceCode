// hlinescopy.cpp: Copying and shifting.
#include "hlinescopy.h"

HLinesCopy::HLinesCopy(QWidget *parent) {
   setWindowTitle("Copy object or part of it");
   fileMenu->clear();
   fileMenu->addAction(openAct);
   fileMenu->addAction(exportAct);
   fileMenu->addAction(exitAct);
   editMenu = menuBar()->addMenu("&Edit");
   selectAct = new QAction("Select vertices", this);
   editMenu->addAction(selectAct);
   connect(selectAct, &QAction::triggered, this,
      &HLinesCopy::selectVertices);
   menuBar()->addMenu(viewMenu);
}

void HLinesCopy::selectVertices() {
   bool ok;
   first = QInputDialog::getInt(this, "Which vertices?",
      "Lowest vertex number      ",
      1, 0, world.size() - 1, 1, &ok);
   if (!ok)
      return;
   last = QInputDialog::getInt(this, "Which vertices?",
      "Highest vertex number      ",
      world.size() - 1, first, world.size() - 1, 1, &ok);
   if (!ok)
      return;
   qreal dx, dy, dz;
   dx = QInputDialog::getDouble(this, "Shift duplicate",
      "Delta x",
      0, -1e8, 1e8, 2, &ok);
   if (ok)
      dy = QInputDialog::getDouble(this, "Shift duplicate",
         "Delta y",
         0, -1e8, 1e8, 2, &ok);
   if (ok)
      dz = QInputDialog::getDouble(this, "Shift duplicate",
         "Delta z",
         0, -1e8, 1e8, 2, &ok);
   if (!ok)
      return;
   int firstempty = world.size(), firstafter = last + 1,
      dshift = firstempty - first,
      newsize = world.size() + firstafter - first;
   world.resize(newsize);
   QMessageBox msgBox;
   msgBox.setWindowTitle("Generated");
   msgBox.setText("Range of newly added vertex numbers " +
      QString::number(firstempty) + " - " +
      QString::number(newsize - 1));
   msgBox.exec();
   for (int i = first; i < firstafter; i++) {
      qreal x = world[i].x, y = world[i].y, z = world[i].z;
      world[i + dshift] = Point3D(x + dx, y + dy, z + dz);
   }
   int nPoly = polyList.size();
   for (int i = 0; i < nPoly; i++) {
      Polygon3D polygon = polyList[i], polygon1 = polygon;
      int j = -1;
      int n0 = polygon.nrs.size();
      for (j = 0; j < n0; j++) {
         int nr = polygon.nrs[j], nrabs = abs(nr);
         if (nrabs < first || nrabs > last)
            break;  // outside range: skip polygon
         polygon1.nrs[j] = nrabs + dshift;
         if (nr < 0)
            polygon1.nrs[j] *= -1;
      }
      if (j == n0)
         polyList.push_back(polygon1);
   }
   boundsCenterDistance();
   computePerspCoord();
   drawImage();
}

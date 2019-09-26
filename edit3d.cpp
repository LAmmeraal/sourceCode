// edit3d.cpp: 3D editing.
#include <sstream>
#include "edit3d.h"
#include "rota3d.h"

Edit3D::Edit3D(QWidget *parent) {
   setWindowTitle("Hidden lines 3D editor");
   importAct = new QAction("&Import object...", this);
   writeDataFileAct = new QAction("&Write data file...",
      this);
   fileMenu->clear();
   fileMenu->addAction(openAct);
   fileMenu->addAction(interruptAct);
   fileMenu->addAction(importAct);
   fileMenu->addAction(writeDataFileAct);
   fileMenu->addAction(exportAct);

   fileMenu->addAction(exitAct);

   selectAct = new QAction("Select vertices (&number range)...",
      this);
   selectAllAct = new QAction("Select all", this);
   selectAllAct->setShortcut(Qt::CTRL | Qt::Key_A);
   deselectAllAct = new QAction("Deselect &all", this);
   duplicateAct = new QAction("&Duplicate selected vertices",
      this);
   shiftAct = new QAction("&Shift selected vertices...", this);
   scaleAct = new QAction("S&cale selected vertices...", this);
   rotateAct = new QAction("&Rotate selected vertices...", this);
   addVertexAct = new QAction("Add &vertex...", this);
   addFaceAct = new QAction("Add &face (or line)...", this);
   delSelectedVerticesAct =
      new QAction("De&lete selected vertices", this);
   editMenu = menuBar()->addMenu("&Edit");
   editMenu->addAction(selectAct);
   editMenu->addAction(selectAllAct);
   editMenu->addAction(deselectAllAct);
   editMenu->addAction(duplicateAct);
   editMenu->addAction(shiftAct);
   editMenu->addAction(scaleAct);
   editMenu->addAction(rotateAct);
   editMenu->addAction(addVertexAct);
   editMenu->addAction(addFaceAct);
   editMenu->addAction(delSelectedVerticesAct);
   connect(openAct, &QAction::triggered, this,
      &Edit3D::open);
   connect(importAct, &QAction::triggered, this,
      &Edit3D::import);
   connect(writeDataFileAct, &QAction::triggered, this,
      &Edit3D::writeDataFile);
   connect(selectAct, &QAction::triggered, this,
      &Edit3D::selectVertices);
   connect(selectAllAct, &QAction::triggered, this,
      &Edit3D::selectAll);
   connect(deselectAllAct, &QAction::triggered, this,
      &Edit3D::deselectAll);
   connect(duplicateAct, &QAction::triggered, this,
      &Edit3D::duplicateVertices);
   connect(shiftAct, &QAction::triggered, this,
      &Edit3D::shiftVertices);
   connect(scaleAct, &QAction::triggered, this,
      &Edit3D::scaleVertices);
   connect(rotateAct, &QAction::triggered, this,
      &Edit3D::rotateVertices);
   connect(addVertexAct, &QAction::triggered, this,
      &Edit3D::addVertex);
   connect(addFaceAct, &QAction::triggered, this,
      &Edit3D::addFace);
   connect(delSelectedVerticesAct, &QAction::triggered, this,
      &Edit3D::delSelectedVertices);

   noVertexDataShownAct = new QAction("Show no vertex data");
   noVertexDataShownAct->setCheckable(true);
   noVertexDataShownAct->setChecked(true);
   showNrsActExt = new QAction("Show &vertex numbers",
      this);
   showNrsActExt->setCheckable(true);
   showXAct = new QAction("Show &x-coordinates", this);
   showXAct->setCheckable(true);
   showYAct = new QAction("Show &y-coordinates", this);
   showYAct->setCheckable(true);
   showZAct = new QAction("Show &z-coordinates", this);
   showZAct->setCheckable(true);

   annotation = new QActionGroup(this);
   annotation->addAction(noVertexDataShownAct);
   annotation->addAction(showNrsActExt);
   annotation->addAction(showXAct);
   annotation->addAction(showYAct);
   annotation->addAction(showZAct);
   viewMenu->removeAction(showNrsAct);
   viewMenu->addSeparator();
   viewMenu->addAction(noVertexDataShownAct);
   viewMenu->addAction(showNrsActExt);
   viewMenu->addAction(showXAct);
   viewMenu->addAction(showYAct);
   viewMenu->addAction(showZAct);
   viewMenu->addSeparator();

   connect(noVertexDataShownAct, &QAction::triggered, this,
      &Edit3D::noVertexDataShown);
   connect(showNrsActExt, &QAction::triggered, this,
      &Edit3D::showNrsOn);
   connect(showNrsActExt, &QAction::triggered, this,
      &Edit3D::showNrsOn);
   connect(showXAct, &QAction::triggered, this,
      &Edit3D::showXOn);
   connect(showYAct, &QAction::triggered, this,
      &Edit3D::showYOn);
   connect(showZAct, &QAction::triggered, this,
      &Edit3D::showZOn);

   menuBar()->addMenu(viewMenu);
}

void Edit3D::writeDataFile() {
   QString fName = QFileDialog::getSaveFileName(this,
      "3D Data file", fileName);
   QFile qf(fName);
   qf.open(QFile::WriteOnly | QFile::Text);
   QTextStream *pOut;
   pOut = new QTextStream(&qf);
   for (int i = 0; i < world.size(); i++) {
      qreal x = world[i].x, y = world[i].y, z = world[i].z;
      if (x < 1e29)
         *pOut << fixed << i << "   " << world[i].x << " "
         << world[i].y << " " << world[i].z << endl;
   }
   *pOut << "Faces:" << endl;
   for (int j = 0; j < polyList.size(); j++) {
      vector<int> nrs = polyList[j].nrs;
      for (int k = 0; k < nrs.size(); k++)
         *pOut << nrs[k] << (k < nrs.size() - 1 ? " " : ".\n");
   }
   qf.close();
}

void Edit3D::selectVertices() {
   bool ok;
   int first = QInputDialog::getInt(this, "Which vertices?",
      "Lowest vertex number      ",
      1, 0, world.size() - 1, 1, &ok);
   if (!ok)
      return;
   int last = QInputDialog::getInt(this, "Which vertices?",
      "Highest vertex number      ",
      world.size() - 1, first, world.size() - 1, 1, &ok);
   if (!ok)
      return;
   QMessageBox msgBox;
   msgBox.setWindowTitle("Selected");
   msgBox.setText("Range of selected vertices: " +
      QString::number(first) + " - " +
      QString::number(last));
   msgBox.exec();
   for (int i = first; i <= last; i++) {
      selection.insert(i);
   }
}

void Edit3D::duplicateVertices() {
   QMessageBox msgBox;
   map<int, int> dupl;

   if (selection.empty())
      return;
   int firstnew = world.size();
   for (set<int>::iterator i = selection.begin();
      i != selection.end(); i++) {
      dupl[*i] = world.size(); // pair (old, new)
      world.push_back(world[*i]);
   }
   int lastnew = world.size() - 1;

   // Copy any face whose vertices are all selected,
   // using vertex number dupl[nr] instead of nr:
   int nPoly = polyList.size();
   for (int i = 0; i < nPoly; i++) {
      Polygon3D polygon = polyList[i], polygon1 = polygon;
      int j = -1;
      int n0 = polygon.nrs.size();
      for (j = 0; j < n0; j++) {
         int nr = polygon.nrs[j], nrabs = abs(nr);
         set<int>::iterator i = selection.find(nrabs);
         if (i == selection.end())
            break;
         polygon1.nrs[j] = dupl[nrabs];
         if (nr < 0)
            polygon1.nrs[j] *= -1;
      }
      if (j == n0)
         polyList.push_back(polygon1);
   }

   // Replace old selection with new one:
   selection.clear();
   for (int i = firstnew; i <= lastnew; i++)
      selection.insert(i);
   boundsCenterDistance();
   computePerspCoord();
   msgBox.setWindowTitle("Duplicated points, now selected");
   msgBox.setText("New range of selected vertex numbers " +
      QString::number(firstnew) + " - " +
      QString::number(lastnew) +
      "\nYou can now shift the new points by supplying\n" +
      "the shift vector (delta x, delta y, delta z).\n" +
      "Use 0 0 0 if you don\'t want to shift them now.");
   msgBox.exec();
   shiftVertices();
   drawImage0();
}

void Edit3D::shiftVertices() {
   bool ok;
   QString text = QInputDialog::getText(this,
      "Shift vector",
      "Enter delta x, delta y, delta z, for example, 5 0 0     ",
      QLineEdit::Normal, "0  0  0",
      &ok);
   std::string str = text.toStdString();
   istringstream ii(str);
   qreal dx, dy, dz;
   ii >> dx >> dy >> dz;
   if (ii.fail())
      return;
   for (set<int>::iterator i = selection.begin();
      i != selection.end(); i++) {
      int j = *i;
      world[j] = Point3D(world[j].x + dx,
         world[j].y + dy, world[j].z + dz);
   }
   boundsCenterDistance();
   computePerspCoord();
   drawImage0();
}

void Edit3D::scaleVertices() {
   bool ok;
   qreal f = QInputDialog::getDouble(this, "Scale selected",
      "Scale factor: ",
      2, -10000, 10000, 2, &ok);
   if (ok) {
      for (set<int>::iterator i = selection.begin();
         i != selection.end(); i++) {
         int j = *i;
         world[j].x *= f;
         world[j].y *= f;
         world[j].z *= f;
      }
      boundsCenterDistance();
      computePerspCoord();
      drawImage0();
   }
}

void Edit3D::rotateVertices() {
   qreal alpha;
   bool ok;
   int nrA, nrB, maxNr = world.size() - 1;
   nrA = QInputDialog::getInt(this, "Rotation",
      "Rotation about axis AB, like right-handed screw.\n \
       Vertex number of point A: ",
      1, 0, maxNr, 1, &ok);
   if (ok)
      nrB = QInputDialog::getInt(this, "Rotation vector AB",
         "Vertex number of point B: ",
         1, 0, maxNr, 1, &ok);
   if (!ok || nrA == nrB || world[nrA].x > 1e29 ||
      world[nrB].x > 1e29)
      return;
   alpha = QInputDialog::getDouble(this, "Rotation",
      "Rotation angle (deg): ",
      90, -360, 360, 2, &ok);
   if (ok) {
      alpha *= M_PI / 180; // Radians
      Rota3D rotation(world[nrA], world[nrB], alpha);
      for (set<int>::iterator i = selection.begin();
         i != selection.end(); i++)
         rotation.rotate(world[*i], world[*i]);
   }
   boundsCenterDistance();
   computePerspCoord();
   drawImage0();
}

void Edit3D::addVertex() {
   bool ok;
   QString text = QInputDialog::getText(this, "Enter x, y, z",
      "for example, 12.3  45  6.7            ",
      QLineEdit::Normal, " ", &ok);
   if (world.size() == 0)
      world.push_back(Point3D(1e30, 0, 0)); // dummy world[0]
   std::string str = text.toStdString();
   istringstream ii(str);
   double x, y, z;
   ii >> x >> y >> z;
   if (ii.fail())
      return;
   int nr = world.size();
   world.push_back(Point3D(x, y, z));
   boundsCenterDistance();
   computePerspCoord();
   selection.insert(nr);
   QMessageBox msgBox;
   msgBox.setWindowTitle("New point");
   msgBox.setText("Vertex " + QString::number(nr) +
      ": (" + QString::number(x) + ", " +
      QString::number(y) + ", " +
      QString::number(z) + ")");
   msgBox.exec();
   boundsCenterDistance();
   computePerspCoord();
   drawImage0();
}

void Edit3D::addFace() {
   bool ok;
   QString text = QInputDialog::getText(this,
      "Enter vertex numbers of face or line",
      "for example, 3 4 5 18 -19 7 8 19 -18",
      QLineEdit::Normal, " ", &ok);
   if (!ok)
      return;
   std::string str = text.toStdString();
   istringstream ii(str);
   int nr;

   Polygon3D polygon;
   // Loop through the polygon:
   for (;;) {
      ii >> nr;
      if (ii.fail())
         break;
      if (abs(nr) >= world.size() || world[abs(nr)].x > 1e29) {
         QMessageBox msgBox;
         msgBox.setWindowTitle("Error");
         msgBox.setText(QString::number(nr) + " undefined.");
         msgBox.exec();
         break;
      }
      polygon.nrs.push_back(nr);
   }
   if (polygon.nrs.size() >= 2) {
      polyList.push_back(polygon);
      drawImage0();
   }
}

void Edit3D::delSelectedVertices() {
   bool warningRequired = true;
   for (set<int>::iterator it = selection.begin();
      it != selection.end(); it++) {
      int nr = *it;
      // Look for polygons of which nr is a face:
      for (int j = 0; j < polyList.size(); j++) {
         vector<int> nrs = polyList[j].nrs;
         for (int i = 0; i < nrs.size(); i++) {
            if (nrs[i] == nr) {
               if (warningRequired) {
                  if (QMessageBox(QMessageBox::Information,
                     "Warning",
                     "Some face(s) will also be deleted; proceed?",
                     QMessageBox::Yes | QMessageBox::No).exec() ==
                     QMessageBox::No)
                     return;
                  warningRequired = false;
               }
               polyList.erase(polyList.begin() + j);
               j--; // The next face will again be polyList[j]
            }
         }
      }
      // Delete the points themselves:
      int i = *it;
      world[i].x = 1e30;
   }
   selection.clear();
   removeAnyTrailingDummies();
   boundsCenterDistance();
   computePerspCoord();
   drawImage0();
}

void Edit3D::removeAnyTrailingDummies() {
   int n = world.size(), n0 = n;
   for (int i = n - 1; i > 0; i--)
      if (world[i].x > 1e29)
         n--;
      else
         break;
   if (n < n0) {
      world.resize(n);
   }
}

void Edit3D::open() {
   selection.clear();
}

void Edit3D::import() {
   QString fileName0 = fileName;
   fileName = QFileDialog::getOpenFileName(this,
      "Import File", fileName, "3D data files (*.dat)");
   vector<Point3D> world0;
   vector<Polygon3D> polyList0;
   int iEnd = world.size();
   world0.resize(iEnd);
   polyList0.resize(polyList.size());
   copy(world.begin(), world.end(), world0.begin());
   copy(polyList.begin(), polyList.end(), polyList0.begin());
   inputObject();
   if (world.size() == 0)
      return;
   int i1Start = 0, i1End = world.size();
   for (int i = 0; i < i1End; i++) {
      if (world[i].x < 1e29)
         break;
      i1Start++;
   }
   // New elements located in world[i1Start],...,world[i1End-1]
   int n0 = world0.size(), n1 = i1End - i1Start;
   world.resize(n0 + n1);
   int d = iEnd - i1Start;
   // Shift the n1 new elements d positions to the right, to create
   // space for the n0 old ones at the beginning; start at the back:
   copy_backward(world.begin() + i1Start, world.begin() + i1End,
      world.end());
   // Copy the old elements, from world0 to world:
   copy(world0.begin(), world0.end(), world.begin());


   // Increase the new vertex numbers by d:
   int nPoly0 = polyList0.size(), nPoly1 = polyList.size();
   for (int i = 0; i < nPoly1; i++) {
      for (int j = 0; j < polyList[i].nrs.size(); j++) {
         int nr = polyList[i].nrs[j];
         polyList[i].nrs[j] = (nr < 0 ? nr - d : nr + d);
      }
   }
   // Do shift operations similar to those for the vertices:
   polyList.resize(nPoly0 + nPoly1);
   copy_backward(polyList.begin(), polyList.begin() + nPoly1,
      polyList.begin() + nPoly0 + nPoly1);
   copy(polyList0.begin(), polyList0.end(), polyList.begin());
   // Order: (n0 old elements, n1 new elements)
   selection.clear();
   for (int i = iEnd; i < n0 + n1; i++)
      if (world[i].x < 1e29)
         selection.insert(i);
   boundsCenterDistance();
   computePerspCoord();
   drawImage0();
}

void Edit3D::selectAll() {
   for (int i = 0; i < world.size(); i++)
      if (world[i].x < 1e29)
         selection.insert(i);
   repaint();
}

void Edit3D::deselectAll() {
   selection.clear();
   repaint();
}

void Edit3D::noVertexDataShown() {
   showNrs = false;
   showCoords = 0;
   repaint();
}

void Edit3D::showNrsOn() {
   showNrs = true;
   showCoords = 0;
   repaint();
}

void Edit3D::showXOn() {
   showCoords = 1;
   showNrs = false;
   repaint();
}
void Edit3D::showYOn() {
   showCoords = 2;
   showNrs = false;
   repaint();
}
void Edit3D::showZOn() {
   showCoords = 3;
   showNrs = false;
   repaint();
}

void Edit3D::drawImage0() {
   interruptAct->setVisible(true);
   HLines::drawImage();
}

void Edit3D::paintEvent(QPaintEvent *e) {
   if (thread.isRunning())
      return;
   HLines::paintEvent(e);

   QPainter pnt(this);
   for (int i = 0; i < scr.size(); i++) {
      if (world[i].x > 1e29)
         continue;
      qreal x = scr[i].x, y = scr[i].y;
      int x1 = xDev(x), y1 = yDev(y);

      if (showCoords != 0) {
         qreal coord = (showCoords == 1 ? world[i].x :
            showCoords == 2 ? world[i].y :
            world[i].z);
         pnt.drawText(x1, y1, QString::number(coord));
      }
      set<int>::iterator it = selection.find(i);
      if (it != selection.end()) { // mark selected vertex:
         pnt.setPen(QPen(Qt::red, 3));
         pnt.setBrush(Qt::red);
         pnt.drawEllipse(x1 - 1, y1 - 1, 3, 3);
         pnt.setPen(QPen(Qt::black, 2));
         pnt.setBrush(Qt::black);
      }
   }
   if (showNrs)
      pnt.drawText(10, 50, "Vertex numbers shown");
   if (showCoords) {
      QString s = (showCoords == 1 ? "x" : showCoords == 2 ?
         "y" : "z");
      pnt.drawText(10, 50, s + "-coordinates shown");
   }
}

void Edit3D::mousePressEvent(QMouseEvent *event) {
   if (thread.isRunning())
      return;
   Basic3D::mousePressEvent(event);
   int x, y;
   if (event->button() == Qt::LeftButton) {
      x = event->pos().x();
      y = event->pos().y();
   }
   for (int i = 0; i < scr.size(); i++) {
      qreal x1 = scr[i].x, y1 = scr[i].y,
         dx = abs(x - xDev(x1)), dy = abs(y - yDev(y1));
      if (dx < 5 && dy < 5) {
         if (selection.find(i) == selection.end())
            selection.insert(i);
         else
            selection.erase(i);
         repaint();
      }
   }
}

void Edit3D::addToThreadStart() { // overrides HLines function
   editMenu->setEnabled(false);
   importAct->setVisible(false);
   exportAct->setEnabled(false);
   writeDataFileAct->setEnabled(false);
}

void Edit3D::addToThreadFinish() { // overrides HLines function
   editMenu->setEnabled(true);
   importAct->setVisible(true);
   exportAct->setEnabled(true);
   writeDataFileAct->setEnabled(true);
}

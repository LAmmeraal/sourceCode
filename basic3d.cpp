// basic3d.cpp: Base class for 3D classes
#include "basic3d.h"

Basic3D::Basic3D(QWidget *parent) {
   QRect rec = QApplication::desktop()->screenGeometry();
   int scrW = rec.width(), scrH = rec.height(), // Device dimensions
      wD = scrW / 2, hD = 2 * wD / 3,
      wMargin = (scrW - wD) / 2,
      hMargin = (scrH - hD) / 2;
   setGeometry(wMargin, hMargin, wD, hD);
   fileMenu = menuBar()->addMenu("&File");
   viewMenu = menuBar()->addMenu("&View");

   openAct = new QAction("Open...", this);
   openAct->setShortcut(Qt::CTRL | Qt::Key_O);
   connect(openAct, &QAction::triggered, this, &Basic3D::open);

   exitAct = new QAction("Exit", this);
   exitAct->setShortcut(Qt::CTRL | Qt::Key_Q);
   connect(exitAct, &QAction::triggered, this, &QWidget::close);

   fileMenu->addAction(openAct);
   fileMenu->addAction(exitAct);

   incrDistAct = new QAction("Increase viewing distance", this);
   incrDistAct->setShortcut(Qt::Key_I);
   connect(incrDistAct, &QAction::triggered, this,
      &Basic3D::incrDist);

   decrDistAct = new QAction("Decrease viewing distance", this);
   decrDistAct->setShortcut(Qt::Key_D);
   connect(decrDistAct, &QAction::triggered, this,
      &Basic3D::decrDist);

   angleChangeAct = new QAction("&Change viewpoint...", this);
   connect(angleChangeAct, &QAction::triggered, this,
      &Basic3D::angleChange);

   resetAct = new QAction("&Restore default viewpoint", this);
   connect(resetAct, &QAction::triggered, this,
      &Basic3D::resetViewpoint);

   viewMenu->addAction(incrDistAct);
   viewMenu->addAction(decrDistAct);
   viewMenu->addAction(angleChangeAct);
   viewMenu->addAction(resetAct);
   menuBar()->addMenu(viewMenu);
}

void Basic3D::simpleMessage(const QString &title,
   const QString &mes) {
   QMessageBox msgBox;
   msgBox.setWindowTitle(title);
   msgBox.setText(mes);
   msgBox.exec();
}

void Basic3D::boundsCenterDistance() {
   xwMin = 1e6, xwMax = -xwMin,
      ywMin = xwMin, ywMax = xwMax,
      zwMin = xwMin, zwMax = xwMax;
   for (int i = 0; i < world.size(); i++) {
      qreal x = world[i].x, y = world[i].y, z = world[i].z;
      if (x > 1e29) // position not in use
         continue;
      xwMin = min(x, xwMin); xwMax = max(x, xwMax);
      ywMin = min(y, ywMin); ywMax = max(y, ywMax);
      zwMin = min(z, zwMin); zwMax = max(z, zwMax);
   }
   xC = (xwMin + xwMax) / 2;
   yC = (ywMin + ywMax) / 2;
   zC = (zwMin + zwMax) / 2;
   qreal xwRange = xwMax - xwMin + 0.001,
      ywRange = ywMax - ywMin + 0.001,
      zwRange = zwMax - zwMin + 0.001;
   rhoMin = sqrt(xwRange * xwRange +
      ywRange * ywRange + zwRange * zwRange);
   rhoMax = 40 * rhoMin;
   rho0 = rho = 3 * rhoMin;
}

bool Basic3D::different(int i, int j) const {
   return world[i].x != world[j].x || world[i].y != world[j].y ||
      world[i].z != world[j].z;
}

void Basic3D::inputObject() {
   QFile qf(fileName);
   qf.open(QFile::ReadOnly | QFile::Text);
   QTextStream in(&qf);
   bool ok;
   QString stri, strx, stry, strz;
   world.clear();
   edges.clear();
   polyList.clear();

   // Find highest vertex number:
   int maxnr = 0;
   while (true) {
      int i;
      in >> stri;
      i = stri.toInt(&ok);
      if (!ok || in.atEnd())
         break;
      maxnr = max(i, maxnr);
      in >> strx >> stry >> strz;
   }
   world.resize(maxnr + 1);
   for (int i = 0; i <= maxnr; i++)
      world[i].x = 1e30; // To detect unused positions later
   in.seek(0);           // Rewind
   for (;;) {
      int i;
      qreal x, y, z;
      in >> stri;
      i = stri.toInt(&ok);
      if (!ok || in.atEnd())
         break;
      in >> strx >> stry >> strz;
      x = strx.toDouble(&ok);
      y = stry.toDouble(&ok);
      z = strz.toDouble(&ok);
      world[i] = Point3D(x, y, z);
   }
   boundsCenterDistance();
   if (stri.length() > 0 && stri.at(0) == 'F') {
      // Read polygons and use its edges i-j:
      QString stri;
      int j, len;
      bool errorfound = false;
      // Loop through the polygons:
      while (!errorfound) {  // Outer loop
         Polygon3D polygon;
         // Loop through the vertices of a polygon:
         while (!errorfound) {  // Inner loop
            in >> stri;
            len = stri.length();
            if (len > 0) {
               bool finalvertex =
                  (len > 1 && stri.at(len - 1) == '.');
               if (finalvertex)
                  len--;
               j = stri.left(len).toInt(&ok);
               if (ok) {
                  int nr = abs(j);
                  if (nr >= world.size() || world[nr].x > 1e29) {
                     simpleMessage("Error",
                        "Undefined vertex: " + QString::number(nr));
                     errorfound = true;
                     break;
                  }
                  int size = polygon.nrs.size();
                  if (size == 0 ||
                     (different(nr, abs(polygon.nrs[size - 1])) &&
                     (!finalvertex ||
                        different(nr, abs(polygon.nrs[0])))))
                     polygon.nrs.push_back(j);
                  if (!finalvertex)
                     continue;
               }
               else
                  break;
            }
            break;
         }
         if (polygon.nrs.size() >= 2) { // 2 for loose lines
            polyList.push_back(polygon);
         }
         if (in.atEnd() || !ok)
            break;
      }
   }
   qf.close();
}

void Basic3D::open() {
   fileName = QFileDialog::getOpenFileName(this,
      "Open File", fileName, "3D data files (*.dat)");
   fileDialogDone = true;
}

void Basic3D::newObjectTest() {
   if (fileDialogDone && fileName.length() > 0) {
      inputObject();
      computePerspCoord();
   }
   fileDialogDone = false;
}

void Basic3D::incrDist() {
   rho = min(rhoMax, rho * 1.5);
   computePerspCoord();
   if (defaultPaint)
      repaint();
}

void Basic3D::decrDist() {
   rho = max(rhoMin, rho / 1.5);
   computePerspCoord();
   if (defaultPaint)
      repaint();
}

void Basic3D::resetViewpoint() {
   rho = rho0;
   thetaDeg = theta0Deg; theta = theta0;
   phiDeg = phi0Deg; phi = phi0;
   computePerspCoord();
   if (defaultPaint)
      repaint();
}

void Basic3D::angleChange() {
   bool ok;
   qreal thetaDeg1, phiDeg1, rho1;
   thetaDeg1 = QInputDialog::getDouble(this, "theta (deg)",
      "Angle in hor. plane", thetaDeg, -180, 180, 1, &ok);
   if (ok)
      phiDeg1 = QInputDialog::getDouble(this, "phi (deg)",
         "Angle in vert. plane", phiDeg, 0, 180, 1, &ok);
   if (ok)
      rho1 = QInputDialog::getDouble(this, "rho",
         "Viewing distance", rho, 1, 1e9, 1, &ok);
   if (ok) {
      thetaDeg = thetaDeg1;
      phiDeg = phiDeg1;
      rho = rho1;
      if (rho < rhoMin) {
         rho = rhoMin;
      }
      if (rho > rhoMax) {
         rho = rhoMax;
      }
      if (rho != rho1)
         simpleMessage("Adapted to range",
            "Viewing distance set to " +
            QString::number(rho, 'f', 1));
      theta = thetaDeg * M_PI / 180;
      phi = phiDeg * M_PI / 180;
      computePerspCoord();
      if (defaultPaint)
         repaint();
   }
}

void Basic3D::mousePressEvent(QMouseEvent *event) {
   if (event->button() == Qt::LeftButton) {
      xPress = (event->pos()).x();
      yPress = (event->pos()).y();
   }
}

void Basic3D::computePerspCoord() {
   int n = world.size();
   xsMin = 1e6, xsMax = -xsMin, ysMin = xsMin, ysMax = xsMax;
   qreal costh = cos(theta), sinth = sin(theta),
      cosph = cos(phi), sinph = sin(phi),
      v11 = -sinth, v12 = -cosph * costh, v13 = sinph * costh,
      v21 = costh, v22 = -cosph * sinth, v23 = sinph * sinth,
      v32 = sinph, v33 = cosph,
      v43 = -rho;
   eye.resize(n);
   scr.resize(n);
   zNear = -1e20;
   zFar = 1e20;
   for (int i = 0; i < n; i++) {
      if (world[i].x > 1e29)
         continue;  // unused position
      // Eye coordinates:
      Point3D p = world[i], pe;
      qreal x = p.x - xC, y = p.y - yC, z = p.z - zC;
      pe.x = v11 * x + v21 * y;
      pe.y = v12 * x + v22 * y + v32 * z;
      pe.z = v13 * x + v23 * y + v33 * z + v43;
      zNear = max(zNear, pe.z);
      zFar = min(zFar, pe.z);
      eye[i] = pe;
      // Screen coordinates:
      qreal X = -pe.x / pe.z, Y = -pe.y / pe.z;
      xsMin = min(X, xsMin); ysMin = min(Y, ysMin);
      xsMax = max(X, xsMax); ysMax = max(Y, ysMax);
      scr[i] = Point2D(X, Y);
   }
   eps1 = 0.001 * (zNear - zFar);
   if (n == 0)
      return;
   xsRange = xsMax - xsMin + 1e-9; // + 1e-9 prevents
   ysRange = ysMax - ysMin + 1e-9; // division by zero.
   d = 0.85 * min(wDraw / xsRange, hDraw / ysRange);
   xsCenter = 0.5 * (xsMin + xsMax);
   ysCenter = 0.5 * (ysMin + ysMax);
}

void Basic3D::mouseReleaseEvent(QMouseEvent *event) {
   if (event->button() == Qt::LeftButton) {
      QPoint release = event->pos();
      int dx = release.x() - xPress, dy = release.y() - yPress;
      if (abs(dx) + abs(dy) < 5) // Not worthwhile
         return;
      if (abs(dx) > abs(dy)) {
         thetaDeg -= 0.001 * dx * width();
         theta = thetaDeg * M_PI / 180;
      }
      else {
         phiDeg -= 0.001 * dy * height();
         if (phiDeg > 180)
            phiDeg = 180;
         if (phiDeg < 0)
            phiDeg = 0;
         phi = phiDeg * M_PI / 180;
      }
      computePerspCoord();
      if (defaultPaint)
         repaint();
   }
}

void Basic3D::resizeEvent(QResizeEvent *e) {
   wDraw = width();
   hDraw = height() - hMenu;
   xDevCenter = wDraw / 2;
   yDevCenter = hMenu + hDraw / 2;
   if (xsRange != 0)
      d = 0.85 * min(wDraw / xsRange, hDraw / ysRange);
   repaint();
}

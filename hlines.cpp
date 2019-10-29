// hlines.cpp: Hidden-line elimination.
#include "hlines.h"

HLines::HLines(QWidget *parent) {
   thread.setParent(this);
   defaultPaint = false;  // to prevent repaint in Basic3D
   setWindowTitle("Hidden-lines model");
   fileMenu->clear();
   interruptAct = new QAction("Interrupt and clear", this);

   connect(interruptAct, &QAction::triggered, this,
      &HLines::interruptProcess);
   connect(openAct, &QAction::triggered, this, &HLines::open);
   exportAct = new QAction("&Export SVG...", this);
   connect(exportAct, &QAction::triggered, this,
      &HLines::exportSVG);
   // The Basic3D constructor has already been executed
   // and has connected incrDistAct,...,resetAct to 
   // Basic3D slot member functions.
   connect(incrDistAct, &QAction::triggered, this,
      &HLines::drawImage);
   connect(decrDistAct, &QAction::triggered, this,
      &HLines::drawImage);
   connect(angleChangeAct, &QAction::triggered, this,
      &HLines::drawImage);
   connect(resetAct, &QAction::triggered, this,
      &HLines::drawImage);
   dashAct = new QAction("Dashed &hidden lines On/Off", this);
   dashAct->setCheckable(true);
   dashAct->setChecked(false);
   connect(dashAct, &QAction::triggered, this,
      &HLines::dashLinesToggle);
   showNrsAct = new QAction("Show &vertex numbers On/Off", this);
   showNrsAct->setCheckable(true);
   showNrsAct->setChecked(false);
   connect(showNrsAct, &QAction::triggered, this,
      &HLines::showNrsToggle);
   setPenWidthAct = new QAction("Set pen width...", this);
   connect(setPenWidthAct, &QAction::triggered, this,
      &HLines::setPenWidth);
   fileMenu->addAction(openAct);
   fileMenu->addAction(interruptAct);
   interruptAct->setVisible(false);
   fileMenu->addAction(exportAct);
   fileMenu->addAction(exitAct);
   viewMenu->addAction(dashAct);
   viewMenu->addAction(showNrsAct);
   viewMenu->addAction(setPenWidthAct);
   progressBar.setRange(0, 100);
}

void HLines::open() {
   if (thread.isRunning()) {
      thread.quit();
   }
   inputObject();
   drawImage();
}

void HLines::interruptProcess() {
   thread.requestInterruption();
   interrupted = true;
}

void HLines::setPenWidth() {
   bool ok;
   int thickness = QInputDialog::getInt(this, "Pen width",
      "Set pen width for solid lines: ",
      penWidth, 0, 100, 1, &ok);
   if (ok)
      penWidth = thickness;
}

void HLines::drawImage() {
   addToThreadStart();
   computePerspCoord();
   todraw.clear();
   tria.clear();
   polnrs.clear();
   edges.clear();
   if (world.size() == 0)
      return;
   connect(&thread, SIGNAL(finished()),
      this, SLOT(onFinished()));
   connect(this, SIGNAL(progress(int)),
      &progressBar, SLOT(setValue(int)));
   progressBar.setParent(this);
   progressBar.show();
   progressBar.setGeometry(0, height() - 10, width(), 10);
   openAct->setEnabled(false);
   interruptAct->setVisible(true);
   viewMenu->setEnabled(false);
   interrupted = false;
   thread.start();
}

void HLines::calculate() {  // called in thread
   int nPolyList = polyList.size();
   for (int i = 0; i < nPolyList; i++) {
      emit progress(i * 30 / nPolyList); // 0-30%
      if (thread.isInterruptionRequested())
         return;
      Polygon3D polygon = polyList[i];
      if (polygon.nrs.size() == 2) { // Loose line segment
         edges.insert(IntPair(abs(polygon.nrs[0]),
            abs(polygon.nrs[1])));
         continue;
      }
      bool clockwise =
         !Triangulate::isCounterclockwise(scr, polygon.nrs);
      if (!dashed && clockwise) // Unless dashed lines are
         continue;     // desired, we are done with this polygon
      // For each polygon edge to be drawn, put the vertex
      // number pair of its endpoints in the set 'edges'.
      // We also compute a, b, c and h of the equation
      // ax + by + cz = h for the plane of each polygon:
      int n = polygon.nrs.size();
      qreal h0 = 1e30, a0, b0, c0, len0 = 0;
      for (int k = 0; k < n; k++) {
         int from = abs(polygon.nrs[k]),
            to = polygon.nrs[(k + 1) % n];
         if (to >= 0) {
            if (!clockwise)
               edges.insert(IntPair(from, to));
         }
         if (k < 2)
            continue;
         // Now we compute a, b, c and h of the equation
         // ax + by + cz = h for the plane of each polygon:
         int iA = abs(polygon.nrs[k - 2]),
            iB = abs(polygon.nrs[k - 1]),
            iC = abs(polygon.nrs[k]);
         Point3D A = eye[iA], B = eye[iB], C = eye[iC];
         qreal u1 = B.x - A.x, u2 = B.y - A.y, u3 = B.z - A.z,
            v1 = C.x - A.x, v2 = C.y - A.y, v3 = C.z - A.z,
            a = u2 * v3 - u3 * v2, b = u3 * v1 - u1 * v3,
            c = u1 * v2 - u2 * v1,
            len = sqrt(a * a + b * b + c * c), h;
         a /= len;
         b /= len;
         c /= len;
         h = a * A.x + b * A.y + c * A.z;
         if (len > len0 && h < 0) {
            a0 = a; b0 = b; c0 = c; h0 = h, len0 = len;
         }
      }
      polygon.a = a0; polygon.b = b0; polygon.c = c0;
      polygon.h = h0;
      polyList[i] = polygon;
      if (clockwise) // Even if dashed lines are desired we are
         continue;   // now done with this polygon.

      // Triangulation:
      vector<Tria> triaExtra;
      Triangulate::triangulate(scr, polygon.nrs, triaExtra);
      for (int j = 0; j < triaExtra.size(); j++) {
         tria.push_back(triaExtra[j]);
         polnrs.push_back(i);
      }
      if (thread.isInterruptionRequested())
         return;
   }
   // Build N x N screen rectangles, each containing a set of
   // related triangles:
   qreal rectWidth = xsRange / N, rectHeight = ysRange / N;
   for (int i = 0; i < N; i++) { // i = row number, 0 at bottom
      emit progress(30 + i * 30 / N);  // 30-60%
      qreal lowerb = ysMin + i * rectHeight,
         upperb = lowerb + rectHeight;
      for (int j = 0; j < N; j++) { // j = column number
         if (thread.isInterruptionRequested())
            return;
         screenrect[i][j].trianrs.clear();
         qreal leftb = xsMin + j * rectWidth,
            rightb = leftb + rectWidth;
         // Build the set of tria subscripts for all triangles
         // related to screen rectangle at row i and column j:
         for (int k = 0; k < tria.size(); k++) {
            Tria t = tria[k];
            int iA = t.A, iB = t.B, iC = t.C;
            qreal xMin = min(scr[iA].x, min(scr[iB].x, scr[iC].x));
            if (xMin > rightb)
               continue;
            qreal xMax = max(scr[iA].x, max(scr[iB].x, scr[iC].x));
            if (xMax < leftb)
               continue;
            qreal yMin = min(scr[iA].y, min(scr[iB].y, scr[iC].y));
            if (yMin > upperb)
               continue;
            qreal yMax = max(scr[iA].y, max(scr[iB].y, scr[iC].y));
            if (yMax < lowerb)
               continue;
            screenrect[i][j].trianrs.push_back(k);
         }
      }
   }
   // Now that we have a complete list of triangles,
   // we will be dealing with all polygon edges that are not
   // completely invisible:
   int nEdges = edges.size(), iEdges = 0;
   for (EdgeSet::iterator k = edges.begin();
      k != edges.end(); k++) {
      emit progress(60 + (iEdges++) * 40 / nEdges);  // 60-100%
      if (thread.isInterruptionRequested())
         return;
      int from = (*k).i, to = (*k).j;
      Point2D P = scr[from], Q = scr[to];
      // Use the screen rectangles to determine which triangles
      // are related to line segment PQ; store these triangles
      // in the vector triaSelected:
      if (P.x > Q.x) {
         Point2D R = P; P = Q; Q = R;
         // Now P.x <= Q.x
      }
      int iP = (P.y - ysMin) / rectHeight,
         jP = (P.x - xsMin) / rectWidth,
         iQ = (Q.y - ysMin) / rectHeight,
         jQ = (Q.x - xsMin) / rectWidth;
      relatedtrianrs.clear();
      int mini = min(iP, iQ), maxi = iP + iQ - mini;
      for (int j = jP; j <= jQ; j++)
         for (int i = mini; i <= maxi; i++)
            collectTrias(i, j);
      linesegment(from, to, 0);
   }
   if (edges.size() > 0 && pOutSVG != 0) {
      *pOutSVG << "</svg>" << endl;
      pOutSVG = 0;
   }
}

void HLines::onFinished() {
   interruptAct->setVisible(false);
   openAct->setEnabled(true);
   viewMenu->setEnabled(true);
   progressBar.hide();
   addToThreadFinish();
   if (interrupted) {
      world.clear();
      polyList.clear();
      drawImage();
   }
   if (!appClosed)
      repaint();
}

void HLines::exportSVG() {
   QString fileNameSVG0 = fileName;
   if (fileNameSVG0.endsWith(".dat", Qt::CaseInsensitive))
      fileNameSVG0.remove(fileNameSVG0.length() - 4, 4);
   fileNameSVG0.append(".svg");
   fileNameSVG = QFileDialog::getSaveFileName(this,
      "Export SVG", fileNameSVG0);
   QFile qf(fileNameSVG);
   qf.open(QFile::WriteOnly | QFile::Text);
   pOutSVG = new QTextStream(&qf);
   *pOutSVG << "<svg width=\"" << width() << "\" height=\""
      << height() << "\" viewBox=\"0 0 " << width() << " "
      << height() << "\"" << endl
      << "xmlns=\"http://www.w3.org/2000/svg\"" << endl
      << "       xmlns:xlink=\"http://www.w3.org/1999/xlink\">"
      << endl;
   repaint();
   if (fileNameSVG.length() > 0)
      simpleMessage("SVG file written", "SVG output in " + 
         fileNameSVG);
   pOutSVG = 0;
}

void HLines::dashLinesToggle() {
   dashed = !dashed;
   dashAct->setChecked(dashed);
   repaint();
}

void HLines::showNrsToggle() {
   showNrs = !showNrs;
   showNrsAct->setChecked(showNrs);
   repaint();
}

qreal distance2(Point2D p, Point2D q) {
   qreal dx = p.x - q.x, dy = p.y - q.y;
   return dx * dx + dy * dy;
}

void HLines::linesegment(int iP, int iQ, int iTrStart) {
   Point2D pScr = scr[iP], qScr = scr[iQ];
   Point3D P = eye[iP], Q = eye[iQ];
   qreal u1 = qScr.x - pScr.x, u2 = qScr.y - pScr.y,
      PQxMin = min(pScr.x, qScr.x),
      PQxMax = pScr.x + qScr.x - PQxMin,
      PQyMin = min(pScr.y, qScr.y),
      PQyMax = pScr.y + qScr.y - PQyMin,
      zP = P.z, zQ = Q.z, // P and Q give eye coordinates
      PQzFar = min(zP, zQ);
   for (int iTr = iTrStart; iTr < relatedtrianrs.size(); iTr++) {
      int itria = relatedtrianrs[iTr];
      Tria t = tria[itria];
      int polnr = polnrs[itria];
      Polygon3D polygon = polyList[polnr];
      int iA = abs(t.A), iB = abs(t.B), iC = abs(t.C);
      Point3D A = eye[iA], B = eye[iB], C = eye[iC];
      Point2D aScr = scr[iA], bScr = scr[iB], cScr = scr[iC];
      qreal xA = aScr.x, yA = aScr.y, xB = bScr.x, yB = bScr.y,
         xC = cScr.x, yC = cScr.y;

      // Test 1 (2D). Are both P and Q on one side of the
      // infinite line AB, and C on the other?
      // Similar tests for the lines BC and CA.
      int ABP = Triangulate::orientation(scr, iA, iB, iP);
      if (ABP <= 0 && ABP +
         Triangulate::orientation(scr, iA, iB, iQ) < 0)
         continue; // Triangle does not obscure PQ.
      int BCP = Triangulate::orientation(scr, iB, iC, iP);
      if (BCP <= 0 && BCP +
         Triangulate::orientation(scr, iB, iC, iQ) < 0)
         continue; // Triangle does not obscure PQ.
      int CAP = Triangulate::orientation(scr, iC, iA, iP);
      if (CAP <= 0 && CAP +
         Triangulate::orientation(scr, iC, iA, iQ) < 0)
         continue; // Triangle does not obscure PQ.

      // Test 2 (2D). Are A, B and C lie on the same side
      // of the infinite line through P and Q?
      if (abs(Triangulate::orientation(scr, iP, iQ, iA) +
         Triangulate::orientation(scr, iP, iQ, iB) +
         Triangulate::orientation(scr, iP, iQ, iC)) >= 2)
         continue;

      // Test 3. Are P and Q nearer than the infinite plane
      // through A, B and C, or do they lie in that plane?
      qreal a = polygon.a, b = polygon.b, c = polygon.c,
         h = polygon.h,
         hP = a * P.x + b * P.y + c * P.z,
         hQ = a * Q.x + b * Q.y + c * Q.z;
      bool hPfar = nearer(h, hP), hQfar = nearer(h, hQ);
      if (!hPfar && !hQfar)
         continue;

      // Test 4. Check if both P and Q behind triangle ABC
      // and farther away (stronger negative)
      bool pInside =
         Triangulate::insideTriangle(scr, iA, iB, iC, iP);
      bool qInside =
         Triangulate::insideTriangle(scr, iA, iB, iC, iQ);
      if (pInside && qInside && hPfar && hQfar)
         return; // This triangle obscures PQ.

      // Test 5. Does PQ penetrate triangle ABC?
      if (hPfar != hQfar) {
         qreal xPQ = Q.x - P.x, yPQ = Q.y - P.y, zPQ = Q.z - P.z,
            lambda = (h - (P.x * a + P.y * b + P.z * c)) /
            (xPQ * a + yPQ * b + zPQ * c);
         if (lambda > 0.001 && lambda < 0.999) {
            // PQ intersects plane ABC in point S:
            qreal xSeye = P.x + lambda * xPQ,
               ySeye = P.y + lambda * yPQ,
               zSeye = P.z + lambda * zPQ;
            qreal xSscr = -xSeye / zSeye, ySscr = -ySeye / zSeye;
            int n = scr.size();
            scr.push_back(Point2D(xSscr, ySscr));
            bool done = false;
            if (lambda > 0.001 && lambda < 0.999 &&
               Triangulate::insideTriangle(scr, iA, iB, iC, n)) {
               eye.push_back(Point3D(xSeye, ySeye, zSeye));
               // Replace PQ with PS and QS:
               linesegment(iP, n, iTr);
               linesegment(iQ, n, iTr);
               eye.pop_back();
               done = true; // Done with PQ
            }
            scr.pop_back();
            if (done)
               return;
         }
      }

      // Test 6. Is PQ is partly visible and partly invisible?
      // Compute the intersections I and J of PQ
      // with ABC in 2D.
      // If the corresponding points of intersection in 3D lie
      // in front of ABC, this triangle does not obscure PQ.
      // Otherwise, the intersections lie behind ABC and
      // this triangle obscures part of PQ:
      if (nearer(hP, h) && pInside || nearer(hQ, h) && qInside)
         continue;
      qreal lambdaMin = 1.0, lambdaMax = 0.0;

      for (int ii = 0; ii < 3; ii++) {
         qreal v1 = bScr.x - aScr.x, v2 = bScr.y - aScr.y,
            w1 = aScr.x - pScr.x, w2 = aScr.y - pScr.y,
            denom = u2 * v1 - u1 * v2;
         if (denom != 0) {
            qreal mu = (u1 * w2 - u2 * w1) / denom;
            // mu = 0 gives A and mu = 1 gives B.
            if (mu > -0.0001 && mu < 1.0001) {
               qreal lambda = (v1 * w2 - v2 * w1) / denom;
               // lambda = PI/PQ
               // (I is point of intersection)
               if (lambda > -0.0001 && lambda < 1.0001) {
                  if (pInside != qInside &&
                     lambda > 0.0001 && lambda < 0.9999) {
                     lambdaMin = lambdaMax = lambda;
                     break; // Only one point of intersection
                  }
                  if (lambda < lambdaMin) lambdaMin = lambda;
                  if (lambda > lambdaMax) lambdaMax = lambda;
               }
            }
         }
         Point2D temp = aScr;
         aScr = bScr; bScr = cScr; cScr = temp;
      }
      qreal epsDist2 = 1e-9;
      if (!pInside && lambdaMin > 0.001) {
         qreal iScrx = pScr.x + lambdaMin * u1,
            iScry = pScr.y + lambdaMin * u2;
         // Back to eye coordinates:
         qreal zI = 1 / (lambdaMin / zQ + (1 - lambdaMin) / zP),
            xI = -zI * iScrx, yI = -zI * iScry;
         if (nearer(a * xI + b * yI + c * zI, h))
            continue; // This triangle does not obscure PQ.
         Point2D iScr(iScrx, iScry);
         if (distance2(iScr, pScr) > epsDist2) {
            int n = scr.size();
            scr.push_back(iScr);
            eye.push_back(Point3D(xI, yI, zI));
            linesegment(iP, n, iTr + 1);
            scr.pop_back();
            eye.pop_back();
         }
      }
      if (!qInside && lambdaMax < 0.999) {
         qreal jScrx = pScr.x + lambdaMax * u1,
            jScry = pScr.y + lambdaMax * u2;
         qreal zJ = 1 / (lambdaMax / zQ + (1 - lambdaMax) / zP),
            xJ = -zJ * jScrx, yJ = -zJ * jScry;
         if (nearer(a * xJ + b * yJ + c * zJ, h))
            continue; // This triangle does not obscure PQ.
         Point2D jScr(jScrx, jScry);
         if (distance2(jScr, qScr) > epsDist2) {
            int n = scr.size();
            scr.push_back(jScr);
            eye.push_back(Point3D(xJ, yJ, zJ));
            linesegment(iQ, n, iTr + 1);
            scr.pop_back();
            eye.pop_back();
         }
      }
      return; // if no continue-statement has been executed
   }
   PointPair line = { pScr.x, pScr.y, qScr.x, qScr.y };
   todraw.push_back(line);
}

void HLines::myDrawLine(QPainter &pnt, bool dashSVG,
   int x1, int y1, int x2, int y2)const {
   pnt.drawLine(x1, y1, x2, y2);
   if (pOutSVG != 0) {
      *pOutSVG << "<line x1=\"" << x1 << "\" y1=\"" << y1
         << "\" x2=\"" << x2 << "\" y2=\"" << y2;
      if (dashSVG)
         *pOutSVG <<
         "\" stroke=\"black\" stroke-dasharray=\"5\"/>";
      else
         *pOutSVG <<
         "\"  style=\"stroke:#000000; stroke-width: 1;\"/>";
      *pOutSVG << endl;
   }
}

void HLines::collectTrias(int i, int j) {
   // Collect triangles for rect (i, j)
   vector<int> rectTrianrs = screenrect[i][j].trianrs;
   int nrel = relatedtrianrs.size(), nrect = rectTrianrs.size();
   vector<int>result(nrel + nrect);
   sort(rectTrianrs.begin(), rectTrianrs.end());
   vector<int>::iterator it =
      set_union(relatedtrianrs.begin(), relatedtrianrs.end(),
         rectTrianrs.begin(), rectTrianrs.end(),
         result.begin());
   int newsize = it - result.begin();
   relatedtrianrs.resize(newsize); //   (result.size());
   copy(result.begin(), it, // result.end(),
      relatedtrianrs.begin());
}

void HLines::mouseReleaseEvent(QMouseEvent *event) {
   if (thread.isRunning())
      return;
   Basic3D::mouseReleaseEvent(event);
   QPoint release = event->pos();
   int dx = release.x() - xPress, dy = release.y() - yPress;
   if (abs(dx) + abs(dy) >= 5) // Otherwise not worthwhile
      drawImage();
}

void HLines::paintEvent(QPaintEvent *event) {
   if (!thread.isFinished())
      return;
   QPainter pnt(this);
   QPen solidPen(Qt::black, penWidth), dashPen;
   QVector<qreal> dashes;
   QFont font;
   font.setPixelSize(12);
   pnt.setFont(font);
   dashes << 6 << 8;
   dashPen.setDashPattern(dashes);
   if (showNrs) {
      for (int i = 0; i < scr.size(); i++) {
         if (world[i].x > 1e29)
            continue;
         qreal x = scr[i].x, y = scr[i].y;
         pnt.drawText(xDev(x), yDev(y), QString::number(i));
      }
   }
   if (dashed) {
      pnt.setPen(dashPen);
      for (int i = 0; i < polyList.size(); i++) {
         Polygon3D polygon = polyList[i];
         int n = polygon.nrs.size();
         for (int k = 0; k < n; k++) {
            int from = abs(polygon.nrs[k]),
               to = polygon.nrs[(k + 1) % n];
            if (to >= 0) {
               // Since lines usually belong to two faces,
               // they can be drawn twice. In view of the dashes,
               // we draw them in the same direction (upward):
               Point2D P = scr[from];
               Point2D Q = scr[to];
               if (P.y > Q.y) {
                  P = scr[to];
                  Q = scr[from];
               }
               myDrawLine(pnt, true, xDev(P.x), yDev(P.y),
                  xDev(Q.x), yDev(Q.y));
            }
         }
      }
   }
   pnt.setPen(solidPen);
   for (int i = 0; i < todraw.size(); ++i) {
      qreal xP = todraw[i].xP, yP = todraw[i].yP,
         xQ = todraw[i].xQ, yQ = todraw[i].yQ;
      myDrawLine(pnt, false, xDev(xP), yDev(yP),
         xDev(xQ), yDev(yQ));
   }
}

void HLines::closeEvent(QCloseEvent *event) {
   appClosed = true;
   progressBar.hide();
   thread.terminate();
   event->accept();
}

void MyThread::run() {
   HLines *p = (HLines *)(parent());
   p->calculate();
}

void HLines::addToThreadStart() {
   // Derived classes can override this
}

void HLines::addToThreadFinish() {
   // Derived classes can override this
}

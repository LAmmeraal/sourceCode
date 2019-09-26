// hfaces.cpp: Hidden faces, Z-buffer algorithm.
#include "hfaces.h"

void HFaces::allocatebuffer() {
   // Delete old buffer, if hbuf > 0:
   for (int i = 0; i < hbuf; i++) {
      delete[] buf[i];
   }
   if (hbuf)
      delete[] buf;
   // Allocate new buffer:
   wbuf = width();
   hbuf = height();
   buf = new float*[hbuf];
   for (int i = 0; i < hbuf; i++) {
      buf[i] = new float[wbuf];
   }
}

HFaces::HFaces(QWidget *parent) {
   setWindowTitle("Hidden-face model");
}

int HFaces::colorCode(qreal a, qreal b, qreal c) const {
   qreal inprod = a * sunX + b * sunY + c * sunZ;
   return int(round((inprod - inprodMin)
      / (inprodRange + 0.1) * 255));
   // "+ 0.1" to prevent zero division
}

void HFaces::paintEvent(QPaintEvent *e) {
   newObjectTest();
   if (hbuf != height() || wbuf != width()) {
      computePerspCoord();
      allocatebuffer();
   }
   for (int i = 0; i < hbuf; i++)
      for (int j = 0; j < wbuf; j++)
         buf[i][j] = 1e30f;
   inprodMin = 1e30;
   inprodMax = -1e30;
   tria.clear();
   polnrs.clear();
   vector<Point2D> scrScaled;
   int nScr = scr.size();
   scrScaled.resize(nScr);
   for (int i = 0; i < nScr; i++) {
      scrScaled[i].x = d * scr[i].x;
      scrScaled[i].y = d * scr[i].y;
   }

   for (int i = 0; i < polyList.size(); i++) {
      Polygon3D polygon = polyList[i];
      if (!Triangulate::isCounterclockwise(scrScaled, polygon.nrs))
         continue;
      // Now we compute a, b, c and h of the equation
      // ax + by + cz = h for the plane of each polygon:
      int iA = abs(polygon.nrs[0]),
         iB = abs(polygon.nrs[1]),
         iC = abs(polygon.nrs[2]);
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
      polygon.a = a; polygon.b = b; polygon.c = c; polygon.h = h;
      polyList[i] = polygon;
      qreal inprod = a * sunX + b * sunY + c * sunZ;
      if (inprod < inprodMin) inprodMin = inprod;
      if (inprod > inprodMax) inprodMax = inprod;
      // Triangulation:
      vector<Tria> triaExtra;
      Triangulate::triangulate(scrScaled, polygon.nrs, triaExtra);
      for (int j = 0; j < triaExtra.size(); j++) {
         tria.push_back(triaExtra[j]);
         polnrs.push_back(i);
      }
   }
   inprodRange = inprodMax - inprodMin;

   QPainter pnt(this);
   for (int i = 0; i < tria.size(); i++) {
      Tria tr = tria[i];
      int polnr = polnrs[i];
      int color = colorCode(polyList[polnr].a,
         polyList[polnr].b, polyList[polnr].c);
      pnt.setPen(QColor(color, color, 0, 255));
      int iA = tr.A, iB = tr.B, iC = tr.C;
      qreal xA = scrScaled[iA].x, yA = scrScaled[iA].y,
         xB = scrScaled[iB].x, yB = scrScaled[iB].y,
         xC = scrScaled[iC].x, yC = scrScaled[iC].y;
      qreal ZA = 1 / eye[iA].z, ZB = 1 / eye[iB].z,
         ZC = 1 / eye[iC].z;
      // We now compute the partial derivatives dZdX and dZdY:
      qreal u1 = xB - xA, v1 = xC - xA,
         u2 = yB - yA, v2 = yC - yA,
         det = u1 * v2 - u2 * v1;
      if (det <= 0)
         continue;
      qreal u3 = ZB - ZA, v3 = ZC - ZA,
         dZdX = (u3 * v2 - u2 * v3) / det,
         dZdY = (u1 * v3 - u3 * v1) / det,
         yBottomR = min(yA, min(yB, yC)),
         yTopR = max(yA, max(yB, yC));
      int yBottom = int(ceil(yBottomR)),
         yTop = int(floor(yTopR));
      qreal dxdyAB = 1e30, dxdyBC = 1e30, dxdyCA = 1e30;
      if (yA != yB) dxdyAB = (xA - xB) / (yA - yB);
      if (yB != yC) dxdyBC = (xB - xC) / (yB - yC);
      if (yC != yA) dxdyCA = (xC - xA) / (yC - yA);
      ZA += 0.001 * (ZA + ZB + ZC);
      // to get the right color on silhouette points
      for (int y = yBottom; y <= yTop; y++) {
         // Compute horizontal line segment (xL, xR)
         // for coordinate y:
         qreal xBCLeft, xCALeft, xABLeft, xBCRight,
            xCARight, xABRight, xL, xR;
         xBCLeft = xCALeft = xABLeft = 1e30;
         xBCRight = xCARight = xABRight = -1e30;
         if ((y > yB) != (y > yC)) // y between yB and yC?
            xBCLeft = xBCRight = xC + (y - yC) * dxdyBC;
         if ((y > yC) != (y > yA)) // y between yC and yA?
            xCALeft = xCARight = xA + (y - yA) * dxdyCA;
         if ((y > yA) != (y > yB)) // y between yA and yB?
            xABLeft = xABRight = xB + (y - yB) * dxdyAB;
         xL = min(xBCLeft, min(xCALeft, xABLeft));
         xR = max(xBCRight, max(xCARight, xABRight));
         int ydev = yDev(y), xLdev = xDev(xL + 0.5),
            xRdev = xDev(xR - 0.5);
         if (ydev >= hbuf)
            continue;
         qreal Z = ZA + (y - yA) * dZdY + (xL - xA) * dZdX;
         for (int xdev = xLdev; xdev <= xRdev; xdev++) {
            if (Z < buf[ydev][xdev]) { // < is nearer
               pnt.drawLine(xdev, ydev, xdev, ydev);
               buf[ydev][xdev] = float(Z);
            }
            Z += dZdX;
         }
      }
   }
}

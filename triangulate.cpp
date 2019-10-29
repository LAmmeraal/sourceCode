// triangulate.cpp: Triangulation of polygons.
#include "triangulate.h"
using namespace std;

bool Triangulate::triangulate(const vector<Point2D> &scr,
   vector<int> pol, vector<Tria> &trias) {
   int nv = pol.size(), k = nv;

   while (pol.size() > 2 && k-- > 0) {
      // Find a suitable triangle, consisting of two
      // polygon edges and an internal diagonal:
      bool triaFound = false;
      int count = nv;
      for (int iA = 0; !triaFound; iA = (iA + 1) % pol.size()) {
         if (count-- < 0)
            return false; // Not a simple polygon
         int iB = (iA + 1) % pol.size(),
            iC = (iA + 2) % pol.size();
         int A = abs(pol[iA]), B = abs(pol[iB]), C = abs(pol[iC]);
         if (orientation(scr, A, B, C) >= 0) {
            // Edges AB and BC; diagonal AC
            // Test to see if none of the other polygon vertices
            // lies within triangle ABC:
            int i = -1;
            for (i = (iC + 1) % pol.size(); i != iA;
               i = (i + 1) % pol.size()) {
               int P = abs(pol[i]);
               // If P is different from A, B and C, and P lies
               // inside triangle ABC, we reject triangle ABC:
               if (P != A && P != B && P != C
                  && insideTriangle(scr, A, B, C, P))
                  break;
            }
            if (i == iA) {
               // Triangle ABC contains no other vertices:
               trias.push_back(Tria{ A, B, C });
               pol.erase(pol.begin() + iB); // Reduce polygon
               triaFound = true;
            }
         }
      }
   }
   while (anyFlipping(scr, trias))
      ;  // Change diagonals of quadrangles if desirable...
   return true;
}

bool Triangulate::anyFlipping(const vector<Point2D> &scr,
   vector<Tria> &tr) {
   int length = tr.size();
   if (length < 2)
      return false;
   for (int i = 0; i < tr.size(); i++) {
      int t[] = { tr[i].A, tr[i].B, tr[i].C };
      for (int j = i + 1; j < length; j++) {
         int u[] = { tr[j].A, tr[j].B, tr[j].C };
         // Look for a common edge of triangles t and u:
         for (int h = 0; h < 3; h++) {
            for (int k = 0; k < 3; k++) {
               if (t[h] == u[k] &&
                  t[(h + 1) % 3] == u[(k + 2) % 3]) {
                  int iP = t[(h + 1) % 3], iQ = t[(h + 2) % 3],
                     iR = t[h], iS = u[(k + 1) % 3];
                  // Currently, diagonal PR divides the quadrangle.
                  // Do flipping if QS a better choice:
                  if (angle(scr, iP, iQ, iR) +
                     angle(scr, iR, iS, iP) > 3.1416) {
                     tr[i].A = iP; tr[i].B = iQ; tr[i].C = iS;
                     tr[j].A = iS; tr[j].B = iQ; tr[j].C = iR;
                     return true;
                  }
               }
            }
         }
      }
   }
   return false;
}

bool Triangulate::insideTriangle(const vector<Point2D> &scr,
   int A, int B, int C, int P) {
   // ABC is assumed to be counter-clockwise
   return orientation(scr, A, B, P) >= 0 &&
      orientation(scr, B, C, P) >= 0 &&
      orientation(scr, C, A, P) >= 0;
}

int Triangulate::orientation(const vector<Point2D> &scr,
   int Pnr, int Qnr, int Rnr) {
   qreal u1 = scr[Qnr].x - scr[Pnr].x,
      u2 = scr[Qnr].y - scr[Pnr].y,
      v1 = scr[Rnr].x - scr[Pnr].x, v2 = scr[Rnr].y - scr[Pnr].y;
   qreal det = u1 * v2 - u2 * v1;
   return det > 0 ? 1 : det < 0 ? -1 : 0;

}

qreal Triangulate::angle(const vector<Point2D> &scr,
   int A, int B, int C) {
   // Return angle ABC
   qreal xa = scr[A].x, ya = scr[A].y,
      xb = scr[B].x, yb = scr[B].y,
      xc = scr[C].x, yc = scr[C].y,
      u1 = xa - xb, u2 = ya - yb,
      v1 = xc - xb, v2 = yc - yb,
      dotproduct = u1 * v1 + u2 * v2,
      ulength = sqrt(u1 * u1 + u2 * u2),
      vlength = sqrt(v1 * v1 + v2 * v2),
      denom = ulength * vlength, cosB;
   cosB = dotproduct / denom;
   if (cosB < -1) cosB = -1; // May occur due to
   if (cosB > 1) cosB = 1;   // rounding off errors
   return acos(cosB);
}

bool Triangulate::isCounterclockwise(const vector<Point2D> &scr,
   const vector<int> &nrs) {
   qreal xRight = -1e30, x;
   int n = nrs.size(), j1 = 0, i = -1;
   do {
      int j = (++i) % n;
      int k = abs(nrs[j]);
      x = scr[k].x;
      if (x >= xRight) {
         xRight = x;
         j1 = j;
      }
   } while (i < n || x == xRight);
   int k = abs(nrs[j1]), kPrev = abs(nrs[(j1 + n - 1) % n]),
      kNext = abs(nrs[(j1 + 1) % n]);
   return orientation(scr, kPrev, k, kNext) == 1;
}
// rota3d.cpp: Rotation about arbitrary axis.
#include "rota3d.h"

Rota3D::Rota3D(const Point3D &a, const Point3D &b,
   qreal alpha) {
   qreal v1 = b.x - a.x, v2 = b.y - a.y, v3 = b.z - a.z,
      cosa = cos(alpha), sina = sin(alpha), c = 1 - cosa,
      theta = atan2(v2, v1),
      costh = cos(theta), cos2th = costh * costh,
      sinth = sin(theta), sin2th = sinth * sinth,
      phi = atan2(sqrt(v1 * v1 + v2 * v2), v3),
      cosphi = cos(phi), cos2phi = cosphi * cosphi,
      sinphi = sin(phi), sin2phi = sinphi * sinphi;

   r11 = cos2th * (cosa * cos2phi + sin2phi) + cosa * sin2th;
   r12 = sina * cosphi + c * sin2phi * costh * sinth;
   r13 = sinphi * (c * cosphi * costh - sina * sinth);

   r21 = c * sin2phi * costh * sinth - sina * cosphi;
   r22 = sin2th * (cosa * cos2phi + sin2phi) + cosa * cos2th;
   r23 = sinphi * (c * cosphi * sinth + sina * costh);

   r31 = sinphi * (c * cosphi * costh + sina * sinth);
   r32 = sinphi * (c * cosphi * sinth - sina * costh);
   r33 = cosa * sin2phi + cos2phi;

   r41 = a.x - a.x * r11 - a.y * r21 - a.z * r31;
   r42 = a.y - a.x * r12 - a.y * r22 - a.z * r32;
   r43 = a.z - a.x * r13 - a.y * r23 - a.z * r33;
}

void Rota3D::rotate(const Point3D &p, Point3D &p1) {
   qreal x = p.x, y = p.y, z = p.z;
   p1.x = x * r11 + y * r21 + z * r31 + r41;
   p1.y = x * r12 + y * r22 + z * r32 + r42;
   p1.z = x * r13 + y * r23 + z * r33 + r43;
}

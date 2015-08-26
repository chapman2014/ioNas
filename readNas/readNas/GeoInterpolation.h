// Gmsh - Copyright (C) 1997-2014 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#ifndef _GEO_INTERPOLATION_H_
#define _GEO_INTERPOLATION_H_


#include "Geo.h"

Vertex InterpolateCurve(Curve *Curve, double u, int derivee);

#endif

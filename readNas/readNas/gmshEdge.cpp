#include "stdafx.h"
#include <sstream>
#include "GModel.h"
#include "gmshEdge.h"
#include "Context.h"

#include "GeoInterpolation.h"

gmshEdge::gmshEdge  (GModel *m, Curve *edge, GVertex *v1, GVertex *v2)
	:GEdge(m, edge->Num, v1, v2), c(edge)
{
	resetMeshAttributes();
}

void gmshEdge::resetMeshAttributes()
{
	meshAttributes.method = c->Method;
	meshAttributes.nbPointsTransfinite = c->nbPointsTransfinite;
	meshAttributes.typeTransfinite = c->typeTransfinite;
	meshAttributes.coeffTransfinite = c->coeffTransfinite;
//	meshAttributes.extrude = c->Extrude;//
	meshAttributes.reverseMesh = c->ReverseMesh;
}

Range<double> gmshEdge::parBounds(int i) const
{
	return Range<double>(c->ubeg, c->uend);
}

GPoint gmshEdge::point(double par) const
{

	Vertex a = InterpolateCurve(c, par, 0);
	return GPoint(a.Pos.X, a.Pos.Y, a.Pos.Z, this, par);
}

SVector3 gmshEdge::firstDer(double par) const
{
	Vertex a = InterpolateCurve(c, par, 1);
	return SVector3(a.Pos.X, a.Pos.Y, a.Pos.Z);
}

SVector3 gmshEdge::secondDer(double par) const
{

	Vertex a = InterpolateCurve(c, par, 2);
	return SVector3(a.Pos.X, a.Pos.Y, a.Pos.Z);
}
#include "StdAfx.h"
#include <sstream>
#include <algorithm>

#include "GModel.h"
#include "GEdge.h"

#include "MLine.h"

#include "Context.h"


SVector3 GEdge::secondDer(double par) const
{
	// use central differences
	const double eps = 1.e-3;
	Range<double> rg = parBounds(0);
	if (par-eps <= rg.low()){
		SVector3 x1 = firstDer(par);
		SVector3 x2 = firstDer(par + eps);
		return 1000 * (x2 - x1);
	}
	else if (par+eps >= rg.high()){
		SVector3 x1 = firstDer(par-eps);
		SVector3 x2 = firstDer(par);
		return 1000 * (x2 - x1);
	}
	SVector3 x1 = firstDer(par - eps);
	SVector3 x2 = firstDer(par + eps);
	return 500 * (x2 - x1);
}

double GEdge::curvature(double par) const
{
	SVector3 d1 = firstDer(par);
	SVector3 d2 = secondDer(par);

	double one_over_norm = 1. / norm(d1);

	SVector3 cross_prod = crossprod(d1,d2);

	return ( norm(cross_prod) * one_over_norm * one_over_norm * one_over_norm );
}
#include "GModel.h"
#include "meshGEdge.h"
#include "GEdge.h"
#include "MLine.h"
#include "Range.h"

#define SQU(a)      ((a)*(a))

typedef struct {
	int Num;
	// t is the local coordinate of the point
	// lc is x'(t)/h(x(t))
	// p is the value of the primitive
	// xp is the norm of the tangent vector
	double t, lc, p, xp;
} IntPoint;

static double smoothPrimitive(GEdge *ge, double alpha,
	std::vector<IntPoint> &Points)
{
	int ITER = 0;
	while (1){
		int count1 = 0;
		int count2 = 0;

		// use a gauss-seidel iteration; iterate forward and then backward;
		// convergence is usually very fast
		for (unsigned int i = 1; i < Points.size(); i++){
			double dh = (Points[i].xp/Points[i].lc - Points[i-1].xp/Points[i-1].lc);
			double dt = Points[i].t - Points[i-1].t;
			double dhdt =  dh/dt;
			if (dhdt / Points[i].xp > (alpha - 1.)*1.01){
				double hnew = Points[i-1].xp / Points[i-1].lc + dt * (alpha-1) * Points[i].xp;
				Points[i].lc = Points[i].xp / hnew;
				count1++;
			}
		}

		for (int i = Points.size() - 1; i > 0; i--){
			double dh = (Points[i-1].xp/Points[i-1].lc - Points[i].xp/Points[i].lc);
			double dt = fabs(Points[i-1].t - Points[i].t);
			double dhdt =  dh/dt;
			if (dhdt / Points[i-1].xp > (alpha-1.)*1.01){
				double hnew = Points[i].xp / Points[i].lc + dt * (alpha-1) * Points[i].xp;
				Points[i-1].lc = Points[i-1].xp / hnew;
				count2 ++;
			}
		}

		++ITER;
		if (ITER > 2000) break;
		if (!(count1 + count2)) break;
	}

	// recompute the primitive
	for (int i = 1; i < (int)Points.size(); i++){
		IntPoint &pt2 = Points[i];
		IntPoint &pt1 = Points[i-1];
		pt2.p = pt1.p + (pt2.t - pt1.t) * 0.5 * (pt2.lc + pt1.lc);
	}
	return Points[Points.size() - 1].p;
}


static double F_Lc(GEdge *ge, double t)
{
	GPoint p = ge->point(t);
	double lc_here;

	Range<double> bounds = ge->parBounds(0);
	double t_begin = bounds.low();
	double t_end = bounds.high();

	if(t == t_begin)
		lc_here = BGM_MeshSize(ge->getBeginVertex(), t, 0, p.x(), p.y(), p.z());
	else if(t == t_end)
		lc_here = BGM_MeshSize(ge->getEndVertex(), t, 0, p.x(), p.y(), p.z());
	else
		lc_here = BGM_MeshSize(ge, t, 0, p.x(), p.y(), p.z());

	SVector3 der = ge->firstDer(t);
	const double d = norm(der);

	return d / lc_here;
}


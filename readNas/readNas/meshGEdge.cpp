#include "StdAfx.h"
#include "GModel.h"
#include "meshGEdge.h"
#include "GEdge.h"
#include "MLine.h"
#include "Range.h"
#include "BackgroundMesh.h"
#include "Context.h"

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

static double F_One(GEdge *ge, double t)
{
	SVector3 der = ge->firstDer(t) ;
	return norm(der);
}

static double trapezoidal(IntPoint * P1, IntPoint * P2)
{
	return (0.5 * (P1->lc + P2->lc) * (P2->t - P1->t));//积分 梯形面积 实质是P1到P2的曲线长度
}

static void RecursiveIntegration(GEdge *ge, IntPoint *from, IntPoint *to,
	double (*f) (GEdge *e, double X),
	std::vector<IntPoint> &Points,
	double Prec, int *depth)
{
	IntPoint P, p1;

	(*depth)++;

	P.t = 0.5 * (from->t + to->t);
	P.lc = f(ge, P.t);

	double val1 = trapezoidal(from, to);
	double val2 = trapezoidal(from, &P);
	double val3 = trapezoidal(&P, to);
	double err = fabs(val1 - val2 - val3);

	if(((err < Prec) && (*depth > 6)) || (*depth > 25)) {
		p1 = Points.back();
		P.p = p1.p + val2;
		Points.push_back(P);

		p1 = Points.back();
		to->p = p1.p + val3;
		Points.push_back(*to);
	}
	else {
		RecursiveIntegration(ge, from, &P, f, Points, Prec, depth);
		RecursiveIntegration(ge, &P, to, f, Points, Prec, depth);
	}

	(*depth)--;
}

static double Integration(GEdge *ge, double t1, double t2,
	double (*f) (GEdge *e, double X),
	std::vector<IntPoint> &Points, double Prec)
{
	IntPoint from, to;

	int depth = 0;

	from.t = t1;
	from.lc = f(ge, from.t);
	from.p = 0.0;
	Points.push_back(from);

	to.t = t2;
	to.lc = f(ge, to.t);

	RecursiveIntegration(ge, &from, &to, f, Points, Prec, &depth);

	return Points.back().p;
}

void meshGEdge::operator() (GEdge *ge)
{
//#if defined(HAVE_ANN)
//	FieldManager *fields = ge->model()->getFields();
//	BoundaryLayerField *blf = 0;
//	Field *bl_field = fields->get(fields->getBoundaryLayerField());
//	blf = dynamic_cast<BoundaryLayerField*> (bl_field);
//#else
//	bool blf = false;
//#endif

	ge->model()->setCurrentMeshEntity(ge);

	//if(ge->geomType() == GEntity::DiscreteCurve) return;
	//if(ge->geomType() == GEntity::BoundaryLayerCurve) return;
	//if(ge->meshAttributes.method == MESH_NONE) return;
	//if(CTX::instance()->mesh.meshOnlyVisible && !ge->getVisibility()) return;

	// look if we are doing the STL triangulation
	std::vector<MVertex*> &mesh_vertices = ge->mesh_vertices ;
	std::vector<MLine*> &lines = ge->lines ;

	deMeshGEdge dem;
	dem(ge);   //

	//if(MeshExtrudedCurve(ge)) return;

	/*if (ge->meshMaster() != ge->tag()){
		GEdge *gef = ge->model()->getEdgeByTag(abs(ge->meshMaster()));
		if (gef->meshStatistics.status == GEdge::PENDING) return;
		Msg::Info("Meshing curve %d (%s) as a copy of %d", ge->tag(),
			ge->getTypeString().c_str(), ge->meshMaster());
		copyMesh(gef, ge, ge->meshMaster());
		ge->meshStatistics.status = GEdge::DONE;
		return;
	}

	Msg::Info("Meshing curve %d (%s)", ge->tag(), ge->getTypeString().c_str());*/

	// compute bounds
	Range<double> bounds = ge->parBounds(0);
	double t_begin = bounds.low();
	double t_end = bounds.high();

	// first compute the length of the curve by integrating one
	double length;
	std::vector<IntPoint> Points;
	if(ge->geomType() == GEntity::Line && ge->getBeginVertex() == ge->getEndVertex())
		length = 0.; // special case t avoid infinite loop in integration
	else
		length = Integration(ge, t_begin, t_end, F_One, Points, 1.e-8 * CTX::instance()->lc);
	ge->setLength(length);
	Points.clear();

	if(length < CTX::instance()->mesh.toleranceEdgeLength){
		ge->setTooSmall(true);
	}

	// Integrate detJ/lc du
	double a;
	int N;
	if(length == 0. && CTX::instance()->mesh.toleranceEdgeLength == 0.){
		printf("Curve %d has a zero length", ge->tag());
		a = 0.;
		N = 1;
	} else{
		a = Integration(ge, t_begin, t_end, F_Lc, Points,
			CTX::instance()->mesh.lcIntegrationPrecision);
	}
	// if the curve is periodic and if the begin vertex is identical to
	// the end vertex and if this vertex has only one model curve
	// adjacent to it, then the vertex is not connecting any other
	// curve. So, the mesh vertex and its associated geom vertex are not
	// necessary at the same location
	GPoint beg_p, end_p;
	if(ge->getBeginVertex() == ge->getEndVertex() &&
		ge->getBeginVertex()->edges().size() == 1){
			end_p = beg_p = ge->point(t_begin);
			printf("Meshing periodic closed curve");
	}
	else{
		MVertex *v0 = ge->getBeginVertex()->mesh_vertices[0];
		MVertex *v1 = ge->getEndVertex()->mesh_vertices[0];
		beg_p = GPoint(v0->x(), v0->y(), v0->z());
		end_p = GPoint(v1->x(), v1->y(), v1->z());
	}

	// do not consider the first and the last vertex (those are not
	// classified on this mesh edge)
	if(N > 1){
		const double b = a / (double)(N - 1);
		int count = 1, NUMP = 1;
		IntPoint P1, P2;
		mesh_vertices.resize(N - 2);
		while(NUMP < N - 1) {
			P1 = Points[count - 1];
			P2 = Points[count];
			const double d = (double)NUMP * b;
			if((fabs(P2.p) >= fabs(d)) && (fabs(P1.p) < fabs(d))) {
				double dt = P2.t - P1.t;
				double dlc = P2.lc - P1.lc;
				double dp = P2.p - P1.p;
				double t   = P1.t + dt / dp * (d - P1.p);//线性的
				SVector3 der = ge->firstDer(t);
				const double d = norm(der);
				double lc  = d/(P1.lc + dlc / dp * (d - P1.p));
				GPoint V = ge->point(t);
				mesh_vertices[NUMP - 1] = new MEdgeVertex(V.x(), V.y(), V.z(), ge, t, lc);
				NUMP++;
			}
			else {
				count++;
			}
		}
		mesh_vertices.resize(NUMP - 1);
	}

	for(unsigned int i = 0; i < mesh_vertices.size() + 1; i++){
		MVertex *v0 = (i == 0) ?
			ge->getBeginVertex()->mesh_vertices[0] : mesh_vertices[i - 1];
		MVertex *v1 = (i == mesh_vertices.size()) ?
			ge->getEndVertex()->mesh_vertices[0] : mesh_vertices[i];
		lines.push_back(new MLine(v0, v1));
	}

	if(ge->getBeginVertex() == ge->getEndVertex() &&
		ge->getBeginVertex()->edges().size() == 1){
			MVertex *v0 = ge->getBeginVertex()->mesh_vertices[0];
			v0->x() = beg_p.x();
			v0->y() = beg_p.y();
			v0->z() = beg_p.z();
	}

	ge->meshStatistics.status = GEdge::DONE;
}
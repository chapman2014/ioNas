#include "StdAfx.h"
#include "BackgroundMesh.h"
#include "GVertex.h"
#include "GEdge.h"
#include "GModel.h"
#include "MLine.h"
#include "MTriangle.h"
#include "Context.h"


static double max_edge_curvature(const GVertex *gv)
{
	double val = 0;
	std::list<GEdge*> l_edges = gv->edges();
	for (std::list<GEdge*>::const_iterator ite = l_edges.begin();
		ite != l_edges.end(); ++ite){
			GEdge *_myGEdge = *ite;
			Range<double> range = _myGEdge->parBounds(0);
			double cc;
			if (gv == _myGEdge->getBeginVertex()) cc = _myGEdge->curvature(range.low());
			else cc = _myGEdge->curvature(range.high());
			val = std::max(val, cc);
	}
    return val;
}


// the mesh vertex is classified on a model vertex.  we compute the
// maximum of the curvature of model faces surrounding this point if
// it is classified on a model edge, we do the same for all model
// faces surrounding it if it is on a model face, we compute the
// curvature at this location

static double LC_MVertex_CURV(GEntity *ge, double U, double V)
{
	double Crv = 0;
	switch(ge->dim()){
	case 0:
		Crv = max_edge_curvature((const GVertex *)ge);
		//Crv = std::max(max_surf_curvature_vertex((const GVertex *)ge), Crv);
		// Crv = max_surf_curvature((const GVertex *)ge);
		break;
	case 1:
		{
			//GEdge *ged = (GEdge *)ge;                      //暂时未添加GFACE，所以用不上
			//Crv = ged->curvature(U);
			//Crv = std::max(Crv, max_surf_curvature(ged, U));
			//// Crv = max_surf_curvature(ged, U);
		}
		break;
	case 2:
		{
			//GFace *gf = (GFace *)ge;
			//Crv = gf->curvature(SPoint2(U, V));
		}
		break;
	}
	double lc = Crv > 0 ? 2 * M_PI / Crv / CTX::instance()->mesh.minCircPoints : MAX_LC;
	return lc;
}



// compute the mesh size at a given vertex due to prescribed sizes at
// mesh vertices
static double LC_MVertex_PNTS(GEntity *ge, double U, double V)
{
	switch(ge->dim()){
	case 0:
		{
			GVertex *gv = (GVertex *)ge;
			double lc = gv->prescribedMeshSizeAtVertex();
			// FIXME we might want to remove this to make all lc treatment consistent
			if(lc >= MAX_LC) return CTX::instance()->lc / 10.;
			return lc;
		}
	case 1:
		{
			GEdge *ged = (GEdge *)ge;
			GVertex *v1 = ged->getBeginVertex();
			GVertex *v2 = ged->getEndVertex();
			if (v1 && v2){
				Range<double> range = ged->parBounds(0);
				double a = (U - range.low()) / (range.high() - range.low());
				double lc = (1 - a) * v1->prescribedMeshSizeAtVertex() +
					(a) * v2->prescribedMeshSizeAtVertex() ;
				// FIXME we might want to remove this to make all lc treatment consistent
				if(lc >= MAX_LC) return CTX::instance()->lc / 10.;
				return lc;
			}
			else
				return MAX_LC;
		}
	default:
		return MAX_LC;
	}
}

// This is the only function that is used by the meshers
double BGM_MeshSize(GEntity *ge, double U, double V,
	double X, double Y, double Z)
{
	// default lc (mesh size == size of the model)
	double l1 = CTX::instance()->lc;

	// lc from points
	double l2 = MAX_LC;
	if(CTX::instance()->mesh.lcFromPoints && ge->dim() < 2)
		l2 = LC_MVertex_PNTS(ge, U, V);

	// lc from curvature
	double l3 = MAX_LC;
	if(CTX::instance()->mesh.lcFromCurvature && ge->dim() < 3)
		l3 = LC_MVertex_CURV(ge, U, V);

	//// lc from fields
	double l4 = MAX_LC;
	//FieldManager *fields = ge->model()->getFields();
	//if(fields->getBackgroundField() > 0){
	//	Field *f = fields->get(fields->getBackgroundField());
	//	if(f) l4 = (*f)(X, Y, Z, ge);
	//}

	// take the minimum, then constrain by lcMin and lcMax
	double lc = std::min(std::min(std::min(l1, l2), l3), l4);
	lc = std::max(lc, CTX::instance()->mesh.lcMin);
	lc = std::min(lc, CTX::instance()->mesh.lcMax);

	if(lc <= 0.){
		printf("Wrong mesh element size lc = %g (lcmin = %g, lcmax = %g)",
			lc, CTX::instance()->mesh.lcMin, CTX::instance()->mesh.lcMax);
		lc = l1;
	}

	//Msg::Debug("BGM X,Y,Z=%g,%g,%g L4=%g L3=%g L2=%g L1=%g LC=%g LFINAL=%g DIM =%d ",
	//X, Y, Z, l4, l3, l2, l1, lc, lc * CTX::instance()->mesh.lcFactor, ge->dim());

	//Emi fix
	//if (lc == l1) lc /= 10.;

	return lc * CTX::instance()->mesh.lcFactor;
}
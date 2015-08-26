#ifndef _GMSH_EDGE_H_
#define _GMSH_EDGE_H_

#include "GEdge.h"
#include "Geo.h"

class gmshEdge : public GEdge {
protected:
	Curve *c; 

public:
	gmshEdge(GModel *model, Curve *edge, GVertex *v1, GVertex *v2);
	virtual ~gmshEdge() {}
	virtual Range<double> parBounds(int i) const;
	virtual GeomType geomType() const;
	virtual GPoint point(double p) const;
	virtual SVector3 firstDer(double par) const;
	virtual SVector3 secondDer(double par) const;
//	ModelType getNativeType() const { return GmshModel; }
	void * getNativePtr() const { return c; }
	virtual std::string getAdditionalInfoString();
	virtual int minimumMeshSegments() const;
	virtual int minimumDrawSegments() const;
	virtual void resetMeshAttributes();
//	virtual SPoint2 reparamOnFace(const GFace *face, double epar, int dir) const;
//	virtual void writeGEO(FILE *fp);
};

#endif

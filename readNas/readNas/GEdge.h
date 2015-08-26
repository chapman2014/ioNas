#ifndef _GEDGE_H_
#define _GEDGE_H_


#include "GEntity.h"
#include <list>
#include <string>
#include <vector>
#include <set>
#include <stdio.h>
#include "Range.h"
#include "GVertex.h"
#include "SVector3.h"


#ifndef GPointH
#include "GPoint.h"
#define GPointH 1
#endif

class MLine;
class MVertex;

// A model edge.
class GEdge :public GEntity{
private:
	double _length;
	bool _tooSmall;
protected:
	//GVertex *v0, *v1;
	GVertex *v0,*v1;
public:
	GEdge(GModel *model, int tag,GVertex* _v0,GVertex*_v1) : 
	  GEntity(model, tag), _tooSmall(false), v0(_v0), v1(_v1){}

   // get the start/end vertices of the edge
    GVertex *getBeginVertex() const { return v0; }
    GVertex *getEndVertex() const { return v1; }
	// get the point for the given parameter location
	virtual GPoint point(double p) const = 0;

	struct {
		mutable GEntity::MeshGenerationStatus status;
	} meshStatistics;

	// get bounds of parametric coordinate
	virtual Range<double> parBounds(int i) const = 0;
	inline double getLowerBound() const{ return parBounds(0).low();};
	inline double getUpperBound() const{ return parBounds(0).high();};


	//一个edge包含多个line
	std::vector<MLine*> lines;
	void addLine(MLine *line){ lines.push_back(line); }

	// get first derivative of edge at the given parameter
	virtual SVector3 firstDer(double par) const = 0;

	// get second derivative of edge at the given parameter (default
	// implentation using central differences)
	virtual SVector3 secondDer(double par) const;


	// get the dimension of the edge (1)
	virtual int dim() const { return 1; }

	// get the curvature
	virtual double curvature(double par) const;
	// true if start == end and no more than 2 segments
	void setTooSmall(bool b) { _tooSmall = b; }

	inline void setLength(const double l) { _length = l; }

	struct {
		char method;
		double coeffTransfinite;
		double meshSize;
		int nbPointsTransfinite;
		int typeTransfinite;
		int minimumMeshSegments;
		// the extrusion parameters (if any)
	//	ExtrudeParams *extrude;
		// reverse mesh orientation
		bool reverseMesh;
	} meshAttributes ;

};




#endif

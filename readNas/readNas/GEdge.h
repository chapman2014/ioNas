#ifndef _GEDGE_H_
#define _GEDGE_H_

#include <list>
#include <string>
#include <vector>
#include <set>
#include <stdio.h>
#include "Range.h"

class MLine;
class MVertex;
class GEdge :public GEntity{

private:
	double _length;
	bool _tooSmall;
protected:
	//GVertex *v0, *v1;
	MVertex* v0,*v1;
public:
	GEdge(MVertex* _v0,MVertex*_v1)
	{
	  v0=_v0;
	  v1=_v1;
	}

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
};




#endif

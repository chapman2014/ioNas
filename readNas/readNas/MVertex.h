#pragma once
#include <stdio.h>
#include <set>
#include <map>
class GEntity;
class GEdge;

class MVertex{
public:
	// the immutable id number of the vertex (this number is unique and
	// is guaranteed never to change once a vertex has been created)
	int _num;
	// a vertex index, used for example during mesh generation or when
	// saving a mesh (this index is not necessarily unique, can change
	// after mesh renumbering, etc.). By convention, vertices with
	// negative indices are not saved
	int _index;
	// a visibility and polynomial order flags
	char _visible, _order;
	// the cartesian coordinates of the vertex
	double _x, _y, _z;

	// the geometrical entity the vertex is associated with
	GEntity *_ge;


    MVertex(double x, double y, double z£¬GEntity *ge=0,int index)£º_ge(ge)
	{
		_x=x;
		_y=y;
		_z=z;
		_index=index;
	}
	// get/set the coordinates
	inline double x() const { return _x; }
	inline double y() const { return _y; }
	inline double z() const { return _z; }
	inline double & x() { return _x; }
	inline double & y() { return _y; }
	inline double & z() { return _z; }
	virtual ~MVertex(){}


};

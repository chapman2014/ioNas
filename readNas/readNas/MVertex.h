#pragma once
#include <stdio.h>
#include <set>
#include <map>

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

    MVertex(double x, double y, double z,int num)
	{
		_x=x;
		_y=y;
		_z=z;
		_index=num;
	}
	virtual ~MVertex(){}


};

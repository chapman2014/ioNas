#ifndef _BACKGROUND_MESH_H_
#define _BACKGROUND_MESH_H_

#include <map>
#include <vector>
#define _USE_MATH_DEFINES 
#include <math.h>


//class GFace;
class GVertex;
class GEdge;
//class MElement;
class MVertex;
class GEntity;

struct crossField2d 
{
	double _angle;
	static void normalizeAngle (double &angle) {
		if (angle < 0) 
			while ( angle <  0 ) angle += (M_PI * .5);
		else if (angle >= M_PI* .5) 
			while ( angle >= M_PI * .5 ) angle -= (M_PI * .5);
	}
	crossField2d (MVertex*, GEdge*);
	crossField2d (double a) : _angle(a){}
	crossField2d & operator += ( const crossField2d & );
};


class backgroundMesh
{
	//MElementOctree *_octree;
	std::vector<MVertex*> _vertices;
	//std::vector<MElement*> _triangles;
	std::map<MVertex*,double> _sizes;  
	std::map<MVertex*,MVertex*> _3Dto2D;
	std::map<MVertex*,MVertex*> _2Dto3D;
	std::map<MVertex*,double> _distance;  
	std::map<MVertex*,double> _angles;  
	static backgroundMesh * _current;

};

double BGM_MeshSize(GEntity *ge, double U, double V, double X, double Y, double Z);

#endif

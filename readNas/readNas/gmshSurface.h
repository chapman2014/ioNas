#ifndef _GMSH_SURFACE_H_
#define _GMSH_SURFACE_H_

#include <math.h>
#include <map>
//#include "Pair.h"
#include "Range.h"
#include "SPoint2.h"
#include "SPoint3.h"
#include "SVector3.h"
#include "SBoundingBox3d.h"
//#include "Numeric.h"


class gmshSurface
{
protected:  
	static std::map<int, gmshSurface*> allGmshSurfaces;
public:
	//there are points define in this surface parameterization
	bool vertex_defined_on_surface;
	virtual ~gmshSurface(){}
	static void reset() 
	{
		std::map<int, gmshSurface*>::iterator it = allGmshSurfaces.begin();
		for (; it != allGmshSurfaces.end(); ++it){
			if(!it->second->vertex_defined_on_surface)
				delete it->second;
		}
		allGmshSurfaces.clear();
	};
	static gmshSurface* getSurface(int tag);
	virtual Range<double> parBounds(int i) const = 0;
	/// Underlying geometric representation of this entity.
	enum gmshSurfaceType {
		Plane,
		Nurbs,
		Cylinder,
		Sphere,
		Cone,
		Torus,
		ParametricSurface,
		PolarSphere
	};
    
	virtual gmshSurface::gmshSurfaceType geomType() const = 0;
	virtual SPoint3 point(double par1, double par2) const = 0;
	virtual SPoint3 point(const SPoint2 &p) const { return point(p.x(), p.y()); }
	virtual SPoint2 parFromPoint(double x, double y, double z);
	// Return the normal to the face at the given parameter location.
	virtual SVector3 normal(const SPoint2 &param) const;
	// Return the first derivate of the face at the parameter location.
 // virtual Pair<SVector3,SVector3> firstDer(const SPoint2 &param);
	virtual double getMetricEigenvalue(const SPoint2 &);
};

#endif

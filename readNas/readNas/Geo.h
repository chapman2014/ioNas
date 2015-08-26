#pragma once
#include <math.h>
#include <vector>
#include "SPoint2.h"
#include "ListUtils.h"
#include "gmshSurface.h"

#define MSH_POINT              100
#define MSH_POINT_BND_LAYER    101
#define MSH_POINT_DISCRETE     102
#define MSH_POINT_FROM_GMODEL  103

#define MSH_SEGM_LINE          200
#define MSH_SEGM_SPLN          201
#define MSH_SEGM_CIRC          202
#define MSH_SEGM_CIRC_INV      203
#define MSH_SEGM_ELLI          204
#define MSH_SEGM_ELLI_INV      205
#define MSH_SEGM_LOOP          206
#define MSH_SEGM_BSPLN         207
#define MSH_SEGM_NURBS         208
#define MSH_SEGM_BEZIER        209
#define MSH_SEGM_BND_LAYER     211
#define MSH_SEGM_DISCRETE      212
#define MSH_SEGM_FROM_GMODEL   213
#define MSH_SEGM_COMPOUND      214

#define MSH_SURF_PLAN          300
#define MSH_SURF_REGL          301
#define MSH_SURF_TRIC          302
#define MSH_SURF_BND_LAYER     303
#define MSH_SURF_LOOP          304
#define MSH_SURF_DISCRETE      305
#define MSH_SURF_FROM_GMODEL   306
#define MSH_SURF_COMPOUND      307

#define MSH_VOLUME             400
#define MSH_VOLUME_DISCRETE    401
#define MSH_VOLUME_FROM_GMODEL 402
#define MSH_VOLUME_COMPOUND    403

#define MSH_PHYSICAL_POINT     500
#define MSH_PHYSICAL_LINE      501
#define MSH_PHYSICAL_SURFACE   502
#define MSH_PHYSICAL_VOLUME    503

struct Coord{
	double X, Y, Z;
};

class Vertex {
public :
	int Num;
	int Typ;
	char Visible;
	double lc, u, w;
	Coord Pos;
	// a model vertex is usually defined in the euclidian coordinates
	// (Pos). Yet, it can also be defined in the parametric coordinates
	// of a surface: pntOnGeometry stores the local coodinates of the
	// vertex in the gmshSurface it belongs to.
//	gmshSurface *geometry;
	SPoint2  pntOnGeometry;
	int boundaryLayerIndex;
	Vertex(double X=0., double Y=0., double Z=0., double l=1., double W=1.)
		: Num(0), Visible(1), lc(l), u(0.), w(W), boundaryLayerIndex(0)
	{
		Typ = MSH_POINT;
		Pos.X = X;
		Pos.Y = Y;
		Pos.Z = Z;
	}
	void norme()
	{
		double d = sqrt(Pos.X * Pos.X + Pos.Y * Pos.Y + Pos.Z * Pos.Z);
		if(d == 0.0) return;
		Pos.X /= d;
		Pos.Y /= d;
		Pos.Z /= d;
	}
	Vertex operator % (Vertex & autre) // cross product
	{
		return Vertex(Pos.Y * autre.Pos.Z - Pos.Z * autre.Pos.Y,
			-(Pos.X * autre.Pos.Z - Pos.Z * autre.Pos.X),
			Pos.X * autre.Pos.Y - Pos.Y * autre.Pos.X, lc, w);
	}
	void SetVisible(int value, bool recursive)
	{
		Visible = value;
	}
};

class DrawingColor{
public:
	int type;
	unsigned int geom, mesh;
};

class CircParam{
public:
	double t1, t2, f1, f2, incl;
	double invmat[3][3];
	double n[3];
};



class Curve{
public:
	int Num;
	int Typ;
	bool degenerated;
	char Visible;
	int Method;
	int nbPointsTransfinite;
	int typeTransfinite;
	double coeffTransfinite;
	double l;
	double mat[4][4];
	Vertex *beg, *end;
	double ubeg, uend;
    List_T *Control_Points;
//	ExtrudeParams *Extrude;
	float *k;
	int degre;
	CircParam Circle;
	DrawingColor Color;
	gmshSurface *geometry;
	std::vector<int> compound;
	int ReverseMesh;
	void SetVisible(int value, bool recursive)
	{
		Visible = value;
		if(recursive){
			if(beg) beg->SetVisible(value, recursive);
			if(end) end->SetVisible(value, recursive);
			for(int i = 0; i < List_Nbr(Control_Points); i++){
				Vertex *pV;
				List_Read(Control_Points, i, &pV);
				pV->SetVisible(value, recursive);
			}
		}
	}
	void SetColor(unsigned int value, bool recursive)
	{
		Color.type = 1;
		Color.geom = Color.mesh = value;
	}
};
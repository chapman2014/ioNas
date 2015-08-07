
#ifndef _DIVIDE_AND_CONQUER_H_
#define _DIVIDE_AND_CONQUER_H_

#include <vector>
#include <algorithm>
typedef struct _CDLIST DListRecord, *DListPeek;
typedef int PointNumero;

typedef struct{
	double v;
	double h;
}DPoint;

struct PointRecord {
	DPoint where;
	int id;//this id added by ligen
	DListPeek adjacent;
	void *data;
	int flag; //0:to be kept, 1:to be removed
	int identificator;
	std::vector<void*> vicinity;
	PointRecord() : adjacent(0), data (0) {}
};
struct _CDLIST{
	PointNumero point_num; 
	DListPeek next, prev;    //Á´±í 
};

typedef struct{
	PointNumero *t;
	int t_length;
}STriangle;

typedef struct{
	PointNumero begin;
	PointNumero end;
}DT;

typedef struct{
	PointNumero from;
	PointNumero to;
}Segment;

typedef struct{
	PointNumero a, b, c;
}Triangle;

class DocRecord{
	private:
		int _hullSize;
		PointNumero *_hull;
		PointNumero Predecessor(PointNumero a, PointNumero b);
		PointNumero Successor(PointNumero a, PointNumero b);
		int FixFirst(PointNumero x, PointNumero f);
		PointNumero First(PointNumero x);
		int IsLeftOf(PointNumero x, PointNumero y, PointNumero check);
		int IsRightOf(PointNumero x, PointNumero y, PointNumero check);
		Segment LowerCommonTangent(DT vl, DT vr);
		Segment UpperCommonTangent(DT vl, DT vr);
		int Qtest(PointNumero h, PointNumero i, PointNumero j, PointNumero k);
		int Merge(DT vl, DT vr);
		DT RecurTrig(PointNumero left, PointNumero right);
		int DListInsert(DListRecord **dlist, DPoint center, PointNumero newPoint);
		int Insert(PointNumero a, PointNumero b);
		int DListDelete(DListPeek *dlist, PointNumero oldPoint);
		int Delete(PointNumero a, PointNumero b);
		void ConvertDListToVoronoiData();
		int ConvertDListToTriangles();
		void RemoveAllDList();
		int BuildDelaunay();
		int CountPointsOnHull();
		void ConvexHull();

public:
	STriangle *_adjacencies;
	int numPoints;        // number of points
	int size_points;
	PointRecord *points;  // points to triangulate
	int numTriangles;     // number of triangles
	Triangle *triangles;  // 2D results
	DocRecord(int n);
	double &x(int i){ return points[i].where.h; } 
	double &y(int i){ return points[i].where.v; } 
	void*  &data(int i){ return points[i].data; } 
   // void setPoints(fullMatrix<double> *p);
	~DocRecord();
	void MakeMeshWithPoints();
	void Voronoi ();
	int hullSize() {return _hullSize;}

	PointNumero *ConvertDlistToArray(DListPeek *dlist, int *n);
};



















#endif
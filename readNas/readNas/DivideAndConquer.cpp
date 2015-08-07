
#include "StdAfx.h"
#define _USE_MATH_DEFINES
#include  <math.h>

#include "DivideAndConquer.h"
#include "robustPredicates.h"




#define Pred(x) ((x)->prev)
#define Succ(x) ((x)->next)

// 通过a的之前邻居来找b的第一个邻居的num号
PointNumero DocRecord::Predecessor(PointNumero a, PointNumero b)
{
	DListPeek p = points[a].adjacent;
	if(p == NULL)
		return -1;

	do {
		if(p->point_num == b)
			return Pred(p)->point_num;
		p = Pred(p);
	} while(p != points[a].adjacent);

	return -1;
}

// 通过a的之后邻居来找b的第一个邻居的num号
PointNumero DocRecord::Successor(PointNumero a, PointNumero b)
{
	DListPeek p = points[a].adjacent;
	if(p == NULL)
		return -1;

	do {
		if(p->point_num == b)
			return Succ(p)->point_num;
		p = Succ(p);
	} while(p != points[a].adjacent);

	return -1;
}

int DocRecord::FixFirst(PointNumero x, PointNumero f)
{
	DListPeek p = points[x].adjacent;
	if(p == NULL)
		return 0;

	int out = 0;
	DListPeek copy = p;
	do {
		if(p->point_num == f) {           //邻接链表遍历到了f
			points[x].adjacent = p;        //使邻接表的第一个顶点是f
			out = 1;
		}
		else
			p = p->next;
	} while((p != copy) && !out);
	return out;
}


PointNumero DocRecord::First(PointNumero x)
{
	return (points[x].adjacent)->point_num;
}

int DocRecord::IsLeftOf(PointNumero x, PointNumero y, PointNumero check)
{
	double pa[2] = {(double)points[x].where.h, (double)points[x].where.v};
	double pb[2] = {(double)points[y].where.h, (double)points[y].where.v};
	double pc[2] = {(double)points[check].where.h, (double)points[check].where.v};

	// we use robust predicates here
	double result = robustPredicates::orient2d(pa, pb, pc);

	return result > 0;
}


int DocRecord::IsRightOf(PointNumero x, PointNumero y, PointNumero check)
{
	return IsLeftOf(y, x, check);
}

Segment DocRecord::LowerCommonTangent(DT vl, DT vr)
{
	PointNumero x, y, z, z1, z2, temp;
	Segment s;

	x = vl.end;   // 
	y = vr.begin; // 
	z = First(y);
	z1 = First(x);
	//返回x邻居z1（如果存在的话）的邻居
	z2 = Predecessor(x, z1);//在X的邻接链表里，，向前找 ，找到z1前一位
	for(;;) {
		if(IsRightOf(x, y, z)) {
			temp = z;
			z = Successor(z, y);
			y = temp;
		}
		else if(IsRightOf(x, y, z2)) {
			temp = z2;
			z2 = Predecessor(z2, x);
			x = temp;
		}
		else {
			s.from = x;
			s.to = y;
			return s;
		}
	}
}

Segment DocRecord::UpperCommonTangent(DT vl, DT vr)
{
	PointNumero x, y, z, z1, z2, temp;
	Segment s;

	x = vl.end;   // vu le tri, c'est le point le + a droite
	y = vr.begin; // idem, le + a gauche
	z = First(y);
	z1 = First(x);
	z2 = Predecessor(y, z);
	for(;;) {
		if(IsLeftOf(x, y, z2)) {
			temp = z2;
			z2 = Predecessor(z2, y);
			y = temp;
		}
		else if(IsLeftOf(x, y, z1)) {
			temp = z1;
			z1 = Successor(z1, x);
			x = temp;
		}
		else {
			s.from = x;
			s.to = y;
			return s;
		}
	}
}

// return 1 if the point k is NOT in the circumcircle of triangle  hij
int DocRecord::Qtest(PointNumero h, PointNumero i, PointNumero j, PointNumero k)
{
	if((h == i) && (h == j) && (h == k)) {
		printf("Identical points in triangulation: increase element size or Mesh.RandomFactor\n");
		return 0;
	}

	double pa[2] = {(double)points[h].where.h, (double)points[h].where.v};
	double pb[2] = {(double)points[i].where.h, (double)points[i].where.v};
	double pc[2] = {(double)points[j].where.h, (double)points[j].where.v};
	double pd[2] = {(double)points[k].where.h, (double)points[k].where.v};

	// we use robust predicates here
	double result = robustPredicates::incircle(pa, pb, pc, pd) *
		robustPredicates::orient2d(pa, pb, pc);

	return (result < 0) ? 1 : 0;
}

int DocRecord::Merge(DT vl, DT vr)
{
	Segment bt, ut;
	int a, b, out;
	PointNumero r, r1, r2, l, l1, l2;

	bt = LowerCommonTangent(vl, vr);
	ut = UpperCommonTangent(vl, vr);
	l = bt.from;  // left endpoint of BT
	r = bt.to;    // right endpoint of BT

	while((l != ut.from) || (r != ut.to)) {
		a = b = 0;
		if(!Insert(l, r))
			return 0;

		r1 = Predecessor(r, l);//候选点R1
		if(r1 == -1)
			return 0;
		if(IsRightOf(l, r, r1)) //已经到达最上的点
			a = 1;                //a 
		else {
			out = 0;
			while(!out) {              //开始寻找最终点 
				r2 = Predecessor(r, r1);
				if(r2 == -1)
					return 0;
				if(r2 < vr.begin)        //如果r2????
					out = 1;
				else if(Qtest(l, r, r1, r2))
					out = 1;              //如果r2在外接圆外,最终候选点为r1，跳出循环
				else {
					if(!Delete(r, r1))    //如果在圆内，删除R R1边
						return 0;
					r1 = r2;                           //r2成为新的R1
					if(IsRightOf(l, r, r1))           //如果新的R1已经过了最高点
						out = a = 1;        //                    //最终候选点是r1，并且a=1，跳出循环
					//没过最高点，则out=0，继续寻找
				}
			}
		}

		l1 = Successor(l, r);
		if(l1 == -1)
			return 0;
		if(IsLeftOf(r, l, l1))
			b = 1;
		else {
			out = 0;
			while(!out) {
				l2 = Successor(l, l1);
				if(l2 == -1)
					return 0;
				if(l2 > vl.end)
					out = 1;
				else if(Qtest(r, l, l1, l2))
					out = 1;
				else {
					if(!Delete(l, l1))
						return 0;
					l1 = l2;
					if(IsLeftOf(r, l, l1))
						out = b = 1;
				}
			}
		}

		if(a)
			l = l1;
		else if(b)
			r = r1;
		else {
			if(Qtest(l, r, r1, l1))  //如果l1在外接圆外
				r = r1;
			else
				l = l1;
		}
	}//直到上公切线




	if(!Insert(l, r))
		return 0;

	if(!FixFirst(ut.to, ut.from))         //????
		return 0;
	if(!FixFirst(bt.from, bt.to))
		return 0;
	return 1;
}

DT DocRecord::RecurTrig(PointNumero left, PointNumero right)
{
	int n, m;
	DT dt;

	dt.begin = left;
	dt.end = right;
	n = right - left + 1; // nombre de points a triangulariser
	switch (n) {
	case 0:
	case 1:
		// 0 ou 1 points -> rien a faire
		break;

	case 2: // deux points : cas trivial
		Insert(left, right);
		FixFirst(left, right);
		FixFirst(right, left);
		break;

	case 3: // trois points : cas trivial
		Insert(left, right);
		Insert(left, left + 1);
		Insert(left + 1, right);
		if(IsRightOf(left, right, left + 1)) {//逆时针顺序？？？
			FixFirst(left, left + 1);
			FixFirst(left + 1, right);
			FixFirst(right, left);
		}
		else {
			FixFirst(left, right);
			FixFirst(left + 1, left);
			FixFirst(right, left + 1);
		}
		break;

	default: // plus de trois points : cas recursif
		m = (left + right) >> 1;
		if(!Merge(RecurTrig(left, m), RecurTrig(m + 1, right)))
			break;
	}
	return dt;
}

static int comparePoints(const void *i, const void *j)
{
	double x, y;

	x = ((PointRecord *) i)->where.h - ((PointRecord *) j)->where.h;
	if(x == 0.) {
		y = ((PointRecord *) i)->where.v - ((PointRecord *) j)->where.v;
		return (y < 0.) ? -1 : 1;
	}
	else
		return (x < 0.) ? -1 : 1;
}

// this fonction builds the delaunay triangulation for a window
int DocRecord::BuildDelaunay()
{
	qsort(points, numPoints, sizeof(PointRecord), comparePoints);
	RecurTrig(0, numPoints - 1);
	return 1;
}

// This routine insert the point 'newPoint' in the list dlist,
// respecting the clock-wise orientation、
//往dlist里插入newPoint
int DocRecord::DListInsert(DListRecord **dlist, DPoint center, PointNumero newPoint)
{
	DListRecord *p, *newp;
	double alpha1, alpha, beta, xx, yy;
	int first;

	newp = new DListRecord;
	newp->point_num = newPoint;

	if(*dlist == NULL) {    //无结点
		*dlist = newp;
		Pred(*dlist) = newp;
		Succ(*dlist) = newp;
		return 1;
	}
	if(Succ(*dlist) == *dlist) {  //只有一个结点
		Pred(*dlist) = newp;
		Succ(*dlist) = newp;
		Pred(newp) = *dlist;
		Succ(newp) = *dlist;
		return 1;
	}

	// If we are here, the double-linked circular list has 2 or more
	// elements, so we have to calculate where to put the new one

	p = *dlist;
	first = p->point_num;

	// first, compute polar coord. of the first point
	yy = (double)(points[first].where.v - center.v);
	xx = (double)(points[first].where.h - center.h);
	alpha1 = atan2(yy, xx);

	// compute polar coord of the point to insert
	yy = (double)(points[newPoint].where.v - center.v);
	xx = (double)(points[newPoint].where.h - center.h);
	beta = atan2(yy, xx) - alpha1;
	if(beta <= 0)
		beta += 2. * M_PI;

	do {
		yy = (double)(points[Succ(p)->point_num].where.v - center.v);
		xx = (double)(points[Succ(p)->point_num].where.h - center.h);
		alpha = atan2(yy, xx) - alpha1;
		if(alpha <= 1.e-15)
			alpha += 2. * M_PI;
		if(alpha >= beta) {            //一直找到一个极角大于待插入点的极角的点
			Succ(newp) = Succ(p);        
			Succ(p) = newp;
			Pred(newp) = p;
			Pred(Succ(newp)) = newp;
			return 1;
		}
		p = Succ(p);
	} while(p != *dlist);

	// never here!
	return 0;
}

// This routine inserts the point 'a' in the adjency list of 'b' and
// the point 'b' in the adjency list of 'a'
int DocRecord::Insert(PointNumero a, PointNumero b)
{
	int rslt = DListInsert(&points[a].adjacent, points[a].where, b);
	rslt &= DListInsert(&points[b].adjacent, points[b].where, a);
	return rslt;
}

int DocRecord::DListDelete(DListPeek *dlist, PointNumero oldPoint)
{
	DListPeek p;

	if(*dlist == NULL)
		return 0;
	if(Succ(*dlist) == *dlist) {
		if((*dlist)->point_num == oldPoint) {
			delete *dlist;
			*dlist = NULL;
			return 1;
		}
		else
			return 0;
	}
	p = *dlist;
	do {
		if(p->point_num == oldPoint) {
			Succ(Pred(p)) = Succ(p);
			Pred(Succ(p)) = Pred(p);
			if(p == *dlist) {
				*dlist = Succ(p);
			}
			delete p;
			return 1;
		}
		p = Succ(p);
	} while(p != *dlist);

	return 0;
}

// This routine removes the point 'a' in the adjency list of 'b' and
// the point 'b' in the adjency list of 'a'
int DocRecord::Delete(PointNumero a, PointNumero b)
{
	int rslt = DListDelete(&points[a].adjacent, b);
	rslt &= DListDelete(&points[b].adjacent, a);
	return rslt;
}


// compte les points sur le polygone convexe  //the points on the convex polygon
int DocRecord::CountPointsOnHull()
{
	PointNumero p, p2, temp;
	int i, n=numPoints;

	if(points[0].adjacent == NULL)
		return 0;
	i = 1;
	p = 0;
	p2 = First(0);
	while((p2 != 0) && (i < n)) {
		i++;
		temp = p2;
		p2 = Successor(p2, p);
		p = temp;
	}
	return (i <= n) ? i : -1;
}

// compte les points sur le polygone convexe
void DocRecord::ConvexHull()
{
	PointNumero p, p2, temp;

	if(points[0].adjacent == NULL)
		return;
	int count = 0;
	p = 0;
	_hull[count++] = p;
	p2 = First(0);
	while((p2 != 0) && (count < numPoints)) {
		temp = p2;
		p2 = Successor(p2, p);
		p = temp;
		_hull[count++] = p;
	}
}

PointNumero *DocRecord::ConvertDlistToArray(DListPeek *dlist, int *n)
{
	DListPeek p, temp;
	int i, max = 0;
	PointNumero *ptr;

	p = *dlist;
	do {
		max++;        //统计点数
		p = Pred(p);
	} while(p != *dlist);
	ptr = new PointNumero[max + 1];
	if(ptr == NULL)
		return NULL;
	p = *dlist;
	for(i = 0; i < max; i++) {
		ptr[i] = p->point_num;
		temp = p;
		p = Pred(p);
		delete temp;
	}
	ptr[max] = ptr[0];
	*dlist = NULL;
	*n = max;
	return ptr;
}

//  Cette routine efface toutes les listes d'adjacence de points
//  this routine will erase all the adjacency lists of items
void DocRecord::RemoveAllDList()
{
	int i;
	DListPeek p, temp;

	for(i = 0; i < numPoints; i++)
		if(points[i].adjacent != NULL) {
			p = points[i].adjacent;
			do {
				temp = p;
				p = Pred(p);
				delete temp;
			} while(p != points[i].adjacent);
			points[i].adjacent = NULL;
		}
}

// Convertir les listes d'adjacence en triangles
int DocRecord::ConvertDListToTriangles()
{
	// on suppose que n >= 3. points est suppose OK.

	int n = numPoints, i, j;
	int count = 0, count2 = 0;
	PointNumero aa, bb, cc;

	STriangle *striangle = new STriangle[n];

	count2 = CountPointsOnHull();

	// nombre de triangles que l'on doit obtenir         
	//the number of triangles that we must achieve
	count2 = 2 * (n - 1) - count2;

	// FIXME ::: WHY 2 * ???????????????????
	triangles = new Triangle[2 * count2];

	for(i = 0; i < n; i++) {
		// on cree une liste de points connectes au point i (t) + nombre de points (t_length)
		// 
		//we need to create a list of items (i (t) + number points (t _ length)
		striangle[i].t = ConvertDlistToArray(&points[i].adjacent,
			&striangle[i].t_length);
	}

	// on balaye les noeuds de gauche a droite -> on cree les triangles
	//we scan the nodes from left to right: we create the triangles
	count = 0;
	for(i = 0; i < n; i++) {
		for(j = 0; j < striangle[i].t_length; j++) {
			if((striangle[i].t[j] > i) && (striangle[i].t[j + 1] > i) &&
				(IsRightOf(i, striangle[i].t[j], striangle[i].t[j + 1]))) {
					aa = i;
					bb = striangle[i].t[j];
					cc = striangle[i].t[j + 1];
					triangles[count].a = aa;
					triangles[count].b = bb;
					triangles[count].c = cc;
					count++;
			}
		}
	}
	numTriangles = count2;

	for(int i = 0; i < n; i++)
		delete [] striangle[i].t;
	delete [] striangle;

	return 1;
}
DocRecord::DocRecord(int n)
	: _hullSize(0), _hull(NULL), _adjacencies(NULL),
	numPoints(n), points(NULL), numTriangles(0), triangles(NULL)
{
	if(numPoints)
		points = new PointRecord[numPoints+3000];
}

DocRecord::~DocRecord()
{
	if(points) delete [] points;
	if(triangles) delete [] triangles;
	if(_hull) delete [] _hull;
	if (_adjacencies){
		for(int i = 0; i < numPoints; i++)
			delete [] _adjacencies[i].t;
		delete _adjacencies;
	}
}

void DocRecord::MakeMeshWithPoints()
{
	if(numPoints < 3) return;
	BuildDelaunay();
	ConvertDListToTriangles();
	RemoveAllDList();
}
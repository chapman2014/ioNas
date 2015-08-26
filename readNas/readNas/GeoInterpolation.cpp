// Gmsh - Copyright (C) 1997-2014 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#include "stdafx.h"
#include "GeoInterpolation.h"
#include "Context.h"

#define SQU(a)      ((a)*(a))

static Vertex InterpolateCubicSpline(Vertex *v[4], double t, double mat[4][4],
                                     int derivee, double t1, double t2)
{
  Vertex V;
  int i, j;
  double vec[4], T[4];

  V.Pos.X = V.Pos.Y = V.Pos.Z = 0.0;
  V.lc = (1 - t) * v[1]->lc + t * v[2]->lc;
  V.w = (1 - t) * v[1]->w + t * v[2]->w;

  if(derivee == 1) {
    T[3] = 0.;
    T[2] = 1.;
    T[1] = 2. * t;
    T[0] = 3. * t * t;
  }
  else if(derivee == 2) {
    T[3] = 0.;
    T[2] = 0.;
    T[1] = 2.;
    T[0] = 6. * t;
  }
  else {
    T[3] = 1.;
    T[2] = t;
    T[1] = t * t;
    T[0] = t * t * t;
  }

  for(i = 0; i < 4; i++) {
    vec[i] = 0.0;
  }

  // X
  for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
      vec[i] += mat[i][j] * v[j]->Pos.X;
    }
  }

  for(j = 0; j < 4; j++) {
    V.Pos.X += T[j] * vec[j];
    vec[j] = 0.0;
  }

  // Y
  for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
      vec[i] += mat[i][j] * v[j]->Pos.Y;
    }
  }

  for(j = 0; j < 4; j++) {
    V.Pos.Y += T[j] * vec[j];
    vec[j] = 0.0;
  }

  // Z
  for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
      vec[i] += mat[i][j] * v[j]->Pos.Z;
    }
  }
  for(j = 0; j < 4; j++) {
    V.Pos.Z += T[j] * vec[j];
    vec[j] = 0.0;
  }

  if(derivee == 1) {
    V.Pos.X /= ((t2 - t1));
    V.Pos.Y /= ((t2 - t1));
    V.Pos.Z /= ((t2 - t1));
  }
  else if(derivee == 2) {
    V.Pos.X /= ((t2 - t1)*(t2 - t1));
    V.Pos.Y /= ((t2 - t1)*(t2 - t1));
    V.Pos.Z /= ((t2 - t1)*(t2 - t1));
  }

  return V;
}
// Non Uniform BSplines
int findSpan(double u, int deg, int n, float *U)
{
  if(u >= U[n])
    return n - 1;
  if(u <= U[0])
    return deg;

  int low = deg;
  int high = n + 1;
  int mid = (low + high) / 2;

  while(u < U[mid] || u >= U[mid + 1]) {
    if(u < U[mid])
      high = mid;
    else
      low = mid;
    mid = (low + high) / 2;
  }
  return mid;
}

static void basisFuns(double u, int i, int deg, float *U, double *N)
{
  double left[1000];
  double *right = &left[deg + 1];

  double temp, saved;

  N[0] = 1.0;
  for(int j = 1; j <= deg; j++) {
    left[j] = u - U[i + 1 - j];
    right[j] = U[i + j] - u;
    saved = 0.0;
    for(int r = 0; r < j; r++) {
      temp = N[r] / (right[r + 1] + left[j - r]);
      N[r] = saved + right[r + 1] * temp;
      saved = left[j - r] * temp;
    }
    N[j] = saved;
  }
}

static Vertex InterpolateNurbs(Curve *Curve, double u, int derivee)
{
  double Nb[1000];
  int span = findSpan(u, Curve->degre, List_Nbr(Curve->Control_Points), Curve->k);
  basisFuns(u, span, Curve->degre, Curve->k, Nb);
  Vertex p;
  p.Pos.X = p.Pos.Y = p.Pos.Z = p.w = p.lc = 0.0;
  for(int i = Curve->degre; i >= 0; --i) {
    Vertex *v;
    List_Read(Curve->Control_Points, span - Curve->degre + i, &v);
    p.Pos.X += Nb[i] * v->Pos.X;
    p.Pos.Y += Nb[i] * v->Pos.Y;
    p.Pos.Z += Nb[i] * v->Pos.Z;
    p.w += Nb[i] * v->w;
    p.lc += Nb[i] * v->lc;
  }
  return p;
}

Vertex InterpolateCurve(Curve *c, double u, int derivee)
{
 /* if(c->Num < 0) {
    Curve *C0 = FindCurve(-c->Num);
    if(!C0){
      Msg::Error("Unknown curve %d", -c->Num);
      return Vertex(0., 0., 0.);
    }
    return InterpolateCurve(C0, C0->ubeg + (C0->uend - C0->ubeg) * (1. - u), derivee);
  }*/

  Vertex V;

  if(derivee==1) {
      double eps1 = (u == 0) ? 0 : 1.e-5;
      double eps2 = (u == 1) ? 0 : 1.e-5;
      Vertex D[2];
      D[0] = InterpolateCurve(c, u - eps1, 0);//返回一个点
      D[1] = InterpolateCurve(c, u + eps2, 0);
      V.Pos.X = (D[1].Pos.X - D[0].Pos.X) / (eps1 + eps2);// 斜率
      V.Pos.Y = (D[1].Pos.Y - D[0].Pos.Y) / (eps1 + eps2);
      V.Pos.Z = (D[1].Pos.Z - D[0].Pos.Z) / (eps1 + eps2);
      V.u = u;
      return V;
  }

  if(derivee==2) {
    switch (c->Typ) {
 /*   case MSH_SEGM_BSPLN:
      V = InterpolateUBS(c, u, 2);
      V.u = u;
      break;
    case MSH_SEGM_BEZIER:
      V = InterpolateBezier(c, u, 2);
      V.u = u;
      break;*/
    default :
      double eps1 = (u == 0) ? 0 : 1.e-5;
      double eps2 = (u == 1) ? 0 : 1.e-5;
      Vertex D[2];
      D[0] = InterpolateCurve(c, u - eps1, 1);
      D[1] = InterpolateCurve(c, u + eps2, 1);
      V.Pos.X = (D[1].Pos.X - D[0].Pos.X) / (eps1 + eps2);
      V.Pos.Y = (D[1].Pos.Y - D[0].Pos.Y) / (eps1 + eps2);
      V.Pos.Z = (D[1].Pos.Z - D[0].Pos.Z) / (eps1 + eps2);
      V.u = u;
      break;
    }
    return V;
  }

  int N, i;
  Vertex *v[5];
  double theta, t1, t2, t;
  Vertex temp1, temp2;

  switch (c->Typ) {

  case MSH_SEGM_LINE:
#if defined(HAVE_BFGS)
    //    printf("MSH_SEGM_LINE\n");
#endif
    N = List_Nbr(c->Control_Points);
    i = (int)((double)(N - 1) * u);
    while(i >= N - 1)
      i--;
    while(i < 0)
      i++;
    t1 = (double)(i) / (double)(N - 1);
    t2 = (double)(i + 1) / (double)(N - 1);
    t = (u - t1) / (t2 - t1);
    List_Read(c->Control_Points, i, &v[1]);
    List_Read(c->Control_Points, i + 1, &v[2]);
    if(!c->geometry){
      V.Pos.X = v[1]->Pos.X + t * (v[2]->Pos.X - v[1]->Pos.X);
      V.Pos.Y = v[1]->Pos.Y + t * (v[2]->Pos.Y - v[1]->Pos.Y);
      V.Pos.Z = v[1]->Pos.Z + t * (v[2]->Pos.Z - v[1]->Pos.Z);
      V.w = (1. - t) * v[1]->w + t * v[2]->w;
      V.lc = (1. - t) * v[1]->lc + t * v[2]->lc;
    }
    else{
      SPoint2 p = v[1]->pntOnGeometry +  (v[2]->pntOnGeometry - v[1]->pntOnGeometry) * t;
      SPoint3 pp = c->geometry->point(p);
      V.Pos.X = pp.x();
      V.Pos.Y = pp.y();
      V.Pos.Z = pp.z();
    }
    break;
  default:
    printf("Unknown curve type in interpolation");
    break;
  }
  V.u = u;
  return V;
}

// Transfinite interpolation on a quadrangle :
// f(u,v) = (1-u)c4(v) + u c2(v) + (1-v)c1(u) + v c3(u)
//          - [ (1-u)(1-v)s1 + u(1-v)s2 + uv s3 + (1-u)v s4 ]

#define TRAN_QUA(c1,c2,c3,c4,s1,s2,s3,s4,u,v) \
   (1.-u)*c4+u*c2+(1.-v)*c1+v*c3-((1.-u)*(1.-v)*s1+u*(1.-v)*s2+u*v*s3+(1.-u)*v*s4)

static Vertex TransfiniteQua(Vertex c1, Vertex c2, Vertex c3, Vertex c4,
                             Vertex s1, Vertex s2, Vertex s3, Vertex s4,
                             double u, double v)
{
  Vertex V;

  V.lc = TRAN_QUA(c1.lc, c2.lc, c3.lc, c4.lc,
                  s1.lc, s2.lc, s3.lc, s4.lc, u, v);
  V.w = TRAN_QUA(c1.w, c2.w, c3.w, c4.w, s1.w, s2.w, s3.w, s4.w, u, v);
  V.Pos.X = TRAN_QUA(c1.Pos.X, c2.Pos.X, c3.Pos.X, c4.Pos.X,
                     s1.Pos.X, s2.Pos.X, s3.Pos.X, s4.Pos.X, u, v);
  V.Pos.Y = TRAN_QUA(c1.Pos.Y, c2.Pos.Y, c3.Pos.Y, c4.Pos.Y,
                     s1.Pos.Y, s2.Pos.Y, s3.Pos.Y, s4.Pos.Y, u, v);
  V.Pos.Z = TRAN_QUA(c1.Pos.Z, c2.Pos.Z, c3.Pos.Z, c4.Pos.Z,
                     s1.Pos.Z, s2.Pos.Z, s3.Pos.Z, s4.Pos.Z, u, v);
  return (V);
}

// Transfinite interpolation on a triangle :
//
//            s3(1,1)
//              +
//            / |
//      c3  /   |
//        /     |  c2
//      /       |
//    /   c1    |
//   +----------+
//  s1(0,0)     s2(1,0)

// Old-style: TRAN_QUA with s1=s4=c4
//
// f(u,v) = u c2 (v) + (1-v) c1(u) + v c3(u) - u(1-v) s2 - uv s3
//
// u = v = 0     -----> x = c1(0) = s1 --> OK
// u = 1 ; v = 0 -----> x = c2(0) + c1(1) - s2 =  s2 --> OK
// u = 1 ; v = 1 -----> x = c2(1) + c3(1) - s3 =  s3 --> OK
// v = 0 --> u s2 + c1(u) - u s2 --> x = c1(u) --> OK
// u = 1 --> c2(v) + (1-v) s2 + v s3 -(1-v) s2  - v s3 --> x = c2(v) --> OK
// u = 0 --> (1-v) s1 + v s1 = s1 !!! not terrible (singular)

#define TRAN_TRI(c1,c2,c3,s1,s2,s3,u,v) u*c2+(1.-v)*c1+v*c3-(u*(1.-v)*s2+u*v*s3);

static Vertex TransfiniteTri(Vertex c1, Vertex c2, Vertex c3,
                             Vertex s1, Vertex s2, Vertex s3, double u, double v)
{
  Vertex V;
  V.lc = TRAN_TRI(c1.lc, c2.lc, c3.lc, s1.lc, s2.lc, s3.lc, u, v);
  V.w = TRAN_TRI(c1.w, c2.w, c3.w, s1.w, s2.w, s3.w, u, v);
  V.Pos.X = TRAN_TRI(c1.Pos.X, c2.Pos.X, c3.Pos.X,
                     s1.Pos.X, s2.Pos.X, s3.Pos.X, u, v);
  V.Pos.Y = TRAN_TRI(c1.Pos.Y, c2.Pos.Y, c3.Pos.Y,
                     s1.Pos.Y, s2.Pos.Y, s3.Pos.Y, u, v);
  V.Pos.Z = TRAN_TRI(c1.Pos.Z, c2.Pos.Z, c3.Pos.Z,
                     s1.Pos.Z, s2.Pos.Z, s3.Pos.Z, u, v);
  return V;
}

// New-style:
//
// f(u,v) = (1-u) (c1(u-v) + c3(1-v)     - s1) +
//          (u-v) (c2(v)   + c1(u)       - s2) +
//            v   (c3(1-u) + c2(1-u+v)   - s3)
//
// u = v = 0 --> f(0,0) = s1 + s1 - s1     = s1
// u = v = 1 --> f(1,1) = s3 + s3 - s3     = s3
// u = 1 ; v = 0 --> f(1,1) = s2 + s2 - s2 = s2
// v = 0 --> (1-u)c1(u) + u c1(u) = c1(u) --> COOL

#define TRAN_TRIB(c1,c1b,c2,c2b,c3,c3b,s1,s2,s3,u,v)\
      (1.-u)*(c1+c3b-s1)+(u-v)*(c2+c1b-s2)+v*(c3+c2b-s3);

static Vertex TransfiniteTriB(Vertex c1, Vertex c1b, Vertex c2,
			      Vertex c2b, Vertex c3, Vertex c3b,
			      Vertex s1, Vertex s2, Vertex s3,
			      double u, double v)
{
  Vertex V;
  V.lc = TRAN_TRIB(c1.lc,c1b.lc, c2.lc,c2b.lc, c3.lc, c3b.lc, s1.lc, s2.lc, s3.lc, u, v);
  V.w = TRAN_TRIB(c1.w, c1b.w, c2.w,c2b.w, c3.w,c3b.w, s1.w, s2.w, s3.w, u, v);
  V.Pos.X = TRAN_TRIB(c1.Pos.X, c1b.Pos.X,c2.Pos.X, c2b.Pos.X,c3.Pos.X,c3b.Pos.X,
                     s1.Pos.X, s2.Pos.X, s3.Pos.X, u, v);
  V.Pos.Y = TRAN_TRIB(c1.Pos.Y, c1b.Pos.Y,c2.Pos.Y, c2b.Pos.Y,c3.Pos.Y,c3b.Pos.Y,
                     s1.Pos.Y, s2.Pos.Y, s3.Pos.Y, u, v);
  V.Pos.Z = TRAN_TRIB(c1.Pos.Z, c1b.Pos.Z,c2.Pos.Z, c2b.Pos.Z,c3.Pos.Z,c3b.Pos.Z,
                     s1.Pos.Z, s2.Pos.Z, s3.Pos.Z, u, v);
  return V;
}

static void TransfiniteSph(Vertex S, Vertex center, Vertex *T)
{
  double r = sqrt(SQU(S.Pos.X - center.Pos.X) + SQU(S.Pos.Y - center.Pos.Y)
                  + SQU(S.Pos.Z - center.Pos.Z));

  double s = sqrt(SQU(T->Pos.X - center.Pos.X) + SQU(T->Pos.Y - center.Pos.Y)
                  + SQU(T->Pos.Z - center.Pos.Z));

  double dirx = (T->Pos.X - center.Pos.X) / s;
  double diry = (T->Pos.Y - center.Pos.Y) / s;
  double dirz = (T->Pos.Z - center.Pos.Z) / s;

  T->Pos.X = center.Pos.X + r * dirx;
  T->Pos.Y = center.Pos.Y + r * diry;
  T->Pos.Z = center.Pos.Z + r * dirz;
}

#include "StdAfx.h"
#include "MVertex.h"
#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;
class MVertex;
class MTriangle
{
   public:
	int _num;
	MVertex *_v0;
	MVertex *_v1;
	MVertex *_v2;
	

	/*MTriangle(MVertex *v0, MVertex *v1, MVertex *v2)
	{
	 _v0=v0;
	 _v1=v1;
	 _v2=v2;
	}*/
	MTriangle(const vector<MVertex*> &v,int num)
		
	{
		_v0=v[0];
		_v1=v[1];
		_v2=v[2];
		_num= num;
	}
	 ~MTriangle(){}
};
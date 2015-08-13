#ifndef _MLINE_H_
#define _MLINE_H_
#include "MVertex.h"
#include <vector>

class MLine{

protected:
	MVertex *_v[2];
	int num,part;
	void _getEdgeVertices(std::vector<MVertex*> &v) const
	{
		v[0] = _v[0];
		v[1] = _v[1];
	}
public :
	MLine(MVertex *v0, MVertex *v1, int num1=0, int part1=0)
		
	{
		_v[0] = v0; _v[1] = v1;
		num=num1;
		part=part1;
	}
	MLine(const std::vector<MVertex*> &v, int num1=0, int part1=0)
	{
		for(int i = 0; i < 2; i++) _v[i] = v[i];
		num=num1;
		part=part1;
	}



};






#endif
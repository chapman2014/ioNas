#ifndef _GEDGE_H_
#define _GEDGE_H_

#include <list>
#include <string>
#include <vector>
#include <set>
#include <stdio.h>


class MLine;
class MVertex;
class GEdge{

private:
	double _length;
	bool _tooSmall;
protected:
	//GVertex *v0, *v1;
	MVertex* v0,*v1;
public:
	GEdge(MVertex* _v0,MVertex*_v1)
	{
	  v0=_v0;
	  v1=_v1;
	}


	struct {
		mutable GEntity::MeshGenerationStatus status;
	} meshStatistics;
	//一个edge包含多个line
	std::vector<MLine*> lines;
	void addLine(MLine *line){ lines.push_back(line); }
};




#endif

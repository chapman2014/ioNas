#include "StdAfx.h"
#include <sstream>
#include <algorithm>
#include "GModel.h"
#include "GVertex.h"

//#include "MPoint.h"

GVertex::GVertex(GModel *m, int tag, double ms) : GEntity(m, tag), meshSize(ms)
{

}
void GVertex::addEdge(GEdge *e)
{
	if(std::find(l_edges.begin(), l_edges.end(), e) == l_edges.end())
		l_edges.push_back(e);
}

void GVertex::delEdge(GEdge *e)
{
	std::list<GEdge*>::iterator it = std::find(l_edges.begin(), l_edges.end(), e);
	if(it != l_edges.end()) l_edges.erase(it);
}
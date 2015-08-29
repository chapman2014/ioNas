#include "stdafx.h"
#include "GModel.h"
#include "discreteVertex.h"

void GModel::_storePointInEntities(std::map<int, MVertex*> &mapV)
{
  if(!mapV.size())
  {
  printf("No Vertex in the Nas File");
  return;
  }
  std::map< int,MVertex*>::iterator it=mapV.begin();
  for(;it!=mapV.end();++it)
  {
   GVertex *v=getVertexByTag(it->first);
   if(!v)
   {
	   v = new discreteVertex(this, it->first);//���õ��id�����µ� 
	   add(v);                             //��GModel����ӵ�
   }
  }
}



GVertex *GModel::getVertexByTag(int n) const
{
	GEntity tmp((GModel*)this, n);
	std::set<GVertex*, GEntityLessThan>::const_iterator it = vertices.find((GVertex*)&tmp);
	if(it != vertices.end())
	{return *it;}
	else
	{return 0;}
}
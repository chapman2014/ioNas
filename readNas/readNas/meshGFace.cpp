#include "StdAfx.h"

#include <sstream>
#include <stdlib.h>
#include <map>
#include "meshGFace.h"
#include "DivideAndConquer.h"
#include "MVertex.h"
#include "ioNas.h"
#include "SBoundingBox3d.h"

extern map<int, MVertex*> vertexData;

extern map<int, std::vector<MTriangle*> > triangleData;

//已有：1D离散化的数据，比如点集
//输出：包含bounding box 的三角化，不包含边恢复
// Builds An initial triangular mesh that respects the boundaries of
// the domain, including embedded points and surfaces  2DMesh第一步，此前应该已经获得1D离散化的数据
bool meshGenerator()
{


	//获得uv之后
   //对Doc格式进行处理
    // DocRecord doc(4);
	std::set<MVertex*>all_vertices;


	//此处添加读nas文件的点

	map<int, MVertex*>::iterator it;
	for(it=vertexData.begin();it!=vertexData.end();it++)
	{
	 all_vertices.insert(it->second);
	}

	if(all_vertices.size() < 3){
		printf("Mesh Generation of Model Face  Skipped: "
			"Only %d Mesh Vertices on The Contours",all_vertices.size());
		return true;
	}

	//参数化  加入bounding box
	// 二维实体，因为z==0，直接将xy当作uv
	SBoundingBox3d bbox;
	for(std::set<MVertex*>::iterator it = all_vertices.begin();
		it != all_vertices.end(); it++){
       
			bbox += SPoint3((*it)->_x,(*it)->_y, 0);                 //加入Bounding Box

	}
	bbox.makeCube();

	// compute the bounding box in parametric space
	SVector3 dd(bbox.max(), bbox.min());
	double LC2D = norm(dd);

	 DocRecord doc(all_vertices.size() + 4);
	 {
		 int i=0;
		 for(set<MVertex*>::iterator it=all_vertices.begin(); it != all_vertices.end(); it++){

			 //没有加随机数


			double XX = 0.00000001* LC2D * (double)rand() /(double)RAND_MAX; //
			double YY = 0.00000001 * LC2D * (double)rand() /(double)RAND_MAX;
			 //      printf("%22.15E %22.15E \n",XX,YY);
			 //double XX=0.00000005;
			 //double YY=0.00000005;
			 doc.points[i].where.h = (*it)->_x + XX;
			 doc.points[i].where.v =(*it)->_y+ YY;
			// doc.points[i].data = points[i];
			 doc.points[i].adjacent = NULL;

			 //added by ligen
			 doc.points[i].id=(*it)->_index;  //
			 //XX*=1.1;
			 //YY*=1.1;
			 i++;
		 }
	 }
		 // increase the size of the bounding box
		 bbox *= 2.5;
		 // add 4 points than encloses the domain (use negative number to
		 // distinguish those fake vertices)
		 double bb[4][2] = {{bbox.min().x(), bbox.min().y()},
		 {bbox.min().x(), bbox.max().y()},
		 {bbox.max().x(), bbox.min().y()},
		 {bbox.max().x(), bbox.max().y()}};
		 for(int ip = 0; ip < 4; ip++){
			 int id=all_vertices.size()+10+ip;
			 doc.points[all_vertices.size() + ip].where.h = bb[ip][0];
			 doc.points[all_vertices.size() + ip].where.v = bb[ip][1];
			 doc.points[all_vertices.size() + ip].adjacent = 0;
			 doc.points[all_vertices.size() + ip].id = id;
			 
			 MVertex * tmp=new MVertex(bb[ip][0],bb[ip][1],0,id);
			 vertexData[id]=(tmp);
		  // doc.points[points.size() + ip].data = pp;
		 }

		 doc.MakeMeshWithPoints();
		 triangleData.clear();
        //处理完毕，准备输出
		 for(int i = 0; i < doc.numTriangles; i++) {
			 int a = doc.triangles[i].a;
			 int b = doc.triangles[i].b;
			 int c = doc.triangles[i].c;
			 int n = doc.numPoints;
			 if(a < 0 || a >= n || b < 0 || b >= n || c < 0 || c >= n){
				printf("Skipping bad triangle %d", i);
				 continue;
			 }
			v.clear();
			int hahha=doc.points[a].id;
			MVertex* tmp=vertexData.find(doc.points[a].id)->second;
		    v.push_back(tmp);
			tmp=vertexData.find(doc.points[b].id)->second;
			v.push_back(tmp);
			tmp=vertexData.find(doc.points[c].id)->second;
			v.push_back(tmp);
			MTriangle * tri=new MTriangle(v,i);
		    triangleData[0].push_back(tri);
		 }
          
	return 1;
}



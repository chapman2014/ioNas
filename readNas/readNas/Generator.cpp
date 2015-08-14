#include <stdlib.h>
#include <stack>
#include "GModel.h"
#include "MLine.h"
#include "MTriangle.h"
#include "meshGFace.h"
#include <algorithm>


void GenerateMesh(GModel *m, int ask)
{
	// Initialize pseudo random mesh generator with the same seed
	srand(1);

	

	// 1D mesh
	if(ask == 1 ) {
		/*std::for_each(m->firstRegion(), m->lastRegion(), deMeshGRegion());
		std::for_each(m->firstFace(), m->lastFace(), deMeshGFace());*/           //目前只针对二维Mesh，不涉及多个face
		Mesh1D(m);
	}

	// 2D mesh
	if(ask == 2 ) {
		/*std::for_each(m->firstRegion(), m->lastRegion(), deMeshGRegion());*/
		Mesh2D(m);
	}
}

static void Mesh1D(GModel *m)
{

	printf("Meshing 1D...\n");


	for(GModel::eiter it = m->firstEdge(); it != m->lastEdge(); ++it)
		(*it)->meshStatistics.status = GEdge::PENDING;


	int nIter = 0, nTot = m->getNumEdges();
	while(1){
		meshGEdge mesher;
		int nPending = 0;
		for(GModel::eiter it = m->firstEdge(); it != m->lastEdge(); ++it){
			if ((*it)->meshStatistics.status == GEdge::PENDING){
				mesher(*it);
				nPending++;
			}
			if(!nIter) printf( "Meshing 1D...the %d/%d edge\n",nPending, nTot);
		}
		if(!nPending) break;
		if(nIter++ > 10) break;
	}


	printf("Done meshing 1D (%g s)");
}
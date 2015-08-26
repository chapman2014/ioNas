#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <vector>
#include <map>
#include <string>

#define NUM_SOLVERS 10

struct contextMeshOptions {
	int draw, changed, light, lightTwoSide, lightLines, pointType;
	int points, lines, triangles, quadrangles, tetrahedra, hexahedra, prisms, pyramids;
	int surfacesEdges, surfacesFaces, volumesEdges, volumesFaces, numSubEdges;
	int pointsNum, linesNum, surfacesNum, volumesNum, qualityType, labelType;
	int optimize, optimizeNetgen, optimizeLloyd, smoothCrossField, refineSteps, remove4triangles;
	double normals, tangents, explode, angleSmoothNormals, allowSwapEdgeAngle;
	double mshFileVersion, mshFilePartitioned, pointSize, lineWidth;
	double qualityInf, qualitySup, radiusInf, radiusSup;
	double scalingFactor, lcFactor, randFactor, lcIntegrationPrecision;
	double lcMin, lcMax, toleranceEdgeLength, anisoMax, smoothRatio;
	int lcFromPoints, lcFromCurvature, lcExtendFromBoundary;
	int dual, voronoi, drawSkinOnly, colorCarousel, labelSampling;
	int fileFormat, nbSmoothing, algo2d, algo3d, algoSubdivide;
	int algoRecombine, recombineAll, recombine3DAll, flexibleTransfinite;
	//-- for recombination test (amaury) --
	int doRecombinationTest, recombinationTestStart;
	int recombinationTestNoGreedyStrat, recombinationTestNewStrat;
	int nProc, nbProc;
	std::string recTestName;
	//-------------------------------------
	int remeshParam, remeshAlgo;
	int order, secondOrderLinear, secondOrderIncomplete;
	int secondOrderExperimental, meshOnlyVisible;
	int minCircPoints, minCurvPoints;
	int hoOptimize, hoNLayers, hoOptPrimSurfMesh;
	double hoThresholdMin, hoThresholdMax, hoPoissonRatio;
	int saveAll, saveTri, saveGroupsOfNodes, binary, bdfFieldFormat, saveParametric;
	int smoothNormals, reverseAllNormals, zoneDefinition, clip;
	int saveElementTagType;
	int switchElementTags;
	int multiplePasses;
	int cgnsImportOrder;
	std::map<int,int> algo2d_per_face;
	std::map<int,int> curvature_control_per_face;
	int bunin;
	int ignorePartBound;
	int NewtonConvergenceTestXYZ;
};

struct contextGeometryOptions {
	int draw, light, lightTwoSide, points, lines, surfaces, volumes;
	int pointsNum, linesNum, surfacesNum, volumesNum, labelType;
	double pointSize, lineWidth, selectedPointSize, selectedLineWidth;
	int pointType, lineType, surfaceType, numSubEdges;
	int oldCircle, oldNewreg, oldRuledSurface;
	int extrudeSplinePoints, extrudeReturnLateral;
	double normals, tangents, scalingFactor;
	int autoCoherence, highlightOrphans, clip, useTransform;
	double tolerance, snap[3], transform[3][3], offset[3];
	int occFixDegenerated, occFixSmallEdges, occFixSmallFaces;
	int occSewFaces, occConnectFaces;
	int copyMeshingMethod, exactExtrusion;
	int matchGeomAndMesh;
	int hideCompounds, orientedPhysicals;
};

class CTX{
private:
	static CTX* _instance;

public:
	CTX();
	~CTX();

	static CTX *instance();
	contextGeometryOptions geom;
	// mesh options
	contextMeshOptions mesh;

	// characteristic length for the whole problem (never used in mesh
	// generation ->only for geo/post)
	double lc;
};


#endif
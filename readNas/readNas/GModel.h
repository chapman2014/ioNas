#pragma once 
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <string>
#include "GVertex.h"
#include "GEdge.h"
#include "SPoint3.h"
#include "SBoundingBox3d.h"

class  GModel
{
protected:
	// the name of the model
	std::string _name;

	// the name of the file the model was read from
	std::string _fileName;
	std::set<std::string> _fileNames;

	// entity that is currently being meshed (used for error reporting)
	GEntity *_currentMeshEntity;

protected:
	// the sets of geometrical regions, faces, edges and vertices in the
	// model
	std::set<GEdge*, GEntityLessThan> edges;
	std::set<GVertex*, GEntityLessThan> vertices;


public:
	GModel(std::string name="");
	virtual ~GModel();


	// get the number of entities in this model
	int getNumEdges() const { return edges.size(); }
	int getNumVertices() const { return vertices.size(); }


	typedef std::set<GEdge*, GEntityLessThan>::iterator eiter;
	typedef std::set<GVertex*, GEntityLessThan>::iterator viter;

	// get an iterator initialized to the first/last entity in this model
	eiter firstEdge() { return edges.begin(); }
	viter firstVertex() { return vertices.begin(); }
	eiter lastEdge() { return edges.end(); }
	viter lastVertex() { return vertices.end(); }

	// add/remove an entity in the model
	void add(GEdge *e) { edges.insert(e); }
	void add(GVertex *v) { vertices.insert(v);}

	// set/get entity that is currently being meshed (for error reporting)
	void setCurrentMeshEntity(GEntity *e){ _currentMeshEntity = e; }
	GEntity *getCurrentMeshEntity(){ return _currentMeshEntity; }

	// find the entity with the given tag
	GEdge *getEdgeByTag(int n) const;
	GVertex *getVertexByTag(int n) const;

	//Ìí¼Óµãµ½model
	void _storePointInEntities(std::map<int, MVertex*> &map);


};
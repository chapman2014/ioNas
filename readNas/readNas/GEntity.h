


class GVertex;
class GEdge;
class GModel;

class GEntity{
private:
	// all entities are owned by a GModel
	GModel *_model;

	// the tag (the number) of this entity
	int _tag;

	// gives the number of the master entity in periodic mesh, gives _tag
	// if non-periodic
	int _meshMaster;

public:
	//尚未添加到cpp文件中
	GEntity(GModel *m, int t): _model(m), _tag(t), _meshMaster(t){};

	// these will become protected at some point
	// the mesh vertices uniquely owned by the entity
	std::vector<MVertex*> mesh_vertices;


	// the model owning this entity
	GModel *model() const { return _model; }

	// get/set the tag of the entity
	int tag() const { return _tag; }
	void setTag(int tag) { _tag = tag; }

	enum MeshGenerationStatus {
		PENDING,
		DONE,
		FAILED
	};

	

};

class GEntityLessThan {
public:
	bool operator()(const GEntity *ent1, const GEntity *ent2) const
	{
		return ent1->tag() < ent2->tag();
	}
};
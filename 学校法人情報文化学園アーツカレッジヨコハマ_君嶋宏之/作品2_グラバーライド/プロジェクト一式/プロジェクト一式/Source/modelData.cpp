#include "modelData.h"
#include "stageObjectManager.h"


ModelData::ModelData(SceneBase* _scene):LoadBase(_scene)
{

}

ModelData::~ModelData()
{
	DeleteHandle(loadMap,MV1DeleteModel);
}

void ModelData::Load()
{
	
	for (int i = 0; i < (int)ObjectType::MAX_NUN; i++) {
		
		char path[60];
		sprintfDx(path, "data\\model\\model%d.mv1", i);
		int handle = MV1LoadModel(path);
		std::string key = std::to_string(i);
		AddHandleToMap(loadMap, key, handle);
		load = 1;
	}
	load = 0;
}



#include "modelLoad.h"


ModelLoad::ModelLoad(SceneBase* _scene) :LoadBase(_scene)
{
	for (auto& model : MODEL) {
		int handle = MV1LoadModel(model.path.c_str());
		AddHandleToMap(model.key,handle);
	}

}

ModelLoad::~ModelLoad()
{
	DeleteHandle(MV1DeleteModel);
}


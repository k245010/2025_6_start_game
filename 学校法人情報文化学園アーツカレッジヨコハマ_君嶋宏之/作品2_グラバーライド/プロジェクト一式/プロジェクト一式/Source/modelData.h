#pragma once
#include "loadBase.h"
#include "LoadData.h"



class ModelData : public LoadBase
{
public:
	ModelData(SceneBase* _scene);
	~ModelData();
	void Load()override;
};
#include "effekseerData.h"

EffekseerData::EffekseerData(SceneBase* _scene) :LoadBase(_scene)
{
	load = 2;
}

EffekseerData::~EffekseerData()
{
	DeleteHandle(loadMap,DeleteEffekseerEffect);
}

void EffekseerData::Load()
{
	
	for (auto& e:EFFECT) {
		
		int handle = LoadEffekseerEffect(e.data.path.c_str(), e.rate);
		AddHandleToMap(loadMap,e.data.key,handle);
		
		
	}
	
}

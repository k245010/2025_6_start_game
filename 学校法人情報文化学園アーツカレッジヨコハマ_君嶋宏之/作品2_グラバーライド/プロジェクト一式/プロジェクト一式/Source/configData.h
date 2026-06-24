#pragma once
#include "../Library/gameObject.h"  
#include <vector>
class ConfigData : public GameObject {
public:
	ConfigData(SceneBase* _scene);
	~ConfigData();
	struct ConfigD {
		int baseVolume;
		int bgmBaseVolume;
		int seBaseVolume;
	};
	ConfigD GetConfigData()const { return configData; }
private:
	
	ConfigD configData;
};

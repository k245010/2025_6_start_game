#pragma once
#include "loadBase.h"

class SoundLoadData :public LoadBase {
public:
	SoundLoadData(SceneBase*_scene);
	~SoundLoadData();
	int GetSoundHandle(std::string dType, std::string sType, std::string sKey);
private:
	std::unordered_map<std::string,std::unordered_map<std::string, std::unordered_map<std::string, int>>> sound;

	int LoadSoundMem3D(std::string path);
	void Load()override;
};
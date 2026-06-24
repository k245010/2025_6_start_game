#include "soundLoadData.h"

SoundLoadData::SoundLoadData(SceneBase* _scene):LoadBase(_scene)
{

}

SoundLoadData::~SoundLoadData()
{
	for (auto& category : sound) {
		for (auto& soundData : category.second) {
			for(auto& s:soundData.second)
			DeleteSoundMem(s.second);
		}
	}
	sound.clear();
}

int SoundLoadData::GetSoundHandle(std::string dType,std::string sType, std::string sKey)
{
	auto dItr = sound.find(dType);//Žw’èƒL[‚ðŽæ“¾
	auto sItr = dItr->second.find(sType);
	auto sKItr = sItr->second.find(sKey);
	if (sound.end() == dItr||dItr->second.end()==sItr|| sItr->second.end() == sKItr) {//–³‚©‚Á‚½‚ç
		return -1;
	}
	return sKItr->second;
}

int SoundLoadData::LoadSoundMem3D(std::string path)
{
	SetCreate3DSoundFlag(true);
	int handle = LoadSoundMem(path.c_str());
	SetCreate3DSoundFlag(false);
	return handle;
}

void SoundLoadData::Load()
{
	
	//sound.emplace("SE",)
	std::unordered_map<std::string, int> se;
	std::unordered_map<std::string, int> bgm;
	std::unordered_map<std::string, int> thse;
	std::unordered_map<std::string, int> thbgm;
	std::unordered_map<std::string, std::unordered_map<std::string, int>> td;
	std::unordered_map<std::string, std::unordered_map<std::string, int>> thd;

	for (auto& s : SDSOUND) {
		int handle = 0;
		handle = LoadSoundMem(s.data.path.c_str());

		//ChangeVolumeSoundMem(s.volume, handle);

		if (s.sKey == "SE") {
			AddHandleToMap(se, s.data.key, handle);

		}
		else if (s.sKey == "BGM") {

			AddHandleToMap(bgm, s.data.key, handle);
		}
		
	}
	for (auto& s : TDSOUND) {
		int handle = 0;
		handle = LoadSoundMem3D(s.data.path.c_str());

		//ChangeVolumeSoundMem(s.volume, handle);
		Set3DRadiusSoundMem(s.distance, handle);
		if (s.sKey == "SE") {
			AddHandleToMap(thse, s.data.key, handle);

		}
		else if (s.sKey == "BGM") {

			AddHandleToMap(thbgm, s.data.key, handle);
		}
		
	}

	td.emplace("SE", se);
	td.emplace("BGM", bgm);
	thd.emplace("SE", thse);
	thd.emplace("BGM", thbgm);

	sound.emplace("2D", td);
	sound.emplace("3D", thd);
	
}



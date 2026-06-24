#include "configData.h"
#include <cassert>

ConfigData::ConfigData(SceneBase* _scene):GameObject(_scene)
{
	FILE* fp = 0;
	std::string path;
	path += "configData.csv";
	errno_t err;
	err = fopen_s(&fp, path.c_str(), "rt");
	if (err != 0) {
		assert(err == 0 && "ƒtƒ@ƒCƒ‹“Ç‚Ýž‚Ý‚ÉŽ¸”s");
	}
	
	fscanf_s(fp, "%d,%d,%d", &configData.baseVolume,&configData.seBaseVolume, &configData.bgmBaseVolume);
	//StageEditor* s = GetScene()->FindGameObject<StageEditor>();
	//s->SetStageConfig(sc);

	fclose(fp);
}

ConfigData::~ConfigData()
{
}

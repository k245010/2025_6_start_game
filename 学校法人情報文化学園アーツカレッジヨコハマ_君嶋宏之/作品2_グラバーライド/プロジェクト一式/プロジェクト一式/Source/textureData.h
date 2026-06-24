#pragma once
#pragma once
#include "LoadData.h"
#include "loadBase.h"

class TextureData : public LoadBase
{
public:
	TextureData(SceneBase* _scene);
	~TextureData();
	void Load()override;
private:
};

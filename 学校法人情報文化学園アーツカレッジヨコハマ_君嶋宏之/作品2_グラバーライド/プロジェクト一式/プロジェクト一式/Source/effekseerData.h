#pragma once
#include "LoadData.h"
#include "loadBase.h"
#include <EffekseerForDXLib.h>

class EffekseerData : public LoadBase
{
public:
	EffekseerData(SceneBase* _scene);
	~EffekseerData();
	void Load()override;

private:
};

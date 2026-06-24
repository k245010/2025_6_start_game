#pragma once


#include "../Library/gameObject.h"

enum class BackGroundType
{
	D2=1,
	D3,
};
class LoadManager;
class StageData;
class BackGroundManager:public GameObject
{
public:
	BackGroundManager(SceneBase*_scene);
	~BackGroundManager();
	//void Draw()override;
	void Update()override;

	void CreateBackGround(BackGroundType _bgt,int _handle);
private:	
	
	GameObject* backGround;
	LoadManager* loadManager;
	StageData* stageData;
	int handle;
};


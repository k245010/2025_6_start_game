#pragma once
#include "../Library/gameObject.h"
#include <vector>
#include "../Library/sceneManager.h"
#include "stageScore.h"



class StageObjectManager;
class StageData;
class CommonData;
class BackGroundManager;
class LoadManager;
class Stage :public GameObject {
public:
	Stage(SceneBase* scene);
	~Stage();
	void Draw()override;
	void Update()override;
	void CreateStageObject();
	void Reset();
	void LoadStage(const char* str);
	/// <summary>
	/// プレイヤー以外を削除
	/// </summary>
	void ReloadStage();
	void ResetStage();
	void CreateStage();
private:
	LoadManager* loadManager;
	StageScore* score;
	CommonData* commonData;
	StageObjectManager* ob;
	StageData* stage;
	BackGroundManager* backGroundManager;
};



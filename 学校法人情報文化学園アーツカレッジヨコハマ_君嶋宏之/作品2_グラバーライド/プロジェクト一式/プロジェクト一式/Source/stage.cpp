#include "stage.h"

#include "stageObjectManager.h"
#include "stageObject.h"
#include "function.h"
#include "commonData.h"
#include "stageData.h"
#include "../ImGui/imgui.h"
#include "backGroundManager.h"
Stage::Stage(SceneBase* scene):GameObject(scene)
{
	CommonFind* commonFind = SceneManager::CommonScene()->FindGameObject<CommonFind>();
	ob = scene->FindGameObject<StageObjectManager>();
	stage = commonFind->CommonFindObject<StageData>();
	commonData = commonFind->CommonFindObject<CommonData>();
	
	
	score = scene->FindGameObject<StageScore>();
	
	loadManager = commonFind->CommonFindObject<LoadManager>();
	CreateStage();
}

Stage::~Stage()
{
	
}

void Stage::Draw()
{
	
}
void Stage::Update()
{
#if _DEBUG
	ImGui::Begin("stage");
	if (ImGui::Button("Reset")) {
		ReloadStage();
	}

	ImGui::End();
#endif // _DEBUG

	

}

void Stage::CreateStageObject()
{
	
	for (int i = 0; i < (int)stage->GetCharaSize(); i++){
		auto s=stage->GetCharaLines(i);
		ob->CreateCharaObject(s.pos,s.scale, s.rotation, s.type, s.collisionId,s.slot);
	}
	for (int i = 0; i < (int)stage->GetTerraSize(); i++) {
		auto s = stage->GetTerraLines(i);
		ob->CreateTerraObject(s.pos,s.scale,s.rotation, s.type, s.collisionId,s.slot);
	}
	score->SetCoinTarget(ob->GetCoinNum());
	commonData->SetMaxCoinNum(ob->GetCoinNum());
}

void Stage::Reset()
{
	stage->AllClear();
	ob->DeleteObject();
}

void Stage::LoadStage(const char* str)
{
	//Reset();
	CreateStageObject();
}

void Stage::ReloadStage()
{
	ob->DeletePointToCoinToPlayerOtherObject();

	for (int i = 0; i < (int)stage->GetCharaSize(); i++) {
		auto s = stage->GetCharaLines(i);
		if (s.type != (int)ObjectType::PLAYER)
		ob->CreateCharaObject(s.pos, s.scale, s.rotation, s.type, s.collisionId, s.slot);
	}
	for (int i = 0; i < (int)stage->GetTerraSize(); i++) {
		auto s = stage->GetTerraLines(i);
		if (s.type != (int)ObjectType::POINT && s.type != (int)ObjectType::COIN)
		ob->CreateTerraObject(s.pos, s.scale, s.rotation, s.type, s.collisionId, s.slot);
	}

	//score->SetCoinTarget(ob->GetCoinNum());
}


void Stage::ResetStage()
{
	
	ob->DeletePlayerOtherObject();
	CreateStageObject();
}

void Stage::CreateStage()
{
	CreateStageObject();

}




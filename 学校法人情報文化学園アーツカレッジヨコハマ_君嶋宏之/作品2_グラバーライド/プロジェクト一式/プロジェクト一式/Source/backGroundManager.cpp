#include "backGroundManager.h"
#include "2dBackGround.h"
#include "3dBackGround.h"
#include "../Library/sceneManager.h"
#include "stageData.h"
#include "loadManager.h"
#include "camera.h"
BackGroundManager::BackGroundManager(SceneBase*_scene):GameObject(_scene)
{
	backGround = nullptr;

	loadManager = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<LoadManager>();
	stageData= SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<StageData>();
	
	auto scf = stageData->GetStageConfigData();
	std::string bgt = std::to_string(scf.backGroundData) + "backGround";
	handle = loadManager->GetHandle(bgt);
	CreateBackGround(BackGroundType(scf.backGroundData / 100), handle);
	
}

BackGroundManager::~BackGroundManager()
{
	//backGround->DestroyMe();
	//backGround = nullptr;
}



void BackGroundManager::Update()
{

}
void BackGroundManager::CreateBackGround(BackGroundType _bgt, int _handle)
{
	switch (_bgt)
	{
	case BackGroundType::D2:
	{
		BackGround2D* b2 = GetScene()->CreateGameObject<BackGround2D>();
		b2->Init(_handle);
		backGround = b2;
	}
		break;
	case BackGroundType::D3:
	{
		BackGround3D* b3 = GetScene()->CreateGameObject<BackGround3D>();
		Camera* camera = GetScene()->FindGameObject<Camera>();
		b3->Init(_handle,camera);
		backGround = b3;
	}
		break;
	default:
		break;
	}
	GetScene()->SetDrawOrder(backGround, 1);
}

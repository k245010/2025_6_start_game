#include "PlayScene.h"
#include "../../Camera/Camera.h"
#include "../../Navigation/NavigationManager/NavigationManager.h"
#include "../../Player/Player.h"
#include "../../Game/GameController.h"
#include "../../Stage/StageManager/StageManager.h"
#include "../../Collision/CollisionManager/CollisionManager.h"
#include "../../Shot/ShotManager/ShotManager.h"
#include "../../Shot/Bullet/BulletManager/BulletManager.h"
#include "../../Enemy/EnemyManager/EnemyManager.h"
#include "../../Wave/WaveController/WaveController.h"
#include "../../Screen.h"
#include "../../Gun/GunManager/GunManager.h"
#include "../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../Common/Light/Light.h"
#include "../../Common/Fog/Fog/Fog.h"
#include "../../../ImGui/imgui.h"
#include <vector>
#include <functional>
#include "../../../Library/DebugNew.h"

int PlayScene::startStageNumber = 1;
bool PlayScene::isNavMeshCreate = true;

PlayScene::PlayScene()
{
	fog					= new Fog();
	light				= new Light();
	bulletManager		= new BulletManager();
	shotManager			= new ShotManager();
	gameController		= new GameController();
	camera				= new Camera();
	navigationManager	= new NavigationManager();
	enemyManager		= new EnemyManager();
	player				= new Player();
	stageManager		= new StageManager();
	
	waveController		= new WaveController();

	stageManager->SetDeleteOrder(1);
	stageManager->Init(PlayScene::startStageNumber);
	gameController->Init();
	navigationManager->Init(PlayScene::isNavMeshCreate);
	camera->Init();
	enemyManager->Init();
	waveController->Init();
	shotManager->Init();
	bulletManager->Init(stageManager);
	player->Init();
	

	gameController->SetDrawOrder(-100);
	waveController->SetDrawOrder(-100);
	player->SetDrawOrder(-9999);
	enemyManager->SetDrawOrder(-1);
	stageManager->SetDrawOrder(-9999);

	light->Init();
}

PlayScene::~PlayScene()
{
	CollisionManager::GetCollisionManagerInstance()->DeleteCollisionManagerInstance();
	GunManager::GetGunManagerInstance()->DeleteGunManagerInstance();

	// エフェクトの削除
	EffekseerObjectManager::DeleteAllEffect();
}

void PlayScene::Update()
{
	CollisionManager::GetCollisionManagerInstance()->Update();
	GunManager::GetGunManagerInstance()->Update();

	if (CheckHitKey(KEY_INPUT_6) && CheckHitKey(KEY_INPUT_0))
	{
		SceneManager::Exit();
	}
}

void PlayScene::Draw()
{
	CollisionManager::GetCollisionManagerInstance()->Draw();
	GunManager::GetGunManagerInstance()->Draw();

	//DrawFormatString(Screen::WIDTH - 100, 0, 0xffffff, "FPS %.1lf", GetFPS());

	//DrawLine3D(VECTOR3(2202.05908, -779.513733, -2622.47583), VECTOR3(2244.35742, 501.649231, -2059.59717), 0xff0000);
}

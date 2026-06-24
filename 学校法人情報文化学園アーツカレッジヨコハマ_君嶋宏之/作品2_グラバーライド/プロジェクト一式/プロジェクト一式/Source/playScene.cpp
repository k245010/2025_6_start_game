#include "playScene.h"
#include "../Library/sceneManager.h"
#include "DebugScreen.h"
#include "camera.h"
#include "light.h"
#include "player.h"
#include "slowMostionManager.h"
#include "gravityManager.h"
#include "stage.h"
#include "stageObjectManager.h"
#include "backGroundManager.h"
#include "commonFind.h"
#include "collisionManager.h"
#include "effectManager.h"
#include "gameManager.h"
#include "pauseUi.h"
#include "clearUi.h"
#include "font.h"
#include "stageScore.h"
#include "playUi.h"
//#include "pushTransitor.h"

PlayScene::PlayScene()
{
	commonFind = SceneManager::CommonScene()->FindGameObject<CommonFind>();
	//slowMostionManager = commonFind->CommonFindObject<SlowMostionManager>();
	
	light = CreateGameObject<Light>();

	effectManager = CreateGameObject<EffectManager>();
	
	//anime = CreateGameObject <Anime>();
	gravityManager = CreateGameObject<GravityManager>();

	stageScore = CreateGameObject<StageScore>();

	stageManager = CreateGameObject<StageObjectManager>();
	
	stage = CreateGameObject<Stage>();
	
	camera = CreateGameObject<Camera>();
	backGroundManager = CreateGameObject<BackGroundManager>();
	SetDrawOrder(backGroundManager, 10);

	collisionManager = CollisionManager::GetCollisionManagerObject();
	gameMananger = CreateGameObject<GameManager>();

	font = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<Font>();

	pauseUi = CreateGameObject<PauseUi>();
	clearUi = CreateGameObject<ClearUi>();
	playUi = CreateGameObject<PlayUi>();
}

PlayScene::~PlayScene()
{
	CollisionManager::DeleteCollisionManagerObject();
	
}

void PlayScene::Update()
{
#ifdef _DEBUG

	if (CheckHitKey(KEY_INPUT_T)) {
		//slowMostionManager->DeleteSlowObject();
		clsDx();
		SceneManager::ChangeScene("SelectScene");
	}
#endif // _DEBUG

	SceneBase::Update();
	
	if (collisionManager != nullptr)
		collisionManager->Update();
}

void PlayScene::Draw()
{
	SceneBase::Draw();	
	
	
	//DrawString(0, 0, "PLAY SCENE", GetColor(255, 255, 255));

	//font->FontDraw(V2Get((float)SCREEN_WIDTH_CENTER, (float)SCREEN_HEIGHT_CENTER), V2Get(3.0f, 3.0f), 0.0f, FONT_KIND::POP, RGB_RED, "‚ ‚ ‚ ");

#ifdef _DEBUG
	DrawFormatString(1900, 0, 0xffffff, "%f", GetFPS());
	DrawString(100, 400, "Push [T]Key To Title", GetColor(255, 255, 255));
	if (collisionManager != nullptr)
		collisionManager->Draw();
#endif
}

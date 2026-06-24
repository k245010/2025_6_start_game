#include "gameManager.h"
#include "player.h"
#include "debugScreen.h"
#include "inputManager.h"
#include "commonFind.h"
#include "soundManager.h"
#include "stage.h"

#include "camera.h"
#include "productionh.h"
#include "stageData.h"
#include "effectManager.h"
#include "Screen.h"
namespace {
	const int GAME_START_COUNT = 240;//240
	const int GOAL_COUNT = 120;
	const int PLAY_AFTER_COUNT = 120;
	const int PAUSE_COUNT = 240;
	
}

GameManager::GameManager(SceneBase* _scene)
{
	gameState = PLAY_BEFORE;
	enemyManager =nullptr;
	player = _scene->FindGameObject<Player>();
	stage = _scene->FindGameObject<Stage>();
	Camera* camera = _scene->FindGameObject<Camera>();

	playBeforeCount = GAME_START_COUNT;
	goalCount = GOAL_COUNT;
	playAfterCount= PLAY_AFTER_COUNT;
	pauseCount = PAUSE_COUNT;

	pauseNum = 1;
	inputManager = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<InputManager>();
	gravityManager = _scene->FindGameObject<GravityManager>();

	sound = SceneManager::CommonScene()->FindGameObject<SoundManager>();
	slow = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<SlowMostionManager>();
	production = SceneManager::CommonScene()->FindGameObject<Production>();
	stageData = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<StageData>();
	effectManager = _scene->FindGameObject<EffectManager>();
	commonData = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<CommonData>();
	commonData->SetClear(false);
	gameFinish = false;
}

GameManager::~GameManager()
{


}

void GameManager::Update()
{
	switch (gameState)
	{
	case GameManager::PLAY_BEFORE:
		
		
			if (player->GetPlayStart()) {
				ChangeState(PLAYING);
				std::string bg = std::to_string(stageData->GetStageConfigData().bgmData);
				sound->PlayBgm(bg+ "bgmStage");
			}
		
		break;
	case GameManager::PLAYING:
		if (player->Goal()) {
			ChangeState(GOAL_FRONT_ANI);
		}
		if (player->noHp()) {// || CheckCameraViewClip(player->GetPosition())
			ChangeState(LOSE);
		}
		if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_RCONTROL, XINPUT_BUTTON_START)) {
			ChangeState(MENU);
			scene = PointStart;
			cursolControl = false;
			pauseNum = 1;
		}
		break;
	case GameManager::MENU:
		if (countStart) {
			pauseCount--;
			if (pauseCount <= 0) {
				//sound->AllStopSound();
				//player = GetScene()->FindGameObject<Player>();
				ChangeState(PLAYING);
				pauseCount = PAUSE_COUNT;
				countStart = false;
				if (buttonNum == 0) {
					stage->ReloadStage();
				}
			}
		}
		else {
			if (cursolControl) {
				if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) || inputManager->GetIsKeyPutNow(XINPUT_BUTTON_START)) {
					countStart = true;
					pauseCount /= 4;
					buttonNum = 1;
				}
				if (inputManager->GetIsKeyboardPut(KEY_INPUT_DOWN) || inputManager->GetStickKnockingPut(0.9f).leftStick == S_DOWN) {
					pauseNum = min(pauseNum++, 3);
					scene = (LoseSceneSelect)pauseNum;
					sound->PlaySe("セレクト");

				}
				if (inputManager->GetIsKeyboardPut(KEY_INPUT_UP) || inputManager->GetStickKnockingPut(0.9f).leftStick == S_UP) {
					pauseNum = max(pauseNum--, 1);
					scene = (LoseSceneSelect)pauseNum;
					sound->PlaySe("セレクト");
				}
				if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_Z, XINPUT_BUTTON_A)) {
					sound->PlaySe("決定");
					buttonNum = 0;
					switch (scene)
					{
					case GameManager::PointStart:
						effectManager->DeleteAllEffect();
						player->LoseMoveStart(scene, pauseCount);
						
						//sound->StopBgm();
						//gravityManager->ChangeWorldGravity(DOWN_GRAVITY);
						countStart = true;
						//ChangeState(LOSE);
						player->HpZero();
						break;
					case GameManager::FirstStart:
						effectManager->DeleteAllEffect();
						player->LoseMoveStart(scene, pauseCount);
						//stage->Stage();
						//gravityManager->ChangeWorldGravity(DOWN_GRAVITY);
						countStart = true;
						//ChangeState(LOSE);
						player->HpZero();
						break;
					case GameManager::SelectScene:
						//slow->DeleteSlowObject();
						sound->AllStopSound();
						production->SceneProductionChange("SelectScene");
						break;
					default:
						break;
					}
				}
			}
		}
		break;
	case GameManager::LOSE:
		if (countStart) {
			pauseCount--;
			if (pauseCount <= 0) {
				//sound->AllStopSound();
				//player = GetScene()->FindGameObject<Player>();
				ChangeState(PLAYING);
				pauseCount = PAUSE_COUNT;
				countStart = false;
				stage->ReloadStage();
			}
		}
		else {
			RespownWaitCounter++;
			if (RespownWaitCounter > 120) {
				effectManager->DeleteAllEffect();
				countStart = true;
				RespownWaitCounter = 0;
				player->LoseMoveStart(PointStart, pauseCount);
			}
		}
		break;
	case GameManager::GOAL_FRONT_ANI:
		if (player->GoalProductFinish()) {
			sound->AllStopSound();
			sound->PlaySe("テッテレー");
			ChangeState(RESULT_DRAW);
			sound->PlayBgm("リザルト音楽");
		}
		break;
	case GameManager::RESULT_DRAW:

		if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_Z,XINPUT_BUTTON_A) && gameFinish)
		{
			commonData->SetClear(true);
			sound->PlaySe("決定");
			ChangeState(PLAY_AFTER);
			player->PlayAfterStart();
			production->SceneProductionChange("SelectScene");
		}
		break;
	case GameManager::PLAY_AFTER:
		playAfterCount--;
		if (playAfterCount <= 0) {
			ChangeState(SCENE_CHANGE);
			sound->AllStopSound();
		}
		break;
	case GameManager::SCENE_CHANGE:
		break;
	}
}

void GameManager::Draw()
{
	if (gameState == GameManager::PLAY_BEFORE||gameState==GameManager::PLAY_AFTER) {
		DrawBox(0, 0, Screen::WIDTH, Screen::SCREEN_HEIGHT_CENTER - 400, 0x000000, true);
		DrawBox(0, Screen::HEIGHT, Screen::WIDTH, Screen::HEIGHT-140, 0x000000, true);
	}


	/*switch (gameState)
	{
	case GameManager::PLAY_BEFORE:
		break;
	case GameManager::PLAYING:
		break;
	case GameManager::GOAL:
		DrawFormatString(500, 500, 0xff00000, "GOAL!!!!!!");
		break;
	case MENU:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(100, 100, 1500, 800, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 250);
		DrawFormatString(500, 100, 0xff00000, "LOSE!!!!");
		DrawFormatString(400, INTERVEL_ALLOW * 1, 0xff0000, "中間ポイントから");
		DrawFormatString(400, INTERVEL_ALLOW * 2, 0xff0000, "初めから");
		DrawFormatString(400, INTERVEL_ALLOW * 3, 0xff0000, "セレクトシーンへ");
		DrawFormatString(350, INTERVEL_ALLOW * pauseNum, 0xff0000, "→");
		DrawFormatString(600, 800, 0xff00000, "Aボタン");
		DrawFormatString(800, 600, 0xff0000, "B : 戻る");
	case GameManager::LOSE:
		break;
	case GameManager::PLAY_AFTER:
		printfDx("222");
		break;
	case GameManager::SCENE_CHANGE:
		break;
	}*/
}
/*
void GameManager::ObjectGetPointer(EnemyManager* _enemyManager, Player* _player)
{
	enemyManager = _enemyManager;
	player = _player;
}*/

void GameManager::ChangeState(GAME_STATE _state)
{
	gameState = _state;
	player->GameStateChange(gameState);
	//enemyManager->ChangeGameState(gameState);
}

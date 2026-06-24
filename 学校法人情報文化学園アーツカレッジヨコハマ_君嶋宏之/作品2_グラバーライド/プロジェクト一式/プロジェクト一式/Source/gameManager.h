#pragma once
#include "../Library/gameObject.h"

class EnemyManager;
class Player;
class InputManager;
class GravityManager;
class SoundManager;
class Stage;
class SlowMostionManager;
class TransitorManager;
class Production;
class StageData;
class EffectManager;
class CommonData;

class GameManager : public GameObject
{
public:

	enum GAME_STATE
	{
		PLAY_BEFORE,
		PLAYING,
		MENU,
		LOSE,
		GOAL_FRONT_ANI,
		RESULT_DRAW,
		PLAY_AFTER,
		SCENE_CHANGE,

		GAME_STATE_MAX,
	};

	enum LoseSceneSelect
	{
		PointStart = 1,
		FirstStart,
		SelectScene,
	};
	GameManager(SceneBase* _scene);
	~GameManager();

	void Update()override;
	void Draw()override;

	//void ObjectGetPointer(EnemyManager* _enemyManager,Player* _player);

	GAME_STATE GetGameState() { return gameState; }

	void ChangeState(GAME_STATE _state);

	int& GetPauseNum() { return pauseNum; }

	bool GetCountStart(){ return countStart; }
	void CursolMoveOn() { cursolControl = true; }

	int GetPushButton() { return buttonNum; }
	void GameFinish() { gameFinish = true; }

private:
	Stage* stage;
	StageData* stageData;
	GAME_STATE gameState;
	EffectManager* effectManager;
	EnemyManager* enemyManager;
	Player* player;
	GravityManager* gravityManager;
	Production* production;
	int playBeforeCount;
	int goalCount;
	int playAfterCount;
	int pauseCount;

	int changeStateCount[GAME_STATE_MAX];
	InputManager* inputManager;
	LoseSceneSelect scene;
	int pauseNum;
	bool countStart;
	int RespownWaitCounter;
	SoundManager* sound;
	SlowMostionManager* slow;
	TransitorManager* transit;
	bool cursolControl;
	int buttonNum;
	CommonData* commonData;

	bool gameFinish;
};
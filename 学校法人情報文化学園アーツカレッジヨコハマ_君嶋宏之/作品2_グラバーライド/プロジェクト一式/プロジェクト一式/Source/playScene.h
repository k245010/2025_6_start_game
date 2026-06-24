#pragma once
#include "../Library/sceneBase.h"

class Camera;
class Light;

class PlayerUi;
class SlowMostionManager;
class GravityManager;
class Stage;
class StageObjectManager;
class CommonFind;
class CollisionManager;
class EffectManager;
class GameManager;
class EnemyManager;
class PauseUi;
class ClearUi;
class Font;
class StageScore;
class PlayUi;
class BackGroundManager;
//class BackGroundManager;
class PlayScene : public SceneBase {
public:
	PlayScene();
	~PlayScene();
	void Update() override;
	void Draw() override;
private:
	Light* light;
	Camera* camera;
	
	PlayerUi* playerUi;
	SlowMostionManager* slowMostionManager;
	GravityManager* gravityManager;
	Stage* stage;
	StageObjectManager* stageManager;
	CommonFind* commonFind;
	//BackGroundManager* backGroundManager;
	CollisionManager* collisionManager;
	EffectManager* effectManager;
	GameManager* gameMananger;
	EnemyManager* enemyManager;
	PauseUi* pauseUi;
	ClearUi* clearUi;
	Font* font;
	StageScore* stageScore;
	PlayUi* playUi;
	BackGroundManager* backGroundManager;
};
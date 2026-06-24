#pragma once
#include "../../../Library/SceneBase.h"
#include "../../../Library/GameObject.h"

/// <summary>
/// ゲームプレイのシーンを制御する
/// </summary>
class Fog;
class Light;
class GameController;
class NavigationManager;
class ShotManager;
class BulletManager;
class StageManager;
class CollisionManager;
class Camera;
class EnemyManager;
class Player;
class WaveController;
class PlayScene : public SceneBase
{
public:
	PlayScene();
	~PlayScene();
	void Update() override;
	void Draw() override;

	static int startStageNumber;	// ロードするゲームを開始するステージナンバー
	static bool isNavMeshCreate;	// ナビメッシュを構築するかどうか　true:構築 / false:ファイルからロード

private:

	Fog* fog;
	Light* light;
	GameController* gameController;
	Camera* camera;
	NavigationManager* navigationManager;
	ShotManager* shotManager;
	BulletManager* bulletManager;
	StageManager* stageManager;
	WaveController* waveController;
	EnemyManager* enemyManager;
	Player* player;
};

#pragma once
#include "../Library/gameObject.h"
#include "enemyBase.h"

enum EnemyKind
{
	ENEMY_1=0,
	ENEMY_2,

	ENEMY_KIND_MAX,
};

class EnemyManager:public GameObject
{
public:
	EnemyManager(SceneBase* _scene);
	~EnemyManager();

	void Update()override;
	void Draw()override;

	void CreateEnemy();
	void CreateEnemy(EnemyKind _kind);

	void ChangeGameState(GameManager::GAME_STATE _state);

private:

	std::list<EnemyBase*> enemyBase;

};

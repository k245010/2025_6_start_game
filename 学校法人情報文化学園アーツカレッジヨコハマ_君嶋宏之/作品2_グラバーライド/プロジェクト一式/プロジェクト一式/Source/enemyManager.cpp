#include "enemyManager.h"
#include "enemyA.h"
#include "enemyB.h"

EnemyManager::EnemyManager(SceneBase* _scene):GameObject(_scene)
{
	//CreateEnemy(ENEMY_1);
}

EnemyManager::~EnemyManager()
{
	for (auto e = enemyBase.begin(); e != enemyBase.end(); e++) {
		(*e)->DestroyMe();
	}
	enemyBase.clear();
}

void EnemyManager::Update()
{
	for (auto e = enemyBase.begin(); e != enemyBase.end();) {
		if (!(*e)->IsActive()) {
			(*e)->DestroyMe();
			e = enemyBase.erase(e);	
			
		}
		else {
			e++;
		}
	}
}

void EnemyManager::Draw()
{

}

void EnemyManager::CreateEnemy()
{
	enemyBase.push_back(GetScene()->CreateGameObject<EnemyBase>());
}

void EnemyManager::CreateEnemy(EnemyKind _kind)
{
	switch (_kind)
	{
	case ENEMY_1:
		enemyBase.push_back(GetScene()->CreateGameObject<EnemyA>());
		break;
	case ENEMY_2:
		enemyBase.push_back(GetScene()->CreateGameObject<EnemyB>());
		break;
	default:
		break;
	}
}

void EnemyManager::ChangeGameState(GameManager::GAME_STATE _state)
{
	for (auto e = enemyBase.begin(); e != enemyBase.end(); e++) {
		(*e)->GameStateChange(_state);
	}
}

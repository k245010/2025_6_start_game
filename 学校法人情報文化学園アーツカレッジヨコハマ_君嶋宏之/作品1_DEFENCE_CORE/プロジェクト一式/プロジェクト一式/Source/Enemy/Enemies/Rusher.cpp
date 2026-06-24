#include "Rusher.h"
#include "../../Common/Animator/Animator.h"
#include "../../Physics/Physics.h"
#include "../EnemyInfo.h"
#include "../EnemyManager/EnemyManager.h"
#include "../../Collision/Collisions/SphereCollision.h"
#include "../../Navigation/NavigationManager/NavigationManager.h"
#include <set>

namespace
{
	constexpr float MODEL_SIZE = 190.0f;

	//_ 移動 _//

	//constexpr float MOVE_SPEED						= 150.0f;					// 実際のゲームの敵の速度
	const VECTOR3 FRICTION							= VECTOR3(30.0f, 0.0f, 30.0f);	// 抵抗力
	constexpr float BASE_MOVE_SPEED_TO_ANI_RATE		= 1.0f / 250.0f;				// 歩行する速度に対しての歩きのアニメーションレート

	//_ 攻撃 _//

	//constexpr float ATTACK_REACH = MODEL_SIZE * 2;
	//constexpr float ATTACK_INTARVEL					= 2.0f;

}

Rusher::Rusher(const VECTOR3& _pos, const int& _hModel, EnemyManager* _enemyManager) : EnemyBase(_pos, _hModel,_enemyManager, ENEMY_KIND::RUSHER)
{
	/*
	enemyKind		= ENEMY_KIND::RUSHER;
	nodeUseKind		= (int)NODE_USE_KIND::NORMAL;
	navigationMode	= (int)NAVIGATION_MODE::IN_BLOCKED;

	transform.size	= VOne * MODEL_SIZE;
	modelSize		= transform.scale.x * transform.size.x;
	moveSpeed		= EnemyInfo::EnemyMoveSpeedData::GetMoveSpeed(ENEMY_KIND::RUSHER);
	attackIntarval	= EnemyInfo::EnemyAttackDamageData::GetTrapAttackInterval(ENEMY_KIND::RUSHER);
	attackReach		= ATTACK_REACH;
	hpMax			= HP_MAX;
	hp				= HP_MAX;*/

	baseMoveSpeedToAniRate = BASE_MOVE_SPEED_TO_ANI_RATE;


	std::string str = "data/models/chara/enemies/Rusher/";
	animation		= new Animator(hModel);
	animation->AddFileSettingAniIndex((int)ENEMY_STATE::STAY, str + "Anim_Idle.mv1", true, 0, 1.0f);
	animation->AddFileSettingAniIndex((int)ENEMY_STATE::MOVE, str + "Anim_Run.mv1",  true, 0, baseMoveSpeedToAniRate * enemyData.moveData.walkSpeed);

	// 攻撃する相手との当たり判定を設定する
	//attackTargetKindList.emplace_back((int)COLLISION_OBJECT_KIND::WALL_BLOCK);

	physics = new Physics(transform, velocity, FRICTION, moveSpeed);

	COLOR_F materialEmissiveColor = GetColorF(0.1f, 0.1f, 0.5f, 0.5f);	// モデルのマテリアルの発光カラー

	// 3Dモデルに含まれる0番目(体)のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 0, materialEmissiveColor);

	materialEmissiveColor = GetColorF(0.1f, 0.1f, 0.1f, 0.5f);
	// 3Dモデルに含まれる1番目(目)のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 1, materialEmissiveColor);
}

Rusher::~Rusher()
{
}

void Rusher::Move()
{
	EnemyBase::Move();
}

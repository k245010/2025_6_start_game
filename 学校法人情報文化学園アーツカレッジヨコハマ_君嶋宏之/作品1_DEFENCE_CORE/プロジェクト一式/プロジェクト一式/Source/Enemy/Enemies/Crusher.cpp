#include "Crusher.h"
#include "../../Navigation/Node/NavPointBase.h"
#include "../../Common/Animator/Animator.h"
#include "../../Physics/Physics.h"
#include "../EnemyInfo.h"
#include "../EnemyManager/EnemyManager.h"
#include "../../Navigation/Node/NodeData.h"
#include "../../Collision/Collisions/SphereCollision.h"

namespace
{
	constexpr float MODEL_SIZE = 190.0f;

	//_ 移動 _//

	//constexpr float MOVE_SPEED						= 150.0f;					// 実際のゲームの敵の速度
	const VECTOR3 FRICTION = VECTOR3(30.0f, 0.0f, 30.0f);	// 抵抗力
	constexpr float BASE_MOVE_SPEED_TO_ANI_RATE = 1.0f / 250.0f;				// 歩行する速度に対しての歩きのアニメーションレート

	//_ 攻撃 _//

	//constexpr float ATTACK_REACH = MODEL_SIZE * 2;
	//constexpr float ATTACK_INTARVEL					= 2.0f;

}


Crusher::Crusher(const VECTOR3& _pos, const int& _hModel, EnemyManager* _enemyManager) : EnemyBase(_pos, _hModel, _enemyManager, ENEMY_KIND::CRUSHER)
{
	baseMoveSpeedToAniRate = BASE_MOVE_SPEED_TO_ANI_RATE;

	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::SLIME_PATCH, 2.0f);

	animation = new Animator(hModel);
	std::string str = "data/models/chara/enemies/Crusher/";
	animation->AddFileSettingAniIndex((int)ENEMY_STATE::STAY, str + "Anim_Idle.mv1", true, 0, 1.0f);
	animation->AddFileSettingAniIndex((int)ENEMY_STATE::MOVE, str + "Anim_Run.mv1", true, 0, baseMoveSpeedToAniRate * enemyData.moveData.walkSpeed);

	physics = new Physics(transform, velocity, FRICTION, moveSpeed);

	COLOR_F materialEmissiveColor = GetColorF(0.15f, 0.15f, 0.15f, 1.0f);	// モデルのマテリアルの発光カラー

	// 3Dモデルに含まれる0番目(体)のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 0, materialEmissiveColor);

	materialEmissiveColor = GetColorF(0.1f, 0.1f, 0.1f, 0.5f);
	// 3Dモデルに含まれる1番目(目)のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 1, materialEmissiveColor);
}

Crusher::~Crusher()
{
}

void Crusher::Move()
{
	EnemyBase::Move();
}

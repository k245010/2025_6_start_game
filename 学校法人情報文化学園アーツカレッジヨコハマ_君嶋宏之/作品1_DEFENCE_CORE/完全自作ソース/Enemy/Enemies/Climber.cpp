#include "Climber.h"
#include "../../Navigation/Node/NavPointBase.h"
#include "../../Common/Animator/Animator.h"
#include "../../Physics/Physics.h"
#include "../EnemyInfo.h"
#include "../EnemyManager/EnemyManager.h"
#include "../../Navigation/Node/NodeData.h"
#include "../../Collision/Collisions/SphereCollision.h"

namespace
{
	constexpr float MODEL_SIZE						= 190.0f;

	//_ 移動 _//

	//constexpr float MOVE_SPEED						= 150.0f;					// 実際のゲームの敵の速度
	const VECTOR3 FRICTION							= VECTOR3(30.0f,0.0f,30.0f);	// 抵抗力
	constexpr float BASE_MOVE_SPEED_TO_ANI_RATE		= 1.0f / 250.0f;				// 歩行する速度に対しての歩きのアニメーションレート

	
	//_ 攻撃 _//

	//constexpr float ATTACK_REACH					= MODEL_SIZE * 2;
	//constexpr float ATTACK_INTARVEL					= 2.0f;

}

Climber::Climber(const VECTOR3& _pos, const int& _hModel, EnemyManager* _enemyManager) : EnemyBase(_pos,_hModel,_enemyManager,ENEMY_KIND::CLIMBER)
{
	/*nodeUseKind		= (int)NODE_USE_KIND::ALL;

	modelSize		= transform.scale.x * transform.size.x;
	moveSpeed		= EnemyInfo::EnemyMoveSpeedData::GetMoveSpeed(ENEMY_KIND::CLIMBER);
	attackIntarval	= EnemyInfo::EnemyAttackDamageData::GetTrapAttackInterval(ENEMY_KIND::CLIMBER);
	attackReach		= ATTACK_REACH;
	hpMax			= HP_MAX;
	hp				= HP_MAX;*/


	baseMoveSpeedToAniRate = BASE_MOVE_SPEED_TO_ANI_RATE;

	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::SLIME_PATCH, 2.0f);

	animation		= new Animator(hModel);
	std::string str = "data/models/chara/enemies/Climber/";
	animation->AddFileSettingAniIndex((int)ENEMY_STATE::STAY, str + "Anim_Idle.mv1", true, 0, 1.0f);
	animation->AddFileSettingAniIndex((int)ENEMY_STATE::MOVE, str + "Anim_Run.mv1",  true, 0, baseMoveSpeedToAniRate * enemyData.moveData.walkSpeed);

	physics = new Physics(transform, velocity, FRICTION, moveSpeed);

	COLOR_F materialEmissiveColor = GetColorF(0.5f, 0.0f, 0.0f, 0.2f);	// モデルのマテリアルの発光カラー

	// 3Dモデルに含まれる0番目(体)のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 0, materialEmissiveColor);

	materialEmissiveColor = GetColorF(0.1f, 0.1f, 0.1f, 0.5f);
	// 3Dモデルに含まれる1番目(目)のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 1, materialEmissiveColor);
}

Climber::~Climber()
{
}

void Climber::Update()
{
	EnemyBase::Update();

	// 壁を登っていたら
	if (isClim)
	{
		// 回転値を加算
		climingRotate.y += 10.0f * DegToRad;

		// 0~360度に収める
		if (climingRotate.y >= 360.0f * DegToRad)
			climingRotate.y -= 360.0f * DegToRad;

		// 移動速度を下げる
		moveSpeed = enemyData.moveData.climbSpeed;
	}
	else
	{
		// 回転値を戻す
		climingRotate	= VZero;

		// 移動速度を戻す
		moveSpeed		= enemyData.moveData.walkSpeed;
	}
}

void Climber::Draw()
{
	float rawRotationY   = transform.rotation.y;	// 元のY軸の回転値
	transform.rotation.y += climingRotate.y;

	EnemyBase::Draw();

	// Y軸の回転値を元に戻す
	transform.rotation.y = rawRotationY;
}

void Climber::Move()
{
	EnemyBase::Move();

	if (navPointNumbers.empty())
		return;

	const NODE_USE_KIND* navUseKind = nullptr;

	switch (moveState)
	{
	case EnemyBase::ENEMY_MOVE_STATE::SET:
		break;
	case EnemyBase::ENEMY_MOVE_STATE::MOVE:

		navUseKind = enemyManager->GetNavPointUseKind(navPointNumbers[navPointIndex]);

		if (navUseKind == nullptr)
			break;

		// 壁を登っていたら
		if (*navUseKind == NODE_USE_KIND::WALL)
			isClim = true;
		else
			isClim = false;
		break;
	default:
		break;
	}
}

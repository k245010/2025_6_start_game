#include "Bullet.h"
#include "../../../Collision/Collisions/SphereCollision.h"
#include "../../../Collision/Collisions/RayCollision.h"
#include "../../../Collision/CollisionFunction.h"
#include "../../../Physics/Physics.h"
#include "../../../Stage/StageManager/StageManager.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../Common/Effekseer/EffekseerObjectManager.h"

namespace
{
	constexpr float DEFAULT_BULLET_SIZE = 55.0f;
}

Bullet::Bullet(BulletInfo::BulletData&& _data, const VECTOR3& _dir, StageManager* _stageManager, const int& _hModel) : Object3D(_data.transform, _hModel),data(std::move(_data))
{
	assert(_stageManager != nullptr);
	stageManager		= _stageManager;

	active				= true;
	isHit				= false;
	transform.size		= VOne * DEFAULT_BULLET_SIZE;
	data.transform.size = VOne * data.size;

	lastPosition		= transform.position;

	sphereColl			= new SphereCollision(&data.transform, COLLISION_OBJECT_KIND::BULLET, "BULLET_SPHERE", [this](const CollisionHitInfoData& tr) {return HitBullet(tr); });

	// 当たる相手と、与えるダメージを設定
	for (const auto& target : data.targetDataList)
	{
		sphereColl->SetTargetTag(target.first, target.second);
	}

	/*sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::WALL_BLOCK);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::GROUND_BLOCK);*/

	//_ 弾が対象をフレーム間で当たらない現象を解消 _//

	/*rayColl = new RayCollision(&data.transform, COLLISION_OBJECT_KIND::BULLET,"BULLET_RAY", [this](const CollisionHitInfoData& tr) {return HitBullet(tr); });
	rayColl->SetTargetTag(data.damageData.targetKind, data.damageData.targetDamage);
	rayColl->SetTargetTag(COLLISION_OBJECT_KIND::WALL_BLOCK);
	rayColl->SetTargetTag(COLLISION_OBJECT_KIND::GROUND_BLOCK);*/

	velocity		= _dir * data.speed;
	maxDistance		= data.speed;		// 最大速度の設定
	initVelocity	= velocity;

	// 弾移動のために使用
	physics = new Physics(data.transform, velocity, data.friction, maxDistance, data.isGravity);
}

Bullet::Bullet(BulletInfo::BulletData&& _data, const VECTOR3& _dir, StageManager* _stageManager) : Bullet(std::move(_data), _dir, _stageManager, 0)
{
}

Bullet::~Bullet()
{
	if (sphereColl != nullptr)
	{
		delete sphereColl;
		sphereColl = nullptr;
	}

	if (rayColl != nullptr)
	{
		delete rayColl;
		rayColl = nullptr;
	}

	physics->DestroyMe();
}

void Bullet::Update()
{
	data.lifeTime -= Time::GameDeltaTime();

	if (data.lifeTime < 0.0f)
	{
		active = false;
		return;	// 非アクティブにしたので、以下の処理は行わないで return
	}

	Object3D::Update();
	
	/*if (rayColl == nullptr)
	{
		rayColl = new RayCollision(&data.transform, COLLISION_OBJECT_KIND::BULLET, "BULLET_RAY", [this](const CollisionHitInfoData& tr) {return HitBullet(tr); });
		rayColl->SetTargetTag(data.damageData.targetKind, data.damageData.targetDamage);
		rayColl->SetTargetTag(COLLISION_OBJECT_KIND::WALL_BLOCK);
		rayColl->SetTargetTag(COLLISION_OBJECT_KIND::GROUND_BLOCK);
	}*/

	//physics->Update();
	//data.velocity += initVelocity;

	float nowSize	= transform.position.Size();
	float oldSize	= lastPosition.Size();
	float error		= VSize(transform.position - lastPosition);

	
	//// 弾の移動
	//data.transform.position += moveVec;

	float raySize = 0.0f;

	raySize = velocity.Size();

	if (inLastPosition)
	{
		if (fabsf(error) > raySize)
		{
			raySize = error;
		}
	}

	if (error <= 0.0f)
		error = velocity.Size();

	VECTOR3 rayVec	= velocity.Normalize() * error;

	if (rayColl != nullptr)
	{
		// 弾の後ろ側にヴェクトルを設定　弾が対象をフレーム間で当たらない現象を解消するため
		rayColl->SetRayDirection(VZero, rayVec);
	}
	////_ 弾が対象をフレーム間で当たらない現象を解消 _//

	//// レイヴェクトル
	//VECTOR3 rayVec		= moveVec;
	//// レイを出す始点座標
	//VECTOR3 startPos	= data.transform.position - rayVec;
	//// レイの当たり判定用のトランスフォーム
	//Transform trans		= data.transform;
	//trans.position		= startPos;

	//rayColl->SetTransform(trans);
	//rayColl->SetRayVec(VZero, rayVec);

	lastPosition	= transform.position;
	inLastPosition	= true;

	float maxDisDelta = maxDistance * Time::GameDeltaTime();

	// 最大速度を超えていたらおさめる
	if (velocity.SquareSize() > (maxDisDelta * maxDisDelta))
	{
		VECTOR3 norm	= velocity.Normalize();
		velocity		= norm * maxDisDelta;
		//data.velocity.z = n.z * maxDisDelta;
	}
}

void Bullet::Draw()
{
	Object3D::Draw();

	if (rayColl != nullptr)
	{
		rayColl->Draw();
	}

	DrawSphere3D(data.transform.position, data.transform.size.x, 16, 0xffff00, 0xffffff, true);
}

bool Bullet::HitBullet(const CollisionHitInfoData& _targetData)
{
	for (const auto& summonObj : data.hitPosSummonStageObjectList)
	{
		if (summonObj.stageObjectKind == StageObjectData::STAGE_OBJECT_KIND::NONE)
			continue;	// 召喚するステージオブジェクトが設定されていない場合

		if (stageManager == nullptr)
			assert(false);
		
		Transform summonTransform	= summonObj.transform;
		summonTransform.position	= data.transform.position;

		// 着弾点へのステージオブジェクトの召喚
		stageManager->PutStageObject(summonTransform, summonObj.stageObjectKind);
	}

	for (const auto& sound : data.hitPosSoundIDList)
	{
		// 着弾時のサウンド再生
		SoundManager::PlaySE(sound);
	}

	// 着弾点のエフェクトが設定されていたら
	if (data.hitPosEffectKind != EffekseerObjectManager::EFFEKSEER_KIND::EF_NONE)
	{
		// 着弾点のエフェクトの再生
		EffekseerObjectManager::SetEffect(data.transform.position, data.hitPosEffectKind);
	}

	active	= false;
	isHit	= true;
	return true;
}

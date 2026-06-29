#include "CollisionBase.h"
#include "../CollisionManager/CollisionManager.h"
#include "SphereCollision.h"
#include "ObbCollision.h"
#include "RayCollision.h"
#include "ModelCollision.h"
#include <assert.h>

CollisionBase::CollisionBase(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName, std::function<bool(const CollisionHitInfoData&)> _func, bool _onHitDeleteMe)
{
	collTransform.trans				= _trans;
	collTransform.lastPosition		= VZero;

	isFirstCollided					= false;
	collisionHitInfoData.ownerName	= _ownerName;
	collisionHitInfoData.tag		= _tagMe;
	collisionHitInfoData.onDeleteMe	= _onHitDeleteMe;
	collisionHitInfoData.hitPointData.reset();

	collFunction					= _func;
	//onDeleteMe = false;
	isActive						= true;

	DoCollisionPushMe();

	CollisionManager::GetCollisionManagerInstance()->PushCollisionObject(this);
}

CollisionBase::~CollisionBase()
{
	if (!targetTag.empty())
		targetTag.clear();

	if (!hitTargetTag.empty())
		hitTargetTag.clear();

	if(!collisionHitInfoData.targetDamageData.empty())
		collisionHitInfoData.targetDamageData.clear();

	collFunction = nullptr;

	CollisionManager::GetCollisionManagerInstance()->PopCollisionObject(this);
}

void CollisionBase::Update()
{
	if (GetTagMe() == COLLISION_OBJECT_KIND::NONE)
		return;

	if (!isFirstCollided)
	{
		// コンストラクタで当たり判定を行うコンテナに自身を入れて当たり判定を行ったので、trueにする
		isFirstCollided = true;
	}
	
	// ラスト座標の更新
	collTransform.lastPosition = collTransform.trans->position;

	if (doCollisionPush && !isActive)
		DoCollisionDeleteMe();

	if (!doCollisionPush && isActive)
		DoCollisionPushMe();
}

bool CollisionBase::SetTargetTag(COLLISION_OBJECT_KIND _kind)
{
	// すでにリスト内に登録されてたら、returnする
	if (HasTargetTag(_kind))
		return false;

	// 当たり判定をする相手のCOLLISION_OBJECT_KINDを登録
	targetTag.push_back(_kind);
	return true;
}

bool CollisionBase::SetTargetTag(COLLISION_OBJECT_KIND _kind, float _damage)
{
	// 当たり判定をする相手のCOLLISION_OBJECT_KINDを登録して成功したら
	if(SetTargetTag(_kind))
	{
		for (auto& targetDamageData : collisionHitInfoData.targetDamageData)
		{
			// すでにCOLLISION_OBJECT_KINDが登録されていたら
			if (targetDamageData.targetKind == _kind)
			{
				// ダメージの代入
				targetDamageData.targetDamage = _damage;
				return true;	// ダメージの登録だできたので、return
			}
		}

		// ダメージデータの登録
		collisionHitInfoData.targetDamageData.emplace_back(CollisionHitDamageData(_kind, _damage));
		return true;
	}
	return false;
}

void CollisionBase::DeleteTargetTag(COLLISION_OBJECT_KIND _kind)
{
	if (HasTargetTag(_kind))
	{
		auto remove = std::remove(targetTag.begin(), targetTag.end(), _kind);

		targetTag.erase(remove, targetTag.end());
	}
}

bool CollisionBase::IsHit(CollisionBase* col)
{
	bool isHit = false;	// 当たったかのフラグ

	switch (col->GetCollisionKind())
	{
	case COLLISION_KIND::SPHERE:

		isHit = CollisionToSphere(dynamic_cast<SphereCollision*>(col));
		break;
	case COLLISION_KIND::OBB:

		isHit = CollisionToOBB(dynamic_cast<OBBCollision*>(col));
		break;
	case COLLISION_KIND::RAY:
		
		isHit = CollisionToRay(dynamic_cast<RayCollision*>(col));
		break;
	case COLLISION_KIND::MODEL:

		isHit = CollisionToModel(dynamic_cast<ModelCollision*>(col));
		break;
	default:
		assert(false);
		break;
	}

	if (isHit)
	{
		//_ お互いに当たった相手の名前を設定 _//

		collisionHitInfoData.targetOwnerName		= col->collisionHitInfoData.ownerName;
		col->collisionHitInfoData.targetOwnerName	= collisionHitInfoData.ownerName;
	}
	return isHit;
}

bool CollisionBase::HasTargetTag(COLLISION_OBJECT_KIND _kind)
{
	for (COLLISION_OBJECT_KIND target : targetTag)
	{
		if (target == _kind)
			return true;
	}
	return false;
}

bool CollisionBase::CollisionEvent(const CollisionHitInfoData& me, const CollisionHitInfoData& target)
{
	bool funcReturn = false;	// 関数の返り値

	//_ 当たり判定処理関数を実行 _//

	if (collFunction != nullptr)
	{
		// 関数の実行
		funcReturn = collFunction(target);

		//_ 当たったときの情報をリセットする _//

		collisionHitInfoData.hitPointData.reset();
		collisionHitInfoData.targetOwnerName = {};
	}
	return funcReturn;
}

void CollisionBase::DoCollisionPushMe()
{
	if (!doCollisionPush)
	{
		CollisionManager::GetCollisionManagerInstance()->PushDoCollisionObject(this);
		doCollisionPush = true;
	}
}

void CollisionBase::DoCollisionDeleteMe()
{
	if (doCollisionPush)
	{
		CollisionManager::GetCollisionManagerInstance()->PopDoCollisionObject(this);
		doCollisionPush = false;
	}
}




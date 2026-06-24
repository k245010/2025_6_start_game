#include "collisionBase.h"
#include "collisionManager.h"
#include "sphereCollision.h"
#include "obbCollision.h"
#include "rayCollision.h"
#include <assert.h>

CollisionBase::CollisionBase(Object* _obj)
{
	object = _obj;
	tagMe = COLLISION_OBJECT_KIND::NONE;

	lastPosition = VGet(500, 0, 0);
	isHitMe = false;

	CollisionManager::GetCollisionManagerObject()->CreateCollisionObject(this);
}

CollisionBase::~CollisionBase()
{
	if (!targetTag.empty())
	{
		targetTag.clear();
	}

	CollisionManager::GetCollisionManagerObject()->DeleteCollisionObject(this);
}

void CollisionBase::Update()
{
	if (GetTagMe() == COLLISION_OBJECT_KIND::NONE)
		return;

	if (GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_GRAVITY || GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_AIR_BARRIER)
	{
		DoCollisionPushMe();
		return;
	}

	VECTOR pos = GetPosition();
	VECTOR size = GetSize();
	VECTOR scale = GetObjectMe()->GetScale();
	if (GetTagMe() != COLLISION_OBJECT_KIND::ENEMY && GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR && GetTagMe() != COLLISION_OBJECT_KIND::HEAD && GetTagMe() != COLLISION_OBJECT_KIND::LAND)
	{
		size *= DEFAULT_CHECK_CAMERA_COLL_SIZE;
	}
	else
	{
		size = VDiv(size, size);
	}
	bool outCamera = CheckCameraViewClip_Box(pos - VGet(size.x * scale.x, size.y * scale.y, size.z * scale.z), pos + VGet(size.x * scale.x, size.y * scale.y, size.z * scale.z));
	
	if (!outCamera)
	{
		DoCollisionPushMe();
	}
	else
	{
		DoCollisionDeleteMe();
	}
}

void CollisionBase::SetTargetTag(COLLISION_OBJECT_KIND _kind)
{
	// ‚·‚Å‚ÉƒŠƒXƒg“à‚É“o˜^‚³‚ê‚Ä‚½‚çAreturn‚·‚é
	if (IsTargetTag(_kind))
	{
		return;
	}
	targetTag.push_back(_kind);
}

void CollisionBase::DeleteTargetTag(COLLISION_OBJECT_KIND _kind)
{
	if (IsTargetTag(_kind))
	{
		auto re = std::remove(targetTag.begin(), targetTag.end(), _kind);
	
		targetTag.erase(re, targetTag.end());
	}
}

void CollisionBase::DrawBase()
{
	DrawFormatString(10, 30, 0xff0000, "tagSize = %d", targetTag.size());
}

bool CollisionBase::IsHit(CollisionBase* col)
{
	switch (col->GetCollisionKind())
	{
	case COLLISION_KIND::SPHERE:
		return CollisionToSphere(dynamic_cast<SphereCollision*>(col));
		break;
	case COLLISION_KIND::OBB:
		return CollisionToOBB(dynamic_cast<OBBCollision*>(col));
		break;
	case COLLISION_KIND::RAY:
		return CollisionToRay(dynamic_cast<RayCollision*>(col));
		break;
	}
	return false;
}

bool CollisionBase::IsTargetTag(COLLISION_OBJECT_KIND _kind)
{
	for (COLLISION_OBJECT_KIND target : targetTag)
	{
		if (target == _kind)
			return true;
	}
	return false;
}

void CollisionBase::DoCollisionPushMe()
{
	if (!doCollisionPush)
	{
		CollisionManager::GetCollisionManagerObject()->PushDoCollisionObject(this);
		doCollisionPush = true;
	}
}

void CollisionBase::DoCollisionDeleteMe()
{
	if (doCollisionPush)
	{
		CollisionManager::GetCollisionManagerObject()->DeleteDoCollisionObject(this);
		doCollisionPush = false;
	}
}




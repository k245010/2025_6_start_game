#include "sphereCollision.h"
#include "obbCollision.h"
#include "stageObject.h"
#include "irondoor.h"

SphereCollision::SphereCollision(Object* obj) : CollisionBase(obj)
{
	kind = COLLISION_KIND::SPHERE;

	L = VGet(0.0f, 0.0f, 0.0f);
	thisPosition = VGet(0.0f, 0.0f, 0.0f);
	returnVec = VGet(0.0f, 0.0f, 0.0f);
	CollRadius = 0.0f;
}

SphereCollision::~SphereCollision()
{
}

void SphereCollision::Draw()
{
	// 当たり判定の描画
	if (object->GetObjectType() == (int)ObjectType::PLAYER || object->GetObjectType() == (int)ObjectType::WALK_ENEMY || object->GetObjectType() == (int)ObjectType::FLY_ENEMY)
	{
		DrawSphere3D(dynamic_cast<CharaBase*>(object)->GetLandPosition(), GetRadius() / 2, 16, 0xff0000, 0xffffff, false);
		DrawSphere3D(dynamic_cast<CharaBase*>(object)->GetCenterPosition(), GetRadius() / 1.8f, 16, 0x00ff00, 0xffffff, false);
	}

	if (object->GetObjectType() == (int)ObjectType::IRON_DOOR)
	{
		for (int i = 0;i < 2;i++)
			DrawSphere3D(dynamic_cast<IronDoor*>(object)->GetCollisionPosition((IronDoor::SphereCollisionType)i), (object->GetScale().x * object->GetSize().x) / 2, 16, 0xff0000, 0xffffff, false);
	}
}

bool SphereCollision::CollisionToSphere(SphereCollision* col)
{
	const float HIT_SIZE = GetRadius() + col->GetRadius();
	if (VSize(VSub(GetPosition(), col->GetPosition())) < HIT_SIZE)
	{
		return true;
	}
	return false;
}

bool SphereCollision::CollisionToOBB(OBBCollision* col)
{
	float len = CollisionToOBBLen(col);
	if (len < CollRadius)
	{
		// ゴールブロックと重力エリアブロックは押し返ししない
		if (col->GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_GOAL || col->GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_GRAVITY_AREA)
			return true;

		// バグなどで、プレイヤーがブロック内部にいたら、死亡させる
		if (GetTagMe() == COLLISION_OBJECT_KIND::PLAYER)
		{
			bool hit = false;
			float dis = VSize(VSub(GetPosition(), col->GetPosition()));

			if (dis < GetRadius() / 2)
				hit = true;

			for (int i = 0;i < 3;i++)
			{
				if (dis < GetVMem(col->GetLen(), i) / 2)
					hit = true;
			}

			if (hit)
			{
				dynamic_cast<Player*>(object)->Dead();
				return true;
			}
		}

		VECTOR v = VGet(0, 0, 0);
		bool wallvel = false;

		// velocityを取得
		if (GetTagMe() != COLLISION_OBJECT_KIND::LAND && GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_GRAVITY)
		{
			v = object->GetVelocity();
			
			if (GetTagMe() == COLLISION_OBJECT_KIND::PLAYER || GetTagMe() == COLLISION_OBJECT_KIND::ENEMY)
			{
				v = col->GetNearModelDirection(dynamic_cast<CharaBase*>(object)->GetCenterPosition());
				v = VScale(v, -1);
			}
			else if (GetTagMe() == COLLISION_OBJECT_KIND::PLAYER_ATTACK)
			{
				v = object->GetGravityVelocity();
			}
		}
		else
		{
			v = object->GetGravityVelocity();
		}

		// プレイヤー回転時は当たり判定しない //
		// キャラのvelocityが入る
		if (GetTagMe() != COLLISION_OBJECT_KIND::HEAD)
		{
			if (VAllSameF(v, 0.0f))
			{
				return false;
			}
		}

		VECTOR norm = VNorm(v);
		norm.z = 0;
		
		bool land = false;
		VECTOR vel = VGet(0.0f, 0.0f, 0.0f);

		// 壁ずりベクトルの残骸
		if (wallvel)
		{
			vel = GetVelocity();
			if (GetTagMe() == COLLISION_OBJECT_KIND::LAND)
			{
				vel = object->GetGravityVelocity();
				land = true;
			}

			if (col->GetTagMe() == COLLISION_OBJECT_KIND::BLOCK)
			{
				constexpr int MEM_MAX = 3;
				VECTOR lenMax = GetVMaxMemNormFabs(returnVec); 
				int m = 0;
				for (m;m < MEM_MAX;m++)
				{
					if (GetVMem(L, m) <= 0)
						continue;

					if (GetVMemIndexMax(lenMax) == m)
						break;
				}
				
				if (m == 0)
				{
					v = VCross(col->GetVDirection(1), col->GetVDirection(2));
				}
				else
				{
					v = VCross(col->GetVDirection(0), col->GetVDirection(MEM_MAX - m));
				}
			}
			float dirSize = -VDot(vel,v);
			VECTOR direction = VScale(v, dirSize);
			VECTOR re = VAdd(vel,VMult(direction, v));

			if (!land)
				object->CollisionForSetVelocity(re);
			else
				object->CollisionForSetGravityVelocity(re);
		}

		VECTOR sub = VScale(norm, CollRadius - fabsf(len));
		
		switch (GetTagMe())
		{
		case COLLISION_OBJECT_KIND::HEAD:

			// 頭に重力ブロックが当たっていたら、めり込んだ分プレイヤーの大きさを小さくする
			if (col->GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_GRAVITY || col->GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR)
			{
				VECTOR baseScale = object->GetScale();
				if (baseScale.y < 0)
					break;
				VECTOR drawSize = VMult(baseScale, object->GetSize());
				VECTOR subSize = VGet(0, drawSize.y - GetVFabs(sub).y, 0);
				VECTOR setScale = VDiv(subSize, object->GetSize());

				if (GetTagMe() == COLLISION_OBJECT_KIND::PLAYER)
				{
					if (setScale.y * object->GetSize().y < 0.5f)
						dynamic_cast<Player*>(object)->Dead();
				}

				dynamic_cast<CharaBase*>(object)->CollisionForSetScale(VGet(baseScale.x, setScale.y, baseScale.z));
			}
			break;
		default:
			if (object->GetObjectType() == (int)ObjectType::PLAYER || object->GetObjectType() == (int)ObjectType::WALK_ENEMY || object->GetObjectType() == (int)ObjectType::FLY_ENEMY)
			{
				dynamic_cast<CharaBase*>(object)->CollisionForSetCenterPosition(VSub(dynamic_cast<CharaBase*>(object)->GetCenterPosition(), sub));
			}
			else
			{
				object->CollisionForSetPosition(VSub(object->GetPosition(), sub));
			}
			break;
		}
		return true;
	}
	return false;
}

float SphereCollision::CollisionToOBBLen(OBBCollision* col)
{
	constexpr int COLL_VMEM_MAX = 2;

	if (col->GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_SPIKE)
	{
		VECTOR len = col->GetLen();
		L = col->GetLen(4.0f);
	}
	else
	{
		L = col->GetLen();
	}

	switch (GetTagMe())
	{
	case COLLISION_OBJECT_KIND::LAND:

		if (object->GetObjectType() == (int)ObjectType::IRON_DOOR)
		{
			thisPosition = dynamic_cast<IronDoor*>(object)->GetCollisionPosition((IronDoor::SphereCollisionType::BOTTOM));
			CollRadius = object->GetScale().x * object->GetSize().x / 2;
		}
		else
		{
			thisPosition = dynamic_cast<CharaBase*>(object)->GetLandPosition();
			CollRadius = GetRadius() / 2.1f;
		}
		break;
	case COLLISION_OBJECT_KIND::BLOCK_GRAVITY:

		thisPosition = object->GetPosition();
		CollRadius = GetRadius() / 1;
		break;
	case COLLISION_OBJECT_KIND::BLOCK_BUMPER:

		thisPosition = object->GetPosition();
		CollRadius = GetRadius() / 1.5f;
		break;
	case COLLISION_OBJECT_KIND::PLAYER:

		thisPosition = dynamic_cast<CharaBase*>(object)->GetCenterPosition();
		CollRadius = GetRadius() / 1.5f;
		break;
	case COLLISION_OBJECT_KIND::ENEMY:

		thisPosition = dynamic_cast<CharaBase*>(object)->GetCenterPosition();
		CollRadius = GetRadius();
		break;
	case COLLISION_OBJECT_KIND::PLAYER_ATTACK:

		thisPosition = dynamic_cast<CharaBase*>(object)->GetAttackPosition();
		CollRadius = GetRadius();
		if(col->GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_BROKEN)
			CollRadius = GetRadius() / 2;
		break;
	case COLLISION_OBJECT_KIND::ENEMY_ATTACK:
		
		thisPosition = dynamic_cast<CharaBase*>(object)->GetAttackPosition();
		CollRadius = GetRadius();
		break;
	case COLLISION_OBJECT_KIND::HEAD:

		if (object->GetObjectType() == (int)ObjectType::IRON_DOOR)
		{
			thisPosition = dynamic_cast<IronDoor*>(object)->GetCollisionPosition((IronDoor::SphereCollisionType::TOP));
			CollRadius = object->GetScale().x * object->GetSize().x / 2;
		}
		else
		{
			thisPosition = dynamic_cast<CharaBase*>(object)->GetHeadPosition();
			CollRadius = GetRadius() / 4;
		}
		break;
	default:	

		thisPosition = object->GetPosition();
		CollRadius = GetRadius();
		break;
	}

	returnVec = VGet(0.0f, 0.0f, 0.0f);
	const VECTOR DISTANCE = VSub(thisPosition, col->GetPosition());
	for (int m = 0;m < COLL_VMEM_MAX;m++)
	{
		if (GetVMem(L, m) <= 0)
			continue;

		float s = VDot(DISTANCE, col->GetVDirection(m)) / GetVMem(L, m);

		s = fabsf(s);
		if (s > 1)
		{
			returnVec = VAdd(returnVec, VScale(VScale(col->GetVDirection(m), (s - 1)), GetVMem(L, m)));
		}
	}
	return VSize(returnVec);
}

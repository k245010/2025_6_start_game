#include "collisionManager.h"
#include "collisionBase.h"
#include "obbCollision.h"
#include "function.h"
#include <assert.h>
#include <iterator>

CollisionManager* CollisionManager::object = nullptr;

CollisionManager::CollisionManager()
{
	obbInit = false;

	for (int i = 0;i < 10;i++)
	{
		draw[i] = false;
		keyPut[i] = false;
	}
}

CollisionManager::~CollisionManager()
{
}

CollisionManager* CollisionManager::GetCollisionManagerObject()
{
	if (object == nullptr)
	{
		object = new CollisionManager();
	}
	return object;
}

void CollisionManager::DeleteCollisionManagerObject()
{
	if (object != nullptr)
	{
		delete object;
		object = nullptr;
	}
}

void CollisionManager::Update()
{
#ifdef _DEBUG
	if (CheckHitKey(KEY_INPUT_I))
	{
		if (!keyPut[3])
		{
			keyPut[3] = true;
		}
	}
	else
	{
		keyPut[3] = false;
	}

	if (keyPut[3])
		return;
#endif // _DEBUG


	constexpr float DEATHRADIUS = 500.0f;
	constexpr float DEATHLINE = 300.0f;

	for (auto itr1 = doCollision.begin();itr1 != doCollision.end();itr1++)
	{
		CollisionBase* coll1 = *itr1;

		if (coll1->GetObjectMe() == nullptr)
			continue;

		if (coll1->GetObjectMe()->IsActive() == false)
			continue;

		if (coll1->GetTagMe() == COLLISION_OBJECT_KIND::NONE)
			continue;

		VECTOR coll1Pos = coll1->GetPosition();
		VECTOR coll1Size = coll1->GetSize();
		VECTOR coll1Scale = coll1->GetObjectMe()->GetScale();

		if (coll1->GetTagMe() != COLLISION_OBJECT_KIND::ENEMY && coll1->GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR && coll1->GetTagMe() != COLLISION_OBJECT_KIND::HEAD && coll1->GetTagMe() != COLLISION_OBJECT_KIND::LAND)
		{
			coll1Size *= DEFAULT_CHECK_CAMERA_COLL_SIZE;
		}
		else
		{
			coll1Size = VDiv(coll1Size, coll1Size);
		}

		// カメラ外に写っているかどうか
		bool outCamera = CheckCameraViewClip_Box(coll1Pos - VGet(coll1Size.x * coll1Scale.x, coll1Size.y * coll1Scale.y, coll1Size.z * coll1Scale.z), coll1Pos + VGet(coll1Size.x * coll1Scale.x, coll1Size.y * coll1Scale.y, coll1Size.z * coll1Scale.z));
		// 例外処理かどうか
		bool isException = IsExceptionCollisionObject(*coll1);

		if (outCamera && !isException)
		{
			coll1->SetVisionMove(!outCamera);
			continue;
		}
		if (coll1->GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_GRAVITY && coll1->GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_AIR_BARRIER)
		{
			coll1->SetVisionMove(!outCamera);
		}

		if (coll1->GetTagMe() == COLLISION_OBJECT_KIND::PLAYER_ATTACK)
		{
			if (dynamic_cast<Player*>(coll1->GetObjectMe())->GetPlayerAttackState() != Player::ATTACK1) 
				continue;
		}

		if (coll1->GetObjectMe()->GetObjectType() == (int)ObjectType::PLAYER)
		{
			if (dynamic_cast<CharaBase*>(coll1->GetObjectMe())->noHp() && dynamic_cast<Player*>(coll1->GetObjectMe())->GetLoseState() != Player::Die)
				continue;
		}

		for (auto itr2 = std::next(itr1);itr2 != doCollision.end();itr2++)
		{
			if (itr1 == itr2)
				continue;

			CollisionBase* coll2 = *itr2;

			if (coll2->GetObjectMe() == nullptr)
				continue;

			if (coll2->GetObjectMe()->IsActive() == false)
				continue;

			if (coll2->GetTagMe() == COLLISION_OBJECT_KIND::NONE)
				continue;

			VECTOR coll2Pos = coll2->GetPosition();
			VECTOR coll2Size = coll2->GetSize();
			VECTOR coll2Scale = coll2->GetObjectMe()->GetScale();

			if (coll2->GetTagMe() != COLLISION_OBJECT_KIND::ENEMY && coll2->GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR && coll2->GetTagMe() != COLLISION_OBJECT_KIND::HEAD && coll2->GetTagMe() != COLLISION_OBJECT_KIND::LAND)
			{
				coll2Size *= DEFAULT_CHECK_CAMERA_COLL_SIZE;
			}
			else
			{
				coll2Size = VDiv(coll2Size, coll2Size);
			}

			outCamera = CheckCameraViewClip_Box(coll2Pos - VGet(coll2Size.x * coll2Scale.x, coll2Size.y * coll2Scale.y, coll2Size.z * coll2Scale.z), coll2Pos + VGet(coll2Size.x * coll2Scale.x, coll2Size.y * coll2Scale.y, coll2Size.z * coll2Scale.z));
			isException = IsExceptionCollisionObject(*coll2);
			
			if (outCamera && !isException)
			{
				coll2->SetVisionMove(!outCamera);
				continue;
			}

			if (coll2->GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_GRAVITY && coll2->GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_AIR_BARRIER)
			{
				coll2->SetVisionMove(!outCamera);
			}

			if (coll2->GetTagMe() == COLLISION_OBJECT_KIND::PLAYER_ATTACK)
			{
				if (dynamic_cast<Player*>(coll2->GetObjectMe())->GetPlayerAttackState() != Player::ATTACK1)
					continue;
			}

			if (IsCollisionObjectKindSame(*coll1, COLLISION_OBJECT_KIND::HEAD))
			{
				if (coll1->GetObjectMe()->GetObjectType() == (int)ObjectType::PLAYER)
				{
					if (!dynamic_cast<Player*>(coll1->GetObjectMe())->GetGroundRand())
						continue;
				}
			}
			else if (IsCollisionObjectKindSame(*coll2, COLLISION_OBJECT_KIND::HEAD))
			{
				if (coll2->GetObjectMe()->GetObjectType() == (int)ObjectType::PLAYER)
				{
					if (!dynamic_cast<Player*>(coll2->GetObjectMe())->GetGroundRand()) 
						continue;
				}
			}

			if (coll2->GetObjectMe()->GetObjectType() == (int)ObjectType::PLAYER)
			{
				if (dynamic_cast<CharaBase*>(coll2->GetObjectMe())->noHp() && dynamic_cast<Player*>(coll2->GetObjectMe())->GetLoseState() != Player::Die) //これだとリスポーン時に問題あり
					continue;
			}

			if (!coll1->IsTargetTag(coll2->GetTagMe()))
				continue;
			if (!coll2->IsTargetTag(coll1->GetTagMe()))
				continue;

			if (IsCollisionObjectKindSame(*coll1, *coll2, COLLISION_OBJECT_KIND::BLOCK) && (IsCollisionObjectKindSame(*coll1, *coll2, COLLISION_OBJECT_KIND::PLAYER) || IsCollisionObjectKindSame(*coll1, *coll2, COLLISION_OBJECT_KIND::ENEMY)) || (IsCollisionObjectKindSame(*coll1, *coll2, COLLISION_OBJECT_KIND::PLAYER) && IsCollisionObjectKindSame(*coll1, *coll2, COLLISION_OBJECT_KIND::ENEMY)))
			{
				if (VSize(VSub(coll1->GetPosition(), coll2->GetPosition())) > DEATHRADIUS)
					continue;
			}

			coll1->ResetIsHitMe();
			coll2->ResetIsHitMe();

			if (coll1->IsHit(coll2))
			{
				coll1->CollisionEvent(*coll1, *coll2);
				coll2->CollisionEvent(*coll2, *coll1);
			}
		}
	}

	// 全てのCollisionインスタンスを回し、画面内のインスタンスだけ当たり判定を行うようにする
	for (auto itr = collision.begin();itr != collision.end();itr++)
	{
		CollisionBase* coll = *itr;

		// 最初にステージが読み込まれた時に、一回だけブロックの法線情報を初期化する
		if (coll->GetCollisionKind() == COLLISION_KIND::OBB)
		{
			if (!dynamic_cast<OBBCollision*>(coll)->GetModelDirectionDone())
				dynamic_cast<OBBCollision*>(coll)->ModelDirectionInit(coll->GetObjectMe()->GetHmodel());
		}

		// Updateを回し、自身が画面に写っていたら、当たり判定のfor文に入れる
		coll->Update();
	}

#ifdef _DEBUG
	if (CheckHitKey(KEY_INPUT_L))
	{
		if (!keyPut[0])
		{
			keyPut[0] = true;
			draw[0] = !draw[0];
		}
	}
	else
	{
		keyPut[0] = false;
	}

	if (CheckHitKey(KEY_INPUT_K))
	{
		if (!keyPut[1])
		{
			keyPut[1] = true;
			draw[1] = !draw[1];
		}
	}
	else
	{
		keyPut[1] = false;
	}
#endif // DEBUG
}

void CollisionManager::Draw()
{
#ifdef _DEBUG
	// 大まかな当たり判定の大きさを描画
	if (draw[0])
	{
		for (auto itr = collision.begin(); itr != collision.end(); itr++)
		{
			CollisionBase* coll = *itr;

			VECTOR collPos = coll->GetPosition();
			VECTOR collSize = coll->GetSize();
			VECTOR collScale = coll->GetObjectMe()->GetScale();
			if (coll->GetTagMe() != COLLISION_OBJECT_KIND::ENEMY && coll->GetTagMe() != COLLISION_OBJECT_KIND::HEAD && coll->GetTagMe() != COLLISION_OBJECT_KIND::LAND)
			{
				collSize *= 10;
			}
			else
			{
				collSize = VDiv(collSize, collSize);
			}
			bool outCamera = CheckCameraViewClip_Box(collPos - VGet(collSize.x * collScale.x, collSize.y * collScale.y, collSize.z * collScale.z), collPos + VGet(collSize.x * collScale.x, collSize.y * collScale.y, collSize.z * collScale.z));
			if (outCamera)
				continue;
			coll->Draw();
			coll->DrawBase();
		}
	}

	if (draw[1])
	{
		DrawFormatString(100, 300, 0xff00ff, "collisionListSize = %d", collision.size());
		DrawFormatString(100, 400, 0xff00ff, "doCollisionListSize = %d", doCollision.size());
	}
#endif // DEBUG
}

void CollisionManager::CreateCollisionObject(CollisionBase* _obj)
{
	collision.push_back(_obj);
}

void CollisionManager::PushDoCollisionObject(CollisionBase* _obj)
{
	doCollision.push_back(_obj);
}

void CollisionManager::DeleteDoCollisionObject(CollisionBase* _obj)
{
	for (auto itr = doCollision.begin();itr != doCollision.end();)
	{
		if (*itr == _obj)
		{
			(*itr) = nullptr;
			itr = doCollision.erase(itr);
			break;
		}
		else {
			++itr;
		}
	}
}

void CollisionManager::DeleteCollisionObject(CollisionBase* _obj)
{
	DeleteDoCollisionObject(_obj);
	for (auto itr = collision.begin();itr != collision.end();)
	{
		if (*itr == _obj)
		{
			(*itr) = nullptr;
			itr = collision.erase(itr);
			break;
		}
		else {
			++itr;
		}
	}
}

bool CollisionManager::IsExceptionCollisionObject(const CollisionBase& _obj)
{
	if (_obj.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_AIR_BARRIER || _obj.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_GRAVITY || _obj.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR || _obj.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_GRAVITY_AREA || _obj.GetObjectMe()->GetObjectType() == (int)ObjectType::WALK_ENEMY)
	{
		return true;
	}
	return false;
}

bool CollisionManager::IsCollisionObjectKindSame(const CollisionBase& _obj, COLLISION_OBJECT_KIND _kind)
{
	if (_obj.GetTagMe() == _kind)
	{
		return true;
	}
	return false;
}

bool CollisionManager::IsCollisionObjectKindSame(const CollisionBase& _obj1, const CollisionBase& _obj2,COLLISION_OBJECT_KIND _kind)
{
	if (IsCollisionObjectKindSame(_obj1, _kind) || IsCollisionObjectKindSame(_obj2, _kind))
	{
		return true;
	}
	return false;
}

CollisionBase* CollisionManager::GetCollisionKindSame(CollisionBase* _obj1, CollisionBase* _obj2, COLLISION_OBJECT_KIND _kind)
{
	if (IsCollisionObjectKindSame(*_obj1, _kind))
	{
		return _obj1;
	}
	else if (IsCollisionObjectKindSame(*_obj2, _kind))
	{
		return _obj2;
	}
	return nullptr;
}



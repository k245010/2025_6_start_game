#include "CollisionManager.h"
#include "../Collisions/CollisionBase.h"
#include "../Collisions/ObbCollision.h"
#include <unordered_set>
#include <iterator>
#include <assert.h>
#include "../../../Library/DebugNew.h"

CollisionManager* CollisionManager::object = nullptr;

CollisionManager::CollisionManager()
{
	for (int i = 0;i < 10;i++)
	{
		draw[i]		= false;
		keyPut[i]	= false;
	}
}

CollisionManager::~CollisionManager()
{
	doCollision.clear();
	deleteCollision.clear();
}

CollisionManager* CollisionManager::GetCollisionManagerInstance()
{
	if (object == nullptr)
	{
		object = new CollisionManager();
	}
	return object;
}

void CollisionManager::DeleteCollisionManagerInstance()
{
	if (object != nullptr)
	{
		delete object;
		object = nullptr;
	}
}

void CollisionManager::Update()
{
	std::unordered_map<CollisionBase*, std::unordered_set<COLLISION_OBJECT_KIND>> hitObj;
	std::unordered_set<CollisionBase*> initHitObj;
	std::unordered_set<CollisionBase*> deleteCheck; // deleteCollisionに同じイテレータが入らないようにする

	for (auto itr1 = doCollision.begin();itr1 != doCollision.end();itr1++)
	{
		if (*itr1 == nullptr)
			continue;

		CollisionBase* coll1 = *itr1;
		
		if (coll1 == nullptr)
			continue;

		if (coll1->GetTagMe() == COLLISION_OBJECT_KIND::NONE)
		{
			// 初めて追加されたら
			if (deleteCheck.insert(*itr1).second)
			{
				// 削除コンテナにpush
				deleteCollision.insert(deleteCollision.begin(), itr1);
			}
			continue;
		}

		if (!coll1->IsActive())
			continue;

		// 削除するインスタンスイテレータに登録されていたら
		if (HasDeleteCollisionItr(itr1))
			continue;

		for (auto itr2 = std::next(itr1);itr2 != doCollision.end();itr2++)
		{
			if (*itr2 == nullptr)
				continue;

			if (itr1 == itr2)
				continue;

			CollisionBase* coll2 = *itr2;


			if (coll2 == nullptr)
				continue;

			if (!coll2->IsActive())
				continue;
			
			// バウンディング処理 --------------------------

			constexpr float COLL_SKIP_SQUARE_DISTANCE = 1500.0f * 1500.0f;	// RayやModelを除いたOBBやSphereとの当たり判定のバウンディングボックス距離の二乗
			
			// どちらもバウンディング対象なら
			if(IsBoundingCollisionKind(coll1->GetCollisionKind()) && IsBoundingCollisionKind(coll2->GetCollisionKind()))
			{ 
				// お互いの距離をみて、バウンディングボックス外の場合
				if (VSquareSize(coll1->GetPosition() - coll2->GetPosition()) >= COLL_SKIP_SQUARE_DISTANCE)
					continue;	// 距離が離れているので、以下の当たり判定をしない
			}

			// --------------------------------------------

			if (coll2->GetTagMe() == COLLISION_OBJECT_KIND::NONE)
			{
				// 初めて追加されたら
				if (deleteCheck.insert(*itr2).second)
				{
					// 削除コンテナにpush
					deleteCollision.insert(deleteCollision.begin(), itr2);
				}
				continue;
			}

			// 削除するインスタンスイテレータに登録されていたら
			if (HasDeleteCollisionItr(itr2))
				continue;
			// 削除するインスタンスイテレータに登録されていたら
			if (HasDeleteCollisionItr(itr1))
				continue;

			// まだ初期化されてなかったら
			if (initHitObj.insert(*itr1).second && hitObj[coll1].empty())
			{
				// 当たったTagの初期化
				coll1->SetHitForTargetObjectKind(hitObj[coll1]);
			}
			
			if (initHitObj.insert(*itr2).second && hitObj[coll2].empty())
			{
				// 当たったTagの初期化
				coll2->SetHitForTargetObjectKind(hitObj[coll2]);
			}

			// 相手が自身との当たり判定を登録してなかったとき //
			if (!coll1->HasTargetTag(coll2->GetTagMe()))
				continue;
			if (!coll2->HasTargetTag(coll1->GetTagMe()))
				continue;

			if (coll1->IsHit(coll2))
			{
				// お互いに当たった相手のタグをpush //
				hitObj[coll1].emplace(coll2->GetTagMe());
				hitObj[coll2].emplace(coll1->GetTagMe());

				// 当たった相手の当たり判定オブジェクトの種類を保存するコンテナの設定 //
				coll1->SetHitForTargetObjectKind(hitObj[coll1]);
				coll2->SetHitForTargetObjectKind(hitObj[coll2]);

				// 当たったときの処理を実行し、自身のインスタンスを削除するとき
				if (coll1->CollisionEvent(coll1->GetCollisionHitInfoData(), coll2->GetCollisionHitInfoData()) || coll1->IsDeleteMe())
				{
					// まだ削除候補でなく自身をpushできたら
					if (deleteCheck.insert(*itr1).second)
					{
						// 削除コンテナにpush
						deleteCollision.insert(deleteCollision.begin(), itr1);
					}
				}

				// 当たったときの処理を実行し、自身のインスタンスを削除するとき
				if (coll2->CollisionEvent(coll2->GetCollisionHitInfoData(), coll1->GetCollisionHitInfoData()) || coll2->IsDeleteMe())
				{
					// まだ削除候補でなく自身をpushできたら
					if (deleteCheck.insert(*itr2).second)
					{
						// 削除コンテナにpush
						deleteCollision.insert(deleteCollision.begin(), itr2);
					}
				}
			}
		}
	}

	// doCollisionからdeleteCollisionに登録されたインスタンスを削除
	DeleteCollisionListDelete(doCollision);

	std::list<CollisionBase*> deleteList;

	// 全てのCollisionインスタンスを回し、削除候補をコンテナにpush
	for (auto itr = collision.begin();itr != collision.end();itr++)
	{
		CollisionBase& coll = **itr;
		coll.Update();

		if (coll.IsDeleteMe())
		{
			// 削除候補コンテナにpush
			deleteList.emplace_back(*itr);
		}
	}

	//DeleteCollisionListDelete(deleteList);

	// DeleteCollisionListDeleteにまとめようとしたが、どのコンテナiteratorなのか判断できなかったのでここで削除
	for (auto itr = deleteList.begin();itr != deleteList.end();itr++)
	{
		CollisionBase* coll = *itr;
		PopCollisionObject(coll);
	}

#ifdef _DEBUG
	if (CheckHitKey(KEY_INPUT_O))
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

	if (CheckHitKey(KEY_INPUT_U))
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

			VECTOR collPos		= coll->GetPosition();
			VECTOR collSize		= coll->GetSize();
			VECTOR collScale	= coll->GetScale();
			collSize			= VDiv(collSize, collSize);
			
			// 当たり判定のインスタンスの座標を見て、カメラ外か確認する
			bool outCamera		= CheckCameraViewClip_Box(collPos - VGet(collSize.x * collScale.x, collSize.y * collScale.y, collSize.z * collScale.z), collPos + VGet(collSize.x * collScale.x, collSize.y * collScale.y, collSize.z * collScale.z));
			
			// カメラ外だったら
			if (outCamera)
				continue;	// カメラ外は描画しないので、continue

			// 当たり判定の描画
			coll->Draw();
		}
	}

	if (draw[1])
	{
		// 全ての当たり判定のインスタンス数
		DrawFormatString(100, 300, 0xff00ff, "collisionListSize = %d", collision.size());
		// 現在当たり判定をするインスタンス数
		DrawFormatString(100, 400, 0xff00ff, "doCollisionListSize = %d", doCollision.size());
	}
#endif // DEBUG
}

void CollisionManager::PushCollisionObject(CollisionBase* _obj)
{
	collision.emplace_back(_obj);
}

void CollisionManager::PushDoCollisionObject(CollisionBase* _obj)
{
	doCollision.emplace_back(_obj);
}

void CollisionManager::PopDoCollisionObject(CollisionBase* _obj)
{
	for (auto itr = doCollision.begin();itr != doCollision.end();)
	{
		if (*itr == _obj)
		{
			(*itr) = nullptr;
			itr = doCollision.erase(itr);
			break;
		}
		else 
		{
			++itr;
		}
	}
}

void CollisionManager::PopCollisionObject(CollisionBase* _obj)
{
	PopDoCollisionObject(_obj);
	for (auto itr = collision.begin();itr != collision.end();)
	{
		if (*itr == _obj)
		{
			(*itr) = nullptr;
			itr = collision.erase(itr);
			break;
		}
		else 
		{
			++itr;
		}
	}
}

void CollisionManager::DeleteCollisionListDelete(std::list<CollisionBase*>& _list, bool _deleteMeList)
{
	for (auto itr = deleteCollision.begin();itr != deleteCollision.end();)
	{
		if (*(*itr) != nullptr)
		{
			if (_deleteMeList)
			{
				delete* (*itr);
			}
			*(*itr) = nullptr;
		}
		_list.erase(*itr);
		itr = deleteCollision.erase(itr);
	}
	deleteCollision.clear();
}

bool CollisionManager::HasDeleteCollisionItr(const std::list<CollisionBase*>::iterator _itr)
{
	// 引数のイテレータを保持していたら
	if (std::find(deleteCollision.begin(), deleteCollision.end(), _itr) != deleteCollision.end())
		return true;
	return false;
}

bool CollisionManager::IsBoundingCollisionKind(const COLLISION_KIND& _collisionKind)
{
	// true:バウンディング対象 / false:バウンディング対象ならtrue
	return (_collisionKind == COLLISION_KIND::SPHERE || _collisionKind == COLLISION_KIND::OBB);
}





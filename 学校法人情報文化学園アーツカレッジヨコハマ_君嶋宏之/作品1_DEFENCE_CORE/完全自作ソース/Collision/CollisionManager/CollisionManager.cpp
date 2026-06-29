#include "CollisionManager.h"
#include "../Collisions/CollisionBase.h"
#include "../Collisions/ObbCollision.h"
#include <unordered_set>
#include <iterator>
#include <assert.h>
#include "../../../Library/DebugNew.h"



CollisionManager* CollisionManager::object = nullptr;

namespace
{
#if USE_NEAREST_PAIR_CONTAINER
	struct PairHash
	{
		size_t operator()(const std::pair<CollisionBase*, CollisionBase*>& _p) const
		{
			size_t h1 = std::hash<CollisionBase*>{}(_p.first);
			size_t h2 = std::hash<CollisionBase*>{}(_p.second);

			// ハッシュ値を合成
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
#else
	struct PairHash
	{
		size_t operator()(const std::pair<CollisionBase*, COLLISION_OBJECT_KIND>& p) const
		{
			size_t h1 = std::hash<CollisionBase*>{}(p.first);
			size_t h2 = std::hash<COLLISION_OBJECT_KIND>{}(p.second);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
#endif
}

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
	std::unordered_map<CollisionBase*, std::unordered_set<COLLISION_OBJECT_KIND>> hitObjList;	// 当たった相手を保存するコンテナ
	std::unordered_set<CollisionBase*> initHitObjList;	// 当たった相手を保存したか判断するコンテナ
	std::unordered_set<CollisionBase*> deleteCheckList; // deleteCollisionに同じイテレータが入らないようにする削除チェックするコンテナ

#if USE_NEAREST_PAIR_CONTAINER
	struct SaveNearestHitData
	{
		std::list<CollisionBase*>::iterator itr1;
		std::list<CollisionBase*>::iterator itr2;
		CollisionHitPointData itr1HitPointData;		// itr1の当たった場所の情報構造体

		SaveNearestHitData(const std::list<CollisionBase*>::iterator _itr1, const std::list<CollisionBase*>::iterator _itr2, const CollisionHitPointData& _itr1HitPointData) : itr1(_itr1), itr2(_itr2), itr1HitPointData(_itr1HitPointData) {}
	};

	std::unordered_map<std::pair<CollisionBase*, CollisionBase*>, SaveNearestHitData, PairHash> nearestHitPairList;
#else
	struct SaveNearestHitData
	{
		CollisionBase* target;						// 当たり判定イベントをする相手
		std::list<CollisionBase*>::iterator itr1;	// 自身のイテレーター
		std::list<CollisionBase*>::iterator itr2;	// 相手のイテレーター
		CollisionHitPointData meHitPointData;		// 自身の当たった場所の情報構造体

		SaveNearestHitData(CollisionBase* _target, const std::list<CollisionBase*>::iterator _itr1, const std::list<CollisionBase*>::iterator _itr2, const CollisionHitPointData& _meHitPointData) : target(_target), itr1(_itr1), itr2(_itr2), meHitPointData(_meHitPointData) {}
	};

	std::unordered_map<std::pair<CollisionBase*, COLLISION_OBJECT_KIND>, SaveNearestHitData, PairHash> nearestHitDataList;	// keyの ( 自身のCollisionBase*, 最も近い距離で当たったCOLLISION_OBJECT_KIND ) 最も近い距離で当たったCollisionBase*をvalueで持つコンテナ
#endif


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
			if (deleteCheckList.insert(*itr1).second)
			{
				// 削除コンテナにpush
				deleteCollision.insert(deleteCollision.begin(), itr1);
			}
			continue;	// 当たり判定できないオブジェクトなので continue
		}

		if (!coll1->IsActive())
			continue;	// nullptrなので continue

		if (HasDeleteCollisionItr(itr1))
			continue;	// 削除するインスタンスイテレータに登録されていたら continue

		// まだinitHitObjListに初期化されてなかったら
		if (initHitObjList.insert(*itr1).second && hitObjList[coll1].empty())
		{
			// 当たったTagの初期化
			coll1->SetHitForTargetObjectKind(hitObjList[coll1]);
		}

		for (auto itr2 = std::next(itr1);itr2 != doCollision.end();itr2++)
		{
			if (*itr2 == nullptr)
				continue;

			CollisionBase* coll2 = *itr2;

			if (coll2 == nullptr)
				continue;

			if (!coll2->IsActive())
				continue;	// 非アクティブ状態なので continue

			// バウンディング処理 --------------------------

			constexpr float COLL_SKIP_SQUARE_DISTANCE = 1500.0f * 1500.0f;	// RayやModelを除いたOBBやSphereとの当たり判定のバウンディングボックス距離の二乗
			
			// どちらもバウンディング対象なら
			if(IsBoundingCollisionKind(coll1->GetCollisionKind()) && IsBoundingCollisionKind(coll2->GetCollisionKind()))
			{ 
				// お互いの距離をみて、バウンディングボックス外の場合
				if (VSquareSize(coll1->GetPosition() - coll2->GetPosition()) >= COLL_SKIP_SQUARE_DISTANCE)
					continue;	// 距離が離れているので、以下の当たり判定をしないで continue
			}

			// --------------------------------------------

			if (coll2->GetTagMe() == COLLISION_OBJECT_KIND::NONE)
			{
				// 初めて追加されたら
				if (deleteCheckList.insert(*itr2).second)
				{
					// 削除コンテナにpush
					deleteCollision.insert(deleteCollision.begin(), itr2);
				}
				continue;	// 当たり判定できないオブジェクトなので continue
			}

			if (HasDeleteCollisionItr(itr2))
				continue;	// 削除するインスタンスイテレータに登録されていたら continue
			
			// まだinitHitObjListに初期化されてなかったら
			if (initHitObjList.insert(*itr2).second && hitObjList[coll2].empty())
			{
				// 当たったTagの初期化
				coll2->SetHitForTargetObjectKind(hitObjList[coll2]);
			}

			// 相手が自身との当たり判定を登録してなかったとき continue //
			if (!coll1->HasTargetTag(coll2->GetTagMe()))
				continue;
			if (!coll2->HasTargetTag(coll1->GetTagMe()))
				continue;

			if (coll1->IsHit(coll2))
			{
				if (coll1->GetUseNearestCollisionFlag() || coll2->GetUseNearestCollisionFlag())
				{
#if USE_NEAREST_PAIR_CONTAINER

					std::unordered_map<std::pair<CollisionBase*, CollisionBase*>, SaveNearestHitData, PairHash>::iterator iterator = nearestHitPairList.find(GetMakeKey(coll1, coll2));	// イテレーター

					// キー存在しないとき
					if (iterator == nearestHitPairList.end())
					{
						const std::optional<CollisionHitPointData>& coll1HitPointData = coll1->GetCollisionHitInfoData().hitPointData;	// 当たった場所の構造体

						// 当たった情報が入っているとき
						if (coll1HitPointData.has_value())
						{
							nearestHitPairList.insert(std::make_pair(GetMakeKey(coll1, coll2), SaveNearestHitData(itr1, itr2, coll1HitPointData.value())));
						}
					}
					else // キーが存在するとき
					{
						const CollisionHitPointData& itrHitPointData				= iterator->second.itr1HitPointData;				// イテレーターのitr1の当たった場所の情報構造体
						const std::optional<CollisionHitPointData>& meHitPointData	= coll1->GetCollisionHitInfoData().hitPointData;	// coll1自身の当たった場所の情報構造体
						const VECTOR3 mePosition = coll1->GetPosition();	// coll1自身の座標

						// 当たった情報が入っているとき
						if (meHitPointData.has_value())
						{
							VECTOR3 itrNearestPos	= itrHitPointData.GetNearestHitPosition((*iterator->second.itr1)->GetPosition());	// イテレーターのitr1の最も近い当たった座標
							VECTOR3 meNearestPos	= meHitPointData.value().GetNearestHitPosition(mePosition);							// coll1自身の最も近い当たった座標
							
							// coll1自身の当たった座標の方が、nearestHitPairListに登録されていた、過去のcoll1自身の当たった座標よりもmePositionに近かったら
							if (VSquareSize(mePosition - meNearestPos) < VSquareSize(mePosition - itrNearestPos))
							{
								// 過去の情報を削除
								iterator = nearestHitPairList.erase(iterator);
								

								// 新しい情報をpush
								nearestHitPairList.insert(std::make_pair(GetMakeKey(coll1,coll2), SaveNearestHitData(itr1, itr2, meHitPointData.value())));
							}
						}
					}
#else

					PairData pair = GetPair(coll1, coll2);	// 自身と相手のペア

					auto iterator = nearestHitDataList.find(std::make_pair(pair.me, pair.target->GetCollisionHitInfoData().tag));

					const std::optional<CollisionHitPointData>& meHitPointData = pair.me->GetCollisionHitInfoData().hitPointData;	// 自身の当たった場所の構造体
					
					// キーが存在しなかったら
					if (iterator == nearestHitDataList.end())
					{
						// 当たった情報が入っているとき
						if (meHitPointData.has_value())
						{
							// 新しいペアをpush
							nearestHitDataList.insert(std::make_pair(std::make_pair(pair.me, pair.target->GetCollisionHitInfoData().tag), SaveNearestHitData(pair.target, itr1, itr2, meHitPointData.value())));
						}
					}
					else // キーが存在したら
					{
						const std::optional<CollisionHitPointData>& hasContainerTargetHitPointData		= iterator->second.target->GetCollisionHitInfoData().hitPointData;		// コンテナ内のtargetの当たった場所の情報構造体
						const std::optional<CollisionHitPointData>& hasNotConatinerTargetHitPointData	= pair.target->GetCollisionHitInfoData().hitPointData;					// targetの当たった場所の情報構造体
						const VECTOR3 mePosition = pair.me->GetPosition();	// coll1自身の座標

						// お互いに当たった情報が入っているとき
						if (hasContainerTargetHitPointData.has_value() && hasNotConatinerTargetHitPointData.has_value())
						{
							VECTOR3 hasConNearestPos = hasContainerTargetHitPointData.value().GetNearestHitPosition(mePosition);		// コンテナ内のtargetの最も近い当たった座標
							VECTOR3 hasNotConNearestPos = hasNotConatinerTargetHitPointData.value().GetNearestHitPosition(mePosition);	// targetの自身から最も近い当たった座標

							// コンテナ内に登録されていないtargetの当たった座標の方が、コンテナ内に登録されているtargetの当たった座標よりもmePositionに近かったら
							if (VSquareSize(mePosition - hasNotConNearestPos) < VSquareSize(mePosition - hasConNearestPos))
							{
								// 過去の情報を削除
								//iterator = nearestHitPairList.erase(iterator);

								// 情報の更新
								iterator->second = SaveNearestHitData(pair.target, itr1, itr2, meHitPointData.value());

								// 新しい情報をpush
								//nearestHitPairList.insert(std::make_pair(GetMakeKey(coll1, coll2), SaveNearestHitData(itr1, itr2, meHitPointData.value())));
							}
						}
					}
#endif
				}
				else
				{
					HitEvent(hitObjList, coll1, coll2, deleteCheckList, itr1, itr2);
				}
			}
		}
	}

#if USE_NEAREST_PAIR_CONTAINER
	for (const auto& mostNearHitPair : nearestHitPairList)
	{
		CollisionBase* coll1 = mostNearHitPair.first.first;
		CollisionBase* coll2 = mostNearHitPair.first.second;

		std::list<CollisionBase*>::iterator itr1 = mostNearHitPair.second.itr1;
		std::list<CollisionBase*>::iterator itr2 = mostNearHitPair.second.itr2;

		HitEvent(hitObjList, coll1, coll2, deleteCheckList, itr1, itr2);
	}
#else
	for (const auto& nearestHitData : nearestHitDataList)
	{
		CollisionBase* coll1 = nearestHitData.first.first;
		CollisionBase* coll2 = nearestHitData.second.target;

		std::list<CollisionBase*>::iterator itr1 = nearestHitData.second.itr1;
		std::list<CollisionBase*>::iterator itr2 = nearestHitData.second.itr2;

		HitEvent(hitObjList, coll1, coll2, deleteCheckList, itr1, itr2);
	}
#endif

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

#if USE_NEAREST_PAIR_CONTAINER
std::pair<CollisionBase*, CollisionBase*> CollisionManager::GetMakeKey(CollisionBase* _coll1, CollisionBase* _coll2)
{
	if (_coll1 > _coll2)
	{
		std::swap(_coll1,_coll2);
	}
	return {_coll1,_coll2};
}
#else
CollisionManager::PairData CollisionManager::GetPair(CollisionBase* _coll1, CollisionBase* _coll2)
{
	if (_coll1 > _coll2)
	{
		std::swap(_coll1, _coll2);
	}
	return PairData(_coll1,_coll2);
}
#endif
void CollisionManager::HitEvent(std::unordered_map<CollisionBase*, std::unordered_set<COLLISION_OBJECT_KIND>>& _hitObjList, CollisionBase*& _coll1, CollisionBase*& _coll2, std::unordered_set<CollisionBase*>& _deleteCheckList, std::list<CollisionBase*>::iterator& _itr1, std::list<CollisionBase*>::iterator& _itr2)
{
	// お互いに当たった相手のタグをpush //
	_hitObjList[_coll1].emplace(_coll2->GetTagMe());
	_hitObjList[_coll2].emplace(_coll1->GetTagMe());

	// 当たった相手の当たり判定オブジェクトの種類を保存するコンテナの設定 //
	_coll1->SetHitForTargetObjectKind(_hitObjList[_coll1]);
	_coll2->SetHitForTargetObjectKind(_hitObjList[_coll2]);

	// 当たったときの処理を実行し、自身のインスタンスを削除するとき
	if (_coll1->CollisionEvent(_coll1->GetCollisionHitInfoData(), _coll2->GetCollisionHitInfoData()) || _coll1->IsDeleteMe())
	{
		// まだ削除候補でなく自身をpushできたら
		if (_deleteCheckList.insert(*_itr1).second)
		{
			// 削除コンテナにpush
			deleteCollision.insert(deleteCollision.begin(), _itr1);
		}
	}

	// 当たったときの処理を実行し、自身のインスタンスを削除するとき
	if (_coll2->CollisionEvent(_coll2->GetCollisionHitInfoData(), _coll1->GetCollisionHitInfoData()) || _coll2->IsDeleteMe())
	{
		// まだ削除候補でなく自身をpushできたら
		if (_deleteCheckList.insert(*_itr2).second)
		{
			// 削除コンテナにpush
			deleteCollision.insert(deleteCollision.begin(), _itr2);
		}
	}
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
			// 削除するとき
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





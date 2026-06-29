#pragma once
#include <list>
#include <Dxlib.h>
#include <unordered_map>
#include <set>
#include "../Collisions/CollisionBase.h"
#include "../CollisionFunction.h"

#define USE_NEAREST_PAIR_CONTAINER (0)

class CollisionBase;
/// <summary>
/// 当たり判定を総括するクラス
/// </summary>
class CollisionManager
{
public:

	/// <summary>
	/// 静的メソッドでthisを返す
	/// </summary>
	/// <returns> this </returns>
	static CollisionManager* GetCollisionManagerInstance();
	
	/// <summary>
	/// 静的メソッドでCollisionManagerを削除する
	/// </summary>
	static void DeleteCollisionManagerInstance();

	void Update();
	void Draw();

	/// <summary>
	/// Collisionインスタンスをリストに入れる　※Objectを継承したインスタンスをCollisionBaseを継承したクラスをnewすることによって、リストに入る
	/// </summary>
	/// <param name="_obj"> newされたCollisionインスタンス </param>
	void PushCollisionObject(CollisionBase* _obj);

	/// <summary>
	/// PushCollisionObjectの要素をリストから削除
	/// </summary>
	/// <param name="_obj"> Collisionインスタンス </param>
	void PopCollisionObject(CollisionBase* _obj);

	/// <summary>
	/// 当たり判定の処理を行うCollisionインスタンスをリストに入れる
	/// </summary>
	/// <param name="_obj"> Collisionインスタンス </param>
	void PushDoCollisionObject(CollisionBase* _obj);

	/// <summary>
	/// PushDoCollisionObjectの要素を削除
	/// </summary>
	/// <param name="_obj"> Collisionインスタンス </param>
	void PopDoCollisionObject(CollisionBase* _obj);

private:

	CollisionManager();
	~CollisionManager();

#if USE_NEAREST_PAIR_CONTAINER
	/// <summary>
	///							CollisionBase*同士のペアを作成し、返す
	/// </summary>
	/// <param name="_coll1">	CollisionBase*１	</param>
	/// <param name="_coll2">	CollisionBase*２	</param>
	/// <returns>				coll1とcoll2のペア	</returns>
	std::pair<CollisionBase*, CollisionBase*> GetMakeKey(CollisionBase* _coll1, CollisionBase* _coll2);
#else
	struct PairData
	{
		CollisionBase* me;		// 自身の当たり判定オブジェクト
		CollisionBase* target;	// 相手の当たり判定オブジェクト

		PairData(CollisionBase* _me, CollisionBase* _target) : me(_me), target(_target) {}
	};

	/// <summary>
	///							CollisionBase*同士のペアを作成し、返す
	/// </summary>
	/// <param name="_coll1">	CollisionBase*１			</param>
	/// <param name="_coll2">	CollisionBase*２			</param>
	/// <returns>				coll1とcoll2のペア PairData	</returns>
	PairData GetPair(CollisionBase* _coll1, CollisionBase* _coll2);
#endif
	/// <summary>
	///									当たり判定イベント
	/// </summary>
	/// <param name="hitObjList">		当たったオブジェクトコンテナ			</param>
	/// <param name="coll1">			当たり判定オブジェクト1					</param>
	/// <param name="coll2">			当たり判定オブジェクト2					</param>
	/// <param name="deleteCheckList">	削除されたかチェックするコンテナ		</param>
	/// <param name="itr1">				当たり判定オブジェクト1のイテレーター	</param>
	/// <param name="itr2">				当たり判定オブジェクト1のイテレーター	</param>
	void HitEvent(std::unordered_map<CollisionBase*, std::unordered_set<COLLISION_OBJECT_KIND>>& hitObjList, CollisionBase*& coll1, CollisionBase*& coll2, std::unordered_set<CollisionBase*>& deleteCheckList, std::list<CollisionBase*>::iterator& itr1, std::list<CollisionBase*>::iterator& itr2);
	
	/// <summary>
	///								deleteCollisionに登録されたiteratorをつかって引数のコンテナの要素をerase
	/// </summary>
	/// <param name="_list">		eraseしたいコンテナ							</param>
	/// <param name="deleteMeList">	要素のCollisionBaseをdeleteするかどうか		</param>
	void DeleteCollisionListDelete(std::list<CollisionBase*>& _list, bool deleteMeList = false);

	/// <summary>
	///							Collisionインスタンス1 or 2のCOLLISION_OBJECT_KINDと引数のCOLLISION_OBJECT_KINDが一致する場合、一致するCollisionインスタンスを返す
	/// </summary>
	/// <param name="_obj1">	Collisionインスタンス1				</param>
	/// <param name="_obj2">	Collisionインスタンス2				</param>
	/// <param name="_kind">	COLLISION_OBJECT_KIND				</param>
	/// <returns>				一致したCollisionインスタンス		</returns>
	CollisionBase* GetCollisionKindSame(CollisionBase* _obj1,CollisionBase* _obj2, COLLISION_OBJECT_KIND _kind);

	/// <summary>
	///						deleteCollisionが引数のイテレータを持っているか返す
	/// </summary>
	/// <param name="_itr">	調べたいstd::list<CollisionBase*>::iterator		</param>
	/// <returns>			true:持っている / false:持っていない			</returns>
	bool HasDeleteCollisionItr(const std::list<CollisionBase*>::iterator _itr);

	/// <summary>
	///									当たり判定の種類から、バウンディング対象か返す
	/// </summary>
	/// <param name="_collisionKind">	当たり判定の種類													</param>
	/// <returns>						true:バウンディング対象である / false:バウンディング対象でない		</returns>
	bool IsBoundingCollisionKind(const COLLISION_KIND& _collisionKind);

	static CollisionManager* object;		// このクラスのインスタンス
	std::list<CollisionBase*> collision;	// すべてのCollisionインスタンスを保存するコンテナ

	std::list<CollisionBase*> doCollision;	// 当たり判定を行うCollisionインスタンスを保存するコンテナ

	std::list<std::list<CollisionBase*>::iterator> deleteCollision;	// 削除するCollisionインスタンスのイテレータを保存するコンテナ

	bool draw[10];   // デバッグ用の描画フラグ
	bool keyPut[10]; // デバッグ用のキー入力連続防止フラグ
};
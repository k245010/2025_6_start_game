#pragma once
#include <list>
#include <Dxlib.h>
#include <unordered_map>
#include <set>
#include "../Collisions/CollisionBase.h"
#include "../CollisionFunction.h"


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
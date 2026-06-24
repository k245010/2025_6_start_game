#pragma once
#include <list>
#include <Dxlib.h>
#include "collisionBase.h"

class GameManager;
class CollisionBase;
class CollisionManager
{
public:

	CollisionManager();
	~CollisionManager();

	/// <summary>
	/// 静的メソッドでthisを返す
	/// </summary>
	/// <returns> this </returns>
	static CollisionManager* GetCollisionManagerObject();
	
	/// <summary>
	/// 静的メソッドでCollisionManagerを削除する
	/// </summary>
	static void DeleteCollisionManagerObject();

	void Update();
	void Draw();

	/// <summary>
	/// Collisionインスタンスをリストに入れる　※Objectを継承したインスタンスをCollisionBaseを継承したクラスをnewすることによって、リストに入る
	/// </summary>
	/// <param name="_obj"> newされたCollisionインスタンス </param>
	void CreateCollisionObject(CollisionBase* _obj);

	/// <summary>
	/// CreateCollisionObjectの要素を削除
	/// </summary>
	/// <param name="_obj"></param>
	void DeleteCollisionObject(CollisionBase* _obj);

	/// <summary>
	/// 当たり判定の処理を行うCollisionインスタンスをリストに入れる　※画面内のObjectの当たり判定を行う時にリスト入る
	/// </summary>
	/// <param name="_obj"> 画面内のCollisionインスタンス </param>
	void PushDoCollisionObject(CollisionBase* _obj);

	/// <summary>
	/// PushDoCollisionObjectの要素を削除
	/// </summary>
	/// <param name="_obj"></param>
	void DeleteDoCollisionObject(CollisionBase* _obj);

private:

	/// <summary>
	/// Collisionインスタンスが例外処理かどうか返す　※例外処理は画面外でもいつでも、当たり判定を行う
	/// </summary>
	/// <param name="_obj"> Collisionインスタンス </param>
	/// <returns> 例外:例外でない </returns>
	bool IsExceptionCollisionObject(const CollisionBase& _obj);

	/// <summary>
	/// CollisionインスタンスのCOLLISION_OBJECT_KINDと引数のCOLLISION_OBJECT_KINDが一致するかどうか返す
	/// </summary>
	/// <param name="_obj"> Collisionインスタンス </param>
	/// <param name="_kind"> COLLISION_OBJECT_KIND </param>
	/// <returns> 一致する:一致しない </returns>
	bool IsCollisionObjectKindSame(const CollisionBase& _obj, COLLISION_OBJECT_KIND _kind);

	/// <summary>
	/// Collisionインスタンス1 or 2 のCOLLISION_OBJECT_KINDと引数のCOLLISION_OBJECT_KINDが一致するかどうか返す
	/// </summary>
	/// <param name="_obj1"> Collisionインスタンス1 </param>
	/// <param name="_obj2"> Collisionインスタンス2 </param>
	/// <param name="_kind"> COLLISION_OBJECT_KIND </param>
	/// <returns> 一致する:一致しない </returns>
	bool IsCollisionObjectKindSame(const CollisionBase& _obj1, const CollisionBase& _obj2, COLLISION_OBJECT_KIND _kind);

	/// <summary>
	/// Collisionインスタンス1 or 2のCOLLISION_OBJECT_KINDと引数のCOLLISION_OBJECT_KINDが一致する場合、一致するCollisionインスタンスを返す
	/// </summary>
	/// <param name="_obj1"> Collisionインスタンス1 </param>
	/// <param name="_obj2"> Collisionインスタンス2 </param>
	/// <param name="_kind"> COLLISION_OBJECT_KIND </param>
	/// <returns> 一致したCollisionインスタンス </returns>
	CollisionBase* GetCollisionKindSame(CollisionBase* _obj1,CollisionBase* _obj2, COLLISION_OBJECT_KIND _kind);

	static CollisionManager* object;
	std::list<CollisionBase*> collision;

	std::list<CollisionBase*> doCollision;

	bool obbInit;

	bool draw[10];
	bool keyPut[10];
};
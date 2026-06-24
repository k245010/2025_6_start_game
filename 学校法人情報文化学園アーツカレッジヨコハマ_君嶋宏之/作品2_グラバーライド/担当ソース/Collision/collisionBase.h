#pragma once
#include "Object.h"
#include "opereter.h"
#include "collisionFunction.h"
#include <list>
#include "player.h"
#include "stageObjectManager.h"

constexpr float DEFAULT_CHECK_CAMERA_COLL_SIZE = 2.0f;

/// <summary>
/// 当たり判定オブジェクト　自分のオブジェクトの種類
/// </summary>
enum class COLLISION_OBJECT_KIND
{
	NONE = 0,
	HEAD,
	LAND,	
	PLAYER,
	PLAYER_ATTACK,
	PLAYER_RAY,
	ENEMY,
	ENEMY_ATTACK,
	BLOCK,
	BLOCK_SPIKE,
	BLOCK_GOAL,
	BLOCK_POINT,
	BLOCK_GRAVITY,
	BLOCK_AIR_BARRIER,
	BLOCK_SWITCH,
	BLOCK_BUMPER,
	BLOCK_RAIL,
	BLOCK_IRON_DOOR,
	BLOCK_BROKEN,
	BLOCK_GRAVITY_AREA,
	BLOCK_COIN,
	ITEM_HEAL,
	ITEM_GHEAL,
	CAMERA_AREA,

	MAX
};

/// <summary>
/// 当たり判定を行う処理の種類
/// </summary>
enum class COLLISION_KIND
{
	SPHERE = 0,
	OBB,
	RAY,
};

class Player;
class SphereCollision;
class OBBCollision;
class CollisionBase
{
public:

	CollisionBase(Object* _obj);
	~CollisionBase();

	virtual void Update();
	virtual void Draw() = 0;
	void DrawBase();

	/// <summary>
	/// 当たったかどうかを返す
	/// </summary>
	/// <param name="col"> CollisionBaseオブジェクト </param>
	/// <returns> あたった:当たってない </returns>
	bool IsHit(CollisionBase* col);

	/// <summary>
	/// 自分が当たり判定で当たっていると判断されたか返す
	/// </summary>
	/// <returns> 当たっている:当たっていない </returns>
	bool GetIsHitMe() { return isHitMe; }

	/// <summary>
	/// 当たったか判定をfalseにリセットする
	/// </summary>
	void ResetIsHitMe() { isHitMe = false; }

	VECTOR GetPosition() const
	{
		if (tagMe == COLLISION_OBJECT_KIND::PLAYER || tagMe == COLLISION_OBJECT_KIND::ENEMY)
		{
			return dynamic_cast<CharaBase*>(object)->GetCenterPosition();
		}
		return object->GetPosition();
	}
	VECTOR GetSize() const { return object->GetSize(); }

	void SetPosition(VECTOR set) { object->CollisionForSetPosition(set); }

	virtual bool CollisionToSphere(SphereCollision* col) = 0;
	virtual bool CollisionToOBB(OBBCollision* col) = 0;
	virtual bool CollisionToRay(RayCollision* col) = 0;

	/// <summary>
	/// 自分の当たり判定オブジェクトの種類を設定
	/// </summary>
	/// <param name="_kind"> 当たり判定オブジェクトの種類 </param>
	void SetTagMe(COLLISION_OBJECT_KIND _kind) { tagMe = _kind; }

	/// <summary>
	/// 自分に当たり判定を行いたい相手の、当たり判定オブジェクトの種類を設定
	/// </summary>
	/// <param name="_kind"> 相手の当たり判定オブジェクトの種類 </param>
	void SetTargetTag(COLLISION_OBJECT_KIND _kind);

	/// <summary>
	/// 自分の当たり判定オブジェクト設定を削除
	/// </summary>
	void DeleteTagMe() { tagMe = COLLISION_OBJECT_KIND::NONE; }

	/// <summary>
	/// 自分に当たり判定を行いたい相手の当たり判定オブジェクト設定を削除
	/// </summary>
	/// <param name="_kind"></param>
	void DeleteTargetTag(COLLISION_OBJECT_KIND _kind);

	/// <summary>
	/// 自分の当たり判定オブジェクトの種類を返す
	/// </summary>
	/// <returns> 当たり判定オブジェクト </returns>
	COLLISION_OBJECT_KIND GetTagMe() const { return tagMe; }

	/// <summary>
	/// 渡された引数に対し自分はそのCOLLISION_OBJECT_KINDをもっているかどうか
	/// </summary>
	/// <param name="_kind"> 判断したい当たり判定オブジェクトの種類 </param>
	/// <returns> 持っている:持っていない </returns>
	bool IsTargetTag(COLLISION_OBJECT_KIND _kind);

	/// <summary>
	/// 当たり判定の種類を返す
	/// </summary>
	/// <returns> Ray or Sphere or OBB </returns>
	COLLISION_KIND GetCollisionKind() const { return kind; };

	inline void SetLastPos()
	{
		if (object->GetObjectType() == (int)ObjectType::PLAYER)
		{
			lastPosition = dynamic_cast<Player*>(object)->GetCenterPosition();

			lastPosition = object->GetPosition();
		}
		else
		{
			lastPosition = object->GetPosition();
		}
	};

	/// <summary>
	/// 1フレーム前のオブジェクトの座標を返す
	/// </summary>
	/// <returns> 1フレーム前の座標 </returns>
	VECTOR GetLastPosition() const { return lastPosition; };

	/// <summary>
	/// 地面に着地している
	/// </summary>
	void SetGroundLanding() { dynamic_cast<Player*>(object)->GroundLanding(true); }

	//void Landing() { object->Landing(); }
	//void NoLanding() { object->NoLanding(); }

	/// <summary>
	/// 地面に着地していない
	/// </summary>
	void NoGround() { dynamic_cast<Player*>(object)->NoGround(); }

	/// <summary>
	/// 死亡している
	/// </summary>
	void CharaDead() { dynamic_cast<Player*>(object)->Dead(); }

	/// <summary>
	/// 当たり判定を行い当たっていたら呼び出される関数(自分と当たった相手の情報を持っている)
	/// </summary>
	/// <param name="me"> 自分の当たり判定情報 </param>
	/// <param name="target"> 相手の当たり判定情報 </param>
	void CollisionEvent(CollisionBase& me, CollisionBase& target) { object->CollisionEvent(me, target);isHitMe = true; }

	/// <summary>
	/// Updateの停止、運転を設定する
	/// </summary>
	/// <param name="set"> 運転:停止 </param>
	void SetVisionMove(bool set) { object->SetVisionMove(set); }

	/// <summary>
	/// 自分のオブジェクト変数を返す
	/// </summary>
	/// <returns> 自分のオブジェクト変数 </returns>
	Object* GetObjectMe() const { return object; }

	//void SetCollisionItr(std::list<CollisionBase*>::iterator itr) const { *collisionItrMe = *itr; }

protected:

	Object* object;
	COLLISION_OBJECT_KIND tagMe;
	std::list<COLLISION_OBJECT_KIND> targetTag;
	
	bool doCollisionPush;
	COLLISION_KIND kind;

	VECTOR lastPosition;
	bool isHitMe; // 当たっているかのフラグ

private:

	void DoCollisionPushMe();
	void DoCollisionDeleteMe();
};
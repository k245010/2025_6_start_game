#pragma once
#include "../../Object3D/Object3D.h"
#include "../CollisionFunction.h"
#include "../CollisionObjectKind.h"
#include <list>
#include <unordered_set>
#include <functional>
#include <array>
#include <optional>
#include <set>

/// <summary>
/// 当たり判定を行う処理の種類
/// </summary>
enum class COLLISION_KIND
{
	SPHERE = 0,
	OBB,
	RAY,
	MODEL,
};

/// <summary>
/// 当たり判定用のトランスフォーム
/// </summary>
struct CollisionTransform
{
	Transform* trans;		// トランスフォームポインタ
	VECTOR3 lastPosition;	// １フレーム前の座標

	CollisionTransform() : trans(nullptr), lastPosition(VZero) {};
};

/// <summary>
/// 当たったときに与えるダメージ情報を保持する構造体
/// </summary>
struct CollisionHitDamageData
{
	COLLISION_OBJECT_KIND targetKind;	// 当たったときにダメージを与える相手の当たり判定オブジェクトの種類
	float targetDamage;					// 与えるダメージ量

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_kind"> 当たったときにダメージを与える相手の当たり判定オブジェクトの種類 </param>
	/// <param name="_damage"> 与えるダメージ量 </param>
	CollisionHitDamageData(COLLISION_OBJECT_KIND _kind = COLLISION_OBJECT_KIND::NONE, float _damage = 0.0f)	: targetKind(_kind), targetDamage(_damage) {}
};

/// <summary>
/// 当たった場所の情報構造体
/// </summary>
struct CollisionHitPointData
{
	std::vector<VECTOR3> hitPosition;		// 当たった座標
	std::vector<VECTOR3> hitNormList;		// 当たったポリゴンの法線コンテナ(モデルの当たり判定で使われる)

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_hitPos"> 当たった座標 </param>
	/// <param name="_hitNormList"> 当たったポリゴンの法線コンテナ </param>
	CollisionHitPointData(const std::vector<VECTOR3>& _hitPos, const std::vector<VECTOR3>& _hitNormList = {}) : hitPosition(_hitPos), hitNormList(_hitNormList) {}
};

/// <summary>
/// 当たり判定で当たったときの処理をまとめた構造体
/// </summary>
struct CollisionHitInfoData
{
	std::string ownerName;								// 自分の名前
	COLLISION_KIND kind;								// 自分の当たり判定の種類
	COLLISION_OBJECT_KIND tag;							// 自分の当たり判定オブジェクト種類
	bool onDeleteMe;									// 当たり判定が通った時に、自身の当たり判定のインスタンスを削除するフラグ
	Transform transform;								// 自身のトランスフォーム

	std::optional<CollisionHitPointData> hitPointData;	// 当たった場所の情報構造体

	std::string targetOwnerName;						// 当たった相手の名前
	
	std::list<CollisionHitDamageData> targetDamageData; // 当たり判定の種類とダメージ量　当たった相手へダメージを与える

	/// <summary>
	/// 指定された当たり判定オブジェクトの種類に対応するダメージ値を返す
	/// </summary>
	/// <param name="_kind"> 当たり判定オブジェクトの種類 </param>
	/// <returns> 該当する種類に対するダメージ量　見つからない場合は -1.0f を返す </returns>
	const float GetDamage(const COLLISION_OBJECT_KIND& _kind) const
	{
		for (auto itr = targetDamageData.begin(); itr != targetDamageData.end();itr++)
		{
			const CollisionHitDamageData& data = *itr;

			if (data.targetKind == _kind)
				return data.targetDamage;
		}
		//assert(false && "ダメージ量が設定されていません");
		return -1.0f;
	}

	CollisionHitInfoData() : ownerName(""), kind(COLLISION_KIND::SPHERE), tag(COLLISION_OBJECT_KIND::NONE), onDeleteMe(false), transform(), targetOwnerName(""), targetDamageData() {}
	//CollisionBase* thisPointer; // 個人の識別のため　（考え中.. 今後GameObjectのTag管理とかいいかも）
};

class SphereCollision;
class OBBCollision;
class RayCollision;
class ModelCollision;
/// <summary>
/// 当たり判定の基底クラス
/// </summary>
class CollisionBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="_trans">			トランスフォームポインタ												</param>
	/// <param name="_tagMe">			自身の当たり判定オブジェクトタグ										</param>
	/// <param name="_ownerName">		自身の当たり判定オブジェクト名											</param>
	/// <param name="func">				当たったときに呼ぶ関数													</param>
	/// <param name="_onHitDeleteMe">	当たったときに自身の当たり判定オブジェクトを非アクティブにするかどうか　
	///									true:非アクティブにする / false:非アクティブにしない					</param>
	CollisionBase(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName = "", std::function<bool(const CollisionHitInfoData&)> func = nullptr, bool _onHitDeleteMe = false);
	virtual ~CollisionBase();

	virtual void Update();
	virtual void Draw() = 0;

	/// <summary>
	///						当たったかどうかを返す
	/// </summary>
	/// <param name="col">	CollisionBaseオブジェクト </param>
	/// <returns>			当たった:当たってない </returns>
	bool IsHit(CollisionBase* col);

	/// <summary>
	///							当たった相手を保存する
	/// </summary>
	/// <param name="_kind">	当たり判定オブジェクト </param>
	void SetHitForTargetObjectKind(const COLLISION_OBJECT_KIND& _kind) { hitTargetTag.emplace(_kind); }

	/// <summary>
	///							当たった相手を保存するために、当たった当たり判定オブジェクトのコンテナを代入する
	/// </summary>
	/// <param name="_kindSet"> 当たった当たり判定オブジェクトのコンテナ </param>
	void SetHitForTargetObjectKind(std::unordered_set<COLLISION_OBJECT_KIND>& _kindSet) { hitTargetTag = _kindSet; }

	/// <summary>
	///				当たった相手を保存したコンテナを返す
	/// </summary>
	/// <returns>	当たった当たり判定オブジェクトのコンテナ </returns>
	const std::unordered_set<COLLISION_OBJECT_KIND>& GetHitForTargetObjectKind() const { return hitTargetTag; }

	/// <summary>
	///				トランスフォームポインタを返す
	/// </summary>
	/// <returns>	トランスフォームポインタ	</returns>
	const Transform* GetTransform() const { return collTransform.trans; }

	/// <summary>
	///							トランスフォームを設定する
	/// </summary>
	/// <param name="_trans">	設定するトランスフォーム	</param>
	void SetTransform(const Transform& _trans) { *collTransform.trans = _trans; }

	/// <summary>
	///				当たり判定の座標を返す
	/// </summary>
	/// <returns>	当たり判定の座標		</returns>
	VECTOR3 GetPosition() const { return collTransform.trans->position; }

	/// <summary>
	///				当たり判定のサイズを返す
	/// </summary>
	/// <returns>	当たり判定のサイズ		</returns>
	VECTOR3 GetSize() const { return collTransform.trans->size; }

	/// <summary>
	///				当たり判定のスケールを返す
	/// </summary>
	/// <returns>	当たり判定のスケール	</returns>
	VECTOR3 GetScale() const { return collTransform.trans->scale; }

	/// <summary>
	///				当たり判定の回転を返す
	/// </summary>
	/// <returns>	当たり判定の回転	</returns>
	VECTOR3 GetRotation() const { return collTransform.trans->rotation; }

	/// <summary>
	///						自身と球の当たり判定をする
	/// </summary>
	/// <param name="col">	SphereCollisionクラスポインタ				</param>
	/// <returns>			true:当たっている / false:当たっていない	</returns>
	virtual bool CollisionToSphere(SphereCollision* col) = 0;

	/// <summary>
	///						自身とOBBの当たり判定をする
	/// </summary>
	/// <param name="col">	OBBCollisionクラスポインタ					</param>
	/// <returns>			true:当たっている / false:当たっていない	</returns>
	virtual bool CollisionToOBB(OBBCollision* col) = 0;

	/// <summary>
	///						自身とレイの当たり判定をする
	/// </summary>
	/// <param name="col">	RayCollisionクラスポインタ					</param>
	/// <returns>			true:当たっている / false:当たっていない	</returns>
	virtual bool CollisionToRay(RayCollision* col) = 0;

	/// <summary>
	///						自身とモデルの当たり判定をする
	/// </summary>
	/// <param name="col">	ModelCollisionクラスポインタ				</param>
	/// <returns>			true:当たっている / false:当たっていない	</returns>
	virtual bool CollisionToModel(ModelCollision* col) = 0;

	/// <summary>
	///							自分の当たり判定オブジェクトの種類を設定
	/// </summary>
	/// <param name="_kind">	当たり判定オブジェクトの種類	</param>
	void SetTagMe(COLLISION_OBJECT_KIND _kind) { collisionHitInfoData.tag = _kind; }

	/// <summary>
	///							当たり判定を行いたい相手の、当たり判定オブジェクトの種類を設定
	/// </summary>
	/// <param name="_kind">	相手の当たり判定オブジェクトの種類 </param>
	bool SetTargetTag(COLLISION_OBJECT_KIND _kind);

	/// <summary>
	///							当たり判定を行いたい相手の、当たり判定オブジェクトの種類と相手に与えるダメージ量を設定
	/// </summary>
	/// <param name="_kind">	当たり判定オブジェクトの種類	</param>
	/// <param name="_damage">	相手に与えるダメージ量			</param>
	bool SetTargetTag(COLLISION_OBJECT_KIND _kind,float _damage);

	/// <summary>
	/// 自分の当たり判定オブジェクト設定を削除 
	/// </summary>
	void DeleteTagMe() { collisionHitInfoData.tag = COLLISION_OBJECT_KIND::NONE; }

	/// <summary>
	///							自分に当たり判定を行いたい相手の当たり判定オブジェクト設定を削除
	/// </summary>
	/// <param name="_kind">	削除する当たり判定オブジェクトの種類	</param>
	void DeleteTargetTag(COLLISION_OBJECT_KIND _kind);

	/// <summary>
	///				自分の当たり判定オブジェクトの種類を返す
	/// </summary>
	/// <returns>	当たり判定オブジェクトの種類 </returns>
	COLLISION_OBJECT_KIND GetTagMe() const { return collisionHitInfoData.tag; }

	/// <summary>
	///							自身と当たり判定する相手のCOLLISION_OBJECT_KINDに対し、引数に渡されたCOLLISISON_OBJECT_KINDが存在するか返す
	/// </summary>
	/// <param name="_kind">	判断したい当たり判定オブジェクトの種類		</param>
	/// <returns>				true:持っている / false:持っていない		</returns>
	bool HasTargetTag(COLLISION_OBJECT_KIND _kind);

	/// <summary>
	///				当たり判定の種類を返す
	/// </summary>
	/// <returns>	当たり判定を行う処理の種類 </returns>
	COLLISION_KIND GetCollisionKind() const { return kind; };

	/// <summary>
	///				1フレーム前のオブジェクトの座標を返す
	/// </summary>
	/// <returns>	1フレーム前の座標 </returns>
	VECTOR3 GetLastPosition() const { return collTransform.lastPosition; };

	/// <summary>
	///							当たった時の関数を実行する  当たり判定を行い、当たっていたらマネージャーが呼び出す関数
	/// </summary>
	/// <param name="me">		自分の当たり判定情報 </param>
	/// <param name="target">	相手の当たり判定情報 </param>
	bool CollisionEvent(const CollisionHitInfoData& me, const CollisionHitInfoData& target);

	/// <summary>
	/// 次のフレームで削除
	/// </summary>
	void DeleteMe() { collisionHitInfoData.onDeleteMe = true; }

	/// <summary>
	///				自身が削除されるどうか
	/// </summary>
	/// <returns>	true:削除する / false:削除しない </returns>
	bool IsDeleteMe() const { return collisionHitInfoData.onDeleteMe; }

	/// <summary>
	///				自身のCollisionHitDamageDataを返す
	/// </summary>
	/// <returns>	CollisionHitInfoData		</returns>
	const CollisionHitInfoData& GetCollisionHitInfoData() { return collisionHitInfoData; }

	/// <summary>
	///						アクティブ状態を設定 （当たり判定を行うかどうかを設定）
	/// </summary>
	/// <param name="set">	設定するアクティブ状態	</param>
	void SetActive(bool set) { isActive = set; }

	/// <summary>
	///				アクティブかどうかを返す
	/// </summary>
	/// <returns>	true:アクティブ / false:非アクティブ </returns>
	bool IsActive() const { return isActive; }

	/// <summary>
	///				はじめの当たり判定を終えたかどうか （初期化後に当たり判定を一度終えているかどうかの確認用）
	/// </summary>
	/// <returns>	true:判定を終えた / false:判定を終えていない </returns>
	bool IsFirstCollided() const { return isFirstCollided; }

protected:					

	std::list<COLLISION_OBJECT_KIND> targetTag;						// 当たり判定をする相手の当たり判定オブジェクトの種類
	std::unordered_set<COLLISION_OBJECT_KIND> hitTargetTag;			// 当たった相手の当たり判定オブジェクトの種類を保存するコンテナ


	std::function<bool(const CollisionHitInfoData&)> collFunction;	// 当たったときに呼ぶ関数
	CollisionHitInfoData collisionHitInfoData;	// 当たったときの情報

	CollisionTransform collTransform;			// 当たり判定用のトランスフォーム
	COLLISION_KIND kind;						// 当たり判定の種類

	bool doCollisionPush = false;				// 当たり判定を行うコンテナに入れるかどうか
	//bool onDeleteMe;							// 自身を削除するかどうか
	bool isActive;								// 当たり判定を行うかどうか
	bool isFirstCollided;						// 最初のフレームに入ったかどうか

private:

	/// <summary>
	/// 当たり判定を行うコンテナに自身を入れる
	/// </summary>
	void DoCollisionPushMe();

	/// <summary>
	/// 当たり判定を行うコンテナに自身を削除
	/// </summary>
	void DoCollisionDeleteMe();
};

